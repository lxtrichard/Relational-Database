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
    switch (theKeyword)
    {
    case Keywords::create_kw: theStatement = new CreateStatement(*this); break;
    case Keywords::show_kw: theStatement = new ShowStatement(*this); break;
    case Keywords::describe_kw: theStatement = new DescribeStatement(*this); break;
    case Keywords::drop_kw: theStatement = new DropStatement(*this); break;
    default: break;
    }
    if (theStatement) {
      StatusResult theResult = theStatement->parse(aTokenizer);
      if (!theResult) {
        delete theStatement;
        theStatement = nullptr;
      }
    }
    if (theStatement) {
      return this;
    }
    return nullptr;
  }

  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer,
                                       StatusResult &aResult) {
    return theStatement;
  }

  StatusResult  SQLProcessor::run(Statement *aStmt) {
    SQLStatement *theStatement = dynamic_cast<SQLStatement*>(aStmt);
    switch (theStatement->getType()) {
    case Keywords::create_kw:
      return createTable(theEntity);
    case Keywords::drop_kw:
      return dropTable(theStatement->getTableName());
    case Keywords::describe_kw:
      return describeTable(theStatement->getTableName());
    case Keywords::show_kw:
      return showTables();
    default: break;
    }
    return StatusResult{ Errors::notImplemented };
  }

  StatusResult  SQLProcessor::createTable(Entity *anEntity){
    if (activeDB) {
      return activeDB->createTable(output, *anEntity);
    }
    return StatusResult{Errors::noError};
  }

  StatusResult  SQLProcessor::describeTable(const std::string &aName){
    if (activeDB) {
      activeDB->describeTable(output, aName);
    }
    return StatusResult{ Errors::noError };
  }

  StatusResult  SQLProcessor::dropTable(const std::string &aName){
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
}
