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
      {Keywords::create_kw,Keywords::drop_kw, Keywords::show_kw, Keywords::use_kw, Keywords::dump_kw};
    auto theIt = std::find(std::begin(theKnown),
                           std::end(theKnown), aKeyword);
    return theIt!=std::end(theKnown);
  }

  CmdProcessor* DBProcessor::recognizes(Tokenizer &aTokenizer) {
    theTokens.clear();
    if (isKnown(aTokenizer.current().keyword)) {
      theTokens.push_back(aTokenizer.current());
      // If show database is called
      if (aTokenizer.current().keyword==Keywords::show_kw) {
        if (aTokenizer.peek(1).keyword==Keywords::databases_kw){
          theTokens.push_back(aTokenizer.current()); 
          if (aTokenizer.peek(2).data[0]==';' || aTokenizer.peek(2).data[0]=='\n'){
            aTokenizer.next(2);
            return this;
          }
        }
      }
      else if (aTokenizer.current().keyword==Keywords::use_kw){
        if (aTokenizer.peek(1).type == TokenType::identifier) {
          theTokens.push_back(aTokenizer.peek(1));
          if (aTokenizer.peek(2).data[0]==';' || aTokenizer.peek(2).data[0]=='\n') {
              aTokenizer.next(2);
              return this;
          }
        }
      }
      else if (aTokenizer.peek(1).keyword==Keywords::database_kw){
        theTokens.push_back(aTokenizer.peek(1));
        if (aTokenizer.peek(2).type==TokenType::identifier){
          theTokens.push_back(aTokenizer.peek(2));
          if (aTokenizer.peek(3).data[0]==';' || aTokenizer.peek(3).data[0]=='\n'){
            aTokenizer.next(3);
            return this;
          }
        }
      }
    }
    return nullptr;
  }

  StatusResult DBProcessor::run(Statement *aStmt) {
    switch (aStmt->getType()) {
    case Keywords::create_kw:
        anActiveDBName = theTokens[2].data;
        createDatabase(anActiveDBName);
        break;
    case Keywords::drop_kw:
        anActiveDBName = theTokens[2].data;
        dropDatabase(anActiveDBName);
        break;
    case Keywords::use_kw:
        anActiveDBName = theTokens[1].data;
        useDatabase(anActiveDBName);
        break;
    case Keywords::show_kw:
        showDatabases();
        break;
    case Keywords::dump_kw:
        anActiveDBName = theTokens[2].data;
        dumpDatabase(anActiveDBName);
        break;
    default: break;
    }
    return StatusResult{ Errors::noError };
  }

  // USE: retrieve a statement based on given text input...
  Statement* DBProcessor::makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) {
    Token theToken = theTokens[0];
    if (isKnown(theToken.keyword)) {
        return new Statement(theToken.keyword);
    }
    return nullptr;
  }
  
  //-------------------------------------------------------

  Database* DBProcessor::getDatabaseInUse() {
    if (activeDB)
      return activeDB;
    return nullptr;
  }

  void DBProcessor::releaseDatabase(){
    if (activeDB){
      delete activeDB;
      activeDB = nullptr;
    }
  }

  Database* DBProcessor::loadDatabase(const std::string aName){
    Database *theDB = nullptr;
    if (dbExists(aName))
      theDB = new Database(aName, OpenDB{});
    return theDB;
  }

  bool DBProcessor::dbExists(const std::string &aDBName) {
    std::string thePath = Config::getDBPath(aDBName);
    std::ifstream theStream(thePath);
    return !theStream ? false : true;
  }

  StatusResult DBProcessor::createDatabase(const std::string &aName) {
    releaseDatabase();
    activeDB = loadDatabase(aName);
    if (!activeDB){
      Timer theTimer;
      activeDB = new Database(aName, CreateDB{});
      output << "Query OK, 1 row affected (" << theTimer.elapsed() << " secs)" << std::endl;
      return StatusResult{Errors::noError};
    }
    else{
      std::cout << "Database already exists!" << std::endl;
      return StatusResult{Errors::databaseCreationError};
    }
  }
  
  StatusResult DBProcessor::showDatabases() const {
    //make a view, load with DB names from storage folder...
    std::string thePath = Config::getStoragePath();
    FolderReader theReader(thePath.c_str());
    Timer theTimer;
    output << "+--------------------+" << std::endl;
    output << "| Database           |" << std::endl;
    output << "+--------------------+" << std::endl;
    int len_str = strlen("+--------------------+");
    int theCount = 0;
    theReader.each(".db",[&](const std::string &aName) {
      output << "| " << aName << std::string(len_str-strlen(aName.c_str())-3, ' ') << "|" << std::endl;
      theCount++;
      return true;
    });
    output << "+--------------------+" << std::endl;
    output << theCount << " rows in set (" << theTimer.elapsed() << " secs)" << std::endl;
    return StatusResult{Errors::noError};
  }

  // USE: call this to perform the dropping of a database (remove the file)...
  StatusResult DBProcessor::dropDatabase(const std::string &aName) {
    Timer theTimer;
    std::string thePath = Config::getDBPath(aName);
    if (dbExists(aName)) {
      std::remove(thePath.c_str());
      output << "Query OK, 0 rows affected (" << theTimer.elapsed() << ") " << std::endl;
      return StatusResult{Errors::noError};
    }
    else {
      std::cout << "Database does not exist!" << std::endl;
      return StatusResult{Errors::unknownDatabase};
    }
    return StatusResult{Errors::noError};
  }

  // USE: DB dump all storage blocks
  StatusResult DBProcessor::dumpDatabase(const std::string &aName)  {
    Timer theTimer;
    output << "+----------------+--------+" << std::endl;
    output << "| Type           | Id     |" << std::endl;
    std::string thePath = Config::getDBPath(aName);
    std::fstream stream;
    stream.open(thePath, std::fstream::in | std::fstream::out);
    stream.seekg(0, stream.end);
    size_t DBSize = stream.tellg();
    stream.seekg(0, stream.beg);
    size_t theBlockNum = DBSize / ECE141::kBlockSize;
    BlockHeader theHeader;
    for (size_t i = 0; i < theBlockNum; i++) {
      output << "+----------------+--------+" << std::endl;
      size_t thePos = i * ECE141::kBlockSize;
      stream.seekp(thePos,stream.beg);
      stream.read(reinterpret_cast<char*>(&theHeader), sizeof(ECE141::BlockHeader));
      // convert blocktype into string
      BlockType theType = static_cast<BlockType>(theHeader.type);
      std::string theTypeName;
      switch(theType) {
        case BlockType::data_block:     theTypeName = "Data"; break;
        case BlockType::free_block:     theTypeName = "Free"; break;
        case BlockType::unknown_block:  theTypeName = "Unknown"; break;
        case BlockType::meta_block:     theTypeName = "Meta"; break;
      }
      output << "| " << std::setw(15) << std::left << theTypeName << "| " << std::setw(7) << std::left << theHeader.id << "|" << std::endl;
    }
    output << "+----------------+--------+" << std::endl;
    output << "0 rows in set (" << theTimer.elapsed() << ") " << std::endl;
    return StatusResult{ECE141::noError};
  }

  
  // USE: call DB object to be loaded into memory...
  StatusResult DBProcessor::useDatabase(const std::string &aName) {   
    if (!dbExists(aName)) {
      std::cout << "Database does not exist!" << std::endl;
      return StatusResult{Errors::unknownDatabase};
    }
    releaseDatabase();
    activeDB = loadDatabase(aName);
    output << "Database changed" << std::endl;
    return StatusResult{ECE141::noError};
  }

}
