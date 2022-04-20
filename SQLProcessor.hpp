//
//  SQLProcessor.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CmdProcessor.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {

  class Statement;
  class DBProcessor; //define this later...
  class Entity;
  class Database;

  //Should this processor have it's own Statement types?

  class SQLProcessor : public CmdProcessor {
  public:
    
    SQLProcessor(std::ostream &anOutput);
    virtual ~SQLProcessor();
    
    CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
    Statement*    makeStatement(Tokenizer &aTokenizer,
                                StatusResult &aResult) override;
    
    StatusResult  run(Statement *aStmt) override;
    
    //We don't DO the work here, but we can help route cmds...
    StatusResult  createTable(Entity *anEntity);
    StatusResult  describeTable(const std::string &aName);
    StatusResult  dropTable(const std::string &aName);
    StatusResult  showTables();
    Database*     getActiveDB() {return activeDB;}
    SQLProcessor* setActiveDB(Database* aDB);
    SQLProcessor* setEntity(Entity* aEntity);
    Entity*       getEntity(){return theEntity;};

  protected:
    //do you need other data members?
    Database      *activeDB;
    Statement     *theStatement;
    Entity        *theEntity;
  };

}
#endif /* SQLProcessor_hpp */
