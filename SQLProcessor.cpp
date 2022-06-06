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

  Statement* SQLProcessor::showFactory(Tokenizer &aTokenizer, Database* aDB){
    Keywords theKeyword = aTokenizer.peek(1).keyword;
    switch (theKeyword)
    {
    case Keywords::tables_kw:
      return new ShowStatement(aDB);
    case Keywords::indexes_kw:
      return new IndexesStatement(aDB);
    case Keywords::index_kw:
      return new IndexStatement(aDB);
    default:
      return nullptr;
    }
  }

  CmdProcessor* SQLProcessor::recognizes(Tokenizer &aTokenizer) {
    if (activeDB==nullptr) {
      std::cout << "NO DATABASE IS LOADED!\n" << std::endl;
      return nullptr;
    }
    Keywords theKeyword = aTokenizer.current().keyword;
    theStatement = nullptr;
    // Define a factory of Statement objects
    std::map<Keywords, std::function<Statement* ()>> theStmtFactory{
      {Keywords::create_kw,   [&]() { return new CreateStatement(activeDB);}},            
      {Keywords::show_kw,     [&]() { return showFactory(aTokenizer, activeDB);}}, // show entry
      {Keywords::describe_kw, [&]() { return new DescribeStatement(activeDB);}},
      {Keywords::drop_kw,     [&]() { return new DropStatement(activeDB); }},
      {Keywords::insert_kw,   [&]() { return new InsertStatement(activeDB); }},
      {Keywords::select_kw,   [&]() { return new SelectStatement(activeDB); }},
      {Keywords::update_kw,   [&]() { return new UpdateStatement(activeDB); }},
      {Keywords::delete_kw,   [&]() { return new DeleteStatement(activeDB); }},
    };
    if (theStmtFactory.count(theKeyword)) {
      theStatement = theStmtFactory[theKeyword]();
    }
    if (theStatement) {
      return this;
    }
    delete theStatement;
    theStatement = nullptr;
    return nullptr;
  }

  Statement* SQLProcessor::makeStatement(Tokenizer &aTokenizer,
                                       StatusResult &aResult) {
    aResult = theStatement->parse(aTokenizer);
    return theStatement;
  }

  StatusResult  SQLProcessor::run(Statement *aStmt) {
    StatusResult theResult = aStmt->run(output);
    return theResult;
  }
}
