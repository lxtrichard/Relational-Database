//
//  DBProcessor.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "DBProcessor.hpp"

#include <map>
#include <fstream>
#include "Config.hpp"
#include "DBProcessor.hpp"
#include "FolderView.hpp"
#include "FolderReader.hpp"
#include "Database.hpp"
#include "Application.hpp"
#include "Helpers.hpp"
#include "DBStatement.hpp"


namespace ECE141 {

  //like a typedef!!!
  using StmtFactory = Statement* (*)(DBProcessor *aProc,
                                   Tokenizer &aTokenize);

  DBProcessor::DBProcessor(std::ostream &anOutput)
    : CmdProcessor(anOutput) {
      activeDB = nullptr;
  }
  
  DBProcessor::~DBProcessor() {
  }
 
  
  //CmdProcessor interface ...
  bool DBProcessor::isKnown(Keywords aKeyword) {
    static Keywords theKnown[]=
      { Keywords::create_kw,Keywords::drop_kw, 
        Keywords::show_kw, Keywords::use_kw, 
        Keywords::dump_kw };
    auto theIt = std::find(std::begin(theKnown),
                           std::end(theKnown), aKeyword);
    return theIt!=std::end(theKnown);
  }

  CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
    // Define a factory of Statement objects
    std::map<Keywords, std::function<Statement* ()>> theStmtFactory{
      {Keywords::create_kw,   [&]() { return new CreateDBStatement();}},            
      {Keywords::drop_kw,     [&]() { return new DropDBStatement();}},
      {Keywords::show_kw,     [&]() { return new ShowDBStatement();}},
      {Keywords::use_kw,      [&]() { return new UseStatement();}},
      {Keywords::dump_kw,     [&]() { return new DumpStatement(); }}
    };
    if (theStmtFactory.count(theKeyword)) {
      theStatement = theStmtFactory[theKeyword]();
    }
    if (theStatement) {
      if (theStatement->parse(aTokenizer)) {
        return this;
      }
    }
    aTokenizer.restart();
    delete theStatement;
    theStatement = nullptr;
    return nullptr;
  }

  StatusResult DBProcessor::run(Statement *aStmt) {

    // ----------------------------------------------------------------
    switch (aStmt->getType()) {
      case Keywords::create_kw:     return createDatabase(aStmt);
      case Keywords::drop_kw:       return dropDatabase(aStmt);
      case Keywords::use_kw:        return useDatabase(aStmt);
      case Keywords::show_kw:       return showDatabases();
      case Keywords::dump_kw:       return dumpDatabase(aStmt);
      default: break;
    }
    return StatusResult{ Errors::notImplemented };
  }

  // USE: retrieve a statement based on given text input...
  Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) {
    
    aResult = StatusResult{noError};
    return theStatement;
  }
  
  //-------------------------------------------------------

  Database* DBProcessor::getDatabaseInUse() {
    return activeDB;
  }

  void DBProcessor::releaseDatabase(){
    if (activeDB){
      delete activeDB;
      activeDB = nullptr;
    }
  }

  bool DBProcessor::dbExists(const std::string &aDBName) {
    std::string thePath = Config::getDBPath(aDBName);
    std::ifstream theStream(thePath);
    return !theStream ? false : true;
  }

  Database* DBProcessor::loadDatabase(const std::string aName){
    Database *theDB = nullptr;
    if (dbExists(aName))
      theDB = new Database(aName, OpenDB{});
    return theDB;
  }  

  StatusResult DBProcessor::createDatabase(Statement *aStmt) {
    DBStatement *theStmt = dynamic_cast<DBStatement*>(aStmt);
    std::string aName = theStmt->getDBName();
    releaseDatabase();
    activeDB = loadDatabase(aName);
    if (!activeDB){
      output << std::setprecision(3) << std::fixed;
      activeDB = new Database(aName, CreateDB{});
      output << "Query OK, 1 row affected (" << Config::getTimer().elapsed() << " secs)" << std::endl;
      return StatusResult{Errors::noError};
    }
    else{
      std::cout << "Database already exists!" << std::endl;
      return StatusResult{Errors::databaseCreationError};
    }
  }
  
  StatusResult DBProcessor::showDatabases() const {
    //make a view, load with DB names from storage folder...
    DBView theView(output);
    theView.showDBs();
    return StatusResult{Errors::noError};
  }

  // USE: call this to perform the dropping of a database (remove the file)...
  StatusResult DBProcessor::dropDatabase(Statement *aStmt) {
    DBStatement *theStmt = dynamic_cast<DBStatement*>(aStmt);
    std::string aName = theStmt->getDBName();
    output << std::setprecision(3) << std::fixed;
    std::string thePath = Config::getDBPath(aName);
    if (dbExists(aName)) {
      if (activeDB)
        if (activeDB->getName() == aName)
          releaseDatabase();
      std::remove(thePath.c_str());
      output << "Query OK, 0 rows affected ("<< Config::getTimer().elapsed() << " secs) " << std::endl;
      return StatusResult{Errors::noError};
    }
    else {
      std::cout << "Database does not exist!" << std::endl;
      return StatusResult{Errors::unknownDatabase};
    }
    return StatusResult{Errors::noError};
  }

  // USE: DB dump all storage blocks
  StatusResult DBProcessor::dumpDatabase(Statement *aStmt)  {
    DBStatement *theStmt = dynamic_cast<DBStatement*>(aStmt);
    std::string aName = theStmt->getDBName();
    releaseDatabase();
    activeDB = loadDatabase(aName);
    if (activeDB){
      DBView theView(output);
      theView.dumpDB(aName);
    }
    return StatusResult{ECE141::noError};
  }

  
  // USE: call DB object to be loaded into memory...
  StatusResult DBProcessor::useDatabase(Statement *aStmt) { 
    DBStatement *theStmt = dynamic_cast<DBStatement*>(aStmt);
    std::string aName = theStmt->getDBName();
    if (!dbExists(aName)) {
      std::cout << "Database does not exist!" << std::endl;
      return StatusResult{Errors::unknownDatabase};
    }
    output << std::setprecision(3) << std::fixed;
    releaseDatabase();
    activeDB = loadDatabase(aName);
    output << "Database changed" << std::endl;
    return StatusResult{ECE141::noError};
  }

}
