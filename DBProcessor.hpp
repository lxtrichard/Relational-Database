//
//  DBProcessor.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"


namespace ECE141 {
  
  class DBStatement;

  class DBProcessor : public CmdProcessor {
  public:
    
    DBProcessor(std::ostream &anOutput);
    ~DBProcessor();
    
      //cmd processor interface...
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;
    StatusResult  run(Statement *aStmt) override;
               
      //dbProcessor interface...
    StatusResult  createDatabase(const std::string &aName);
    StatusResult  dumpDatabase(const std::string &aName);
    StatusResult  dropDatabase(const std::string &aName);
    StatusResult  showDatabases() const;
    StatusResult  useDatabase(const std::string &aName);

    bool          dbExists(const std::string &aDBName);
    bool          isKnown(Keywords aKeyword);
    Database*     getDatabaseInUse();

  protected:

    //stuff?
    Database      *activeDB;
    std::vector<Token> theTokens;
    std::string   anActiveDBName;
    Database*     loadDatabase(const std::string aName);
    void          releaseDatabase();
  };

}
#endif /* DBProcessor_hpp */
