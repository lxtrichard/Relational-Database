#ifndef DBStatement_hpp
#define DBStatement_hpp

#include <iostream>
#include <vector>
#include <memory>
#include "keywords.hpp"
#include "Statement.hpp"
#include "Attribute.hpp"
#include "SQLProcessor.hpp"
#include "DBQuery.hpp"

namespace ECE141 {
  
  class Tokenizer;
  
  class DBStatement: public Statement {
  public:
    DBStatement(Keywords aStmtType);
    
    ~DBStatement(){};

    std::string             getDBName() {return theDBName;}
    void                    setDBName(std::string dbName) {theDBName = dbName;}
    StatusResult            run(std::ostream &aStream);

    // StatusResult  parse(Tokenizer &aTokenizer);    
  protected:
    std::string theDBName;
  };

  class CreateDBStatement : public DBStatement {
  public:
      CreateDBStatement() : DBStatement(Keywords::create_kw) {};
      ~CreateDBStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

  class DropDBStatement : public DBStatement {
  public:
      DropDBStatement() : DBStatement(Keywords::drop_kw) {};
      ~DropDBStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };
  
  class ShowDBStatement : public DBStatement {
  public:
      ShowDBStatement() : DBStatement(Keywords::show_kw) {};
      ~ShowDBStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  
  };

  class UseStatement : public DBStatement {
  public:
      UseStatement() : DBStatement(Keywords::use_kw) {};
      ~UseStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

  class DumpStatement : public DBStatement {
  public:
      DumpStatement() : DBStatement(Keywords::dump_kw) {};
      ~DumpStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

}

#endif /* DBStatement_hpp */