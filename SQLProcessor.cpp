//
//  SQLProcessor.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#include "SQLProcessor.hpp"
#include "SQLStatement.hpp"
#include "BlockIO.hpp"
#include "DBProcessor.hpp"

namespace ECE141 {


  SQLProcessor::SQLProcessor(std::ostream &anOutput)
    :  CmdProcessor(anOutput) {
      activeDB = nullptr;
      theStatement = nullptr;
      theEntity = nullptr;
  }

  SQLProcessor::~SQLProcessor() {}

  SQLProcessor* SQLProcessor::setActiveDB(Database *aDB) {
    activeDB = aDB;
    return this;
  }
  SQLProcessor* SQLProcessor::setEntity(Entity* aEntity) {
    theEntity = aEntity;
    return this;
  }

  CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
    if (activeDB==nullptr) {
      std::cout << "NO DATABASE IS LOADED!\n" << std::endl;
      return nullptr;
    }
    Keywords theKeyword = aTokenizer.current().keyword;
    // Define a factory of Statement objects
    std::map<Keywords, std::function<Statement* ()>> theStmtFactory{
      {Keywords::create_kw,   [&]() { return new CreateStatement();}},            
      {Keywords::show_kw,     [&]() { return new ShowStatement();}},
      {Keywords::describe_kw, [&]() { return new DescribeStatement();}},
      {Keywords::drop_kw,     [&]() { return new DropStatement(); }},
      {Keywords::insert_kw,   [&]() { return new InsertStatement(); }},
      {Keywords::select_kw,   [&]() { return new SelectStatement(); }}
    };
    if (theStmtFactory.count(theKeyword)) {
      theStatement = theStmtFactory[theKeyword]();
    }
    if (theStatement) {
      StatusResult theResult = theStatement->parse(aTokenizer);
      if (theResult) {
        return this;
      }
    }
    delete theStatement;
    theStatement = nullptr;
    return nullptr;
  }

  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer,
                                       StatusResult &aResult) {
    return theStatement;
  }

  StatusResult  SQLProcessor::run(Statement *aStmt) {
    switch (aStmt->getType()) {
      case Keywords::create_kw:     return createTable(aStmt);
      case Keywords::describe_kw:   return describeTable(aStmt);
      case Keywords::drop_kw:       return dropTable(aStmt);
      case Keywords::show_kw:       return showTables();
      case Keywords::insert_kw:     return insertRows(aStmt);
      case Keywords::select_kw:     return selectRows(aStmt);
      default: break;
    }
    return StatusResult{ Errors::notImplemented };
  }

  StatusResult  SQLProcessor::createTable(Statement *aStmt){
    SQLStatement *theStatement = dynamic_cast<SQLStatement*>(aStmt);
    theEntity = new Entity(theStatement->getTableName());
    for (auto& attr : theStatement->getAttributes()) {
      theEntity->addAttribute(attr);
    }
    if (activeDB) {
      return activeDB->createTable(output, *theEntity);
    }
    return StatusResult{Errors::noError};
  }

  StatusResult  SQLProcessor::describeTable(Statement *aStmt){
    SQLStatement *theStatement = dynamic_cast<SQLStatement*>(aStmt);
    const std::string &aName = theStatement->getTableName();
    if (activeDB) {
      activeDB->describeTable(output, aName);
    }
    return StatusResult{ Errors::noError };
  }

  StatusResult  SQLProcessor::dropTable(Statement *aStmt){
    SQLStatement *theStatement = dynamic_cast<SQLStatement*>(aStmt);
    const std::string &aName = theStatement->getTableName();
    if (activeDB) {
      activeDB->dropTable(output, aName);
    }
    return StatusResult{ Errors::noError };
  }

  StatusResult  SQLProcessor::showTables(){
    if (activeDB) {
      activeDB->showTables(output);
    }
    return StatusResult{ Errors::noError };
  }

  StatusResult  SQLProcessor::insertRows(Statement * aStmt){
    InsertStatement* theStatement = dynamic_cast<InsertStatement*>(aStmt);
    if (activeDB) {
      return activeDB->insertRows(output, 
                theStatement->getTableName(),
                theStatement->getAttributes(),
                theStatement->getValues());
    }
    return StatusResult{ Errors::noError };
  }

  StatusResult  SQLProcessor::selectRows(Statement * aStmt){
    SelectStatement* theStatement = dynamic_cast<SelectStatement*>(aStmt);
    if (activeDB) {
     return activeDB->selectRows(output, theStatement->getQuery());
    }
    return StatusResult{ Errors::noError };
  }
}
