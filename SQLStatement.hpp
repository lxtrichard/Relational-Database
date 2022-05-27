#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include <iostream>
#include <vector>
#include <memory>
#include "keywords.hpp"
#include "Statement.hpp"
#include "Attribute.hpp"
#include "SQLProcessor.hpp"
#include "DBQuery.hpp"
#include "Database.hpp"
#include "Joins.hpp"

namespace ECE141 {
  
  class Tokenizer;
  
  class SQLStatement: public Statement {
  public:
    SQLStatement(Database* aDB, Keywords aStmtType);
    
    ~SQLStatement(){};
    std::string             getTableName() {return thetableName;}
    std::vector<Attribute>  getAttributes() {return attributes;}
    virtual StatusResult    run(std::ostream &aStream) const;

  protected:
    Database*   theDB;
    std::string thetableName;
    std::vector<Attribute> attributes;
  };

  class CreateStatement : public SQLStatement {
  public:
      CreateStatement(Database* aDB) : SQLStatement(aDB, Keywords::create_kw) {};
      ~CreateStatement(){};
      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  parseAttributes(Tokenizer& aTokenizer);
      StatusResult  parseAttribute(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  parseOptions(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  run(std::ostream &aStream);
  };
  
  class ShowStatement : public SQLStatement {
  public:
      ShowStatement(Database* aDB) : SQLStatement(aDB, Keywords::show_kw) {};
      ~ShowStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  run(std::ostream &aStream);

  };

  class DescribeStatement : public SQLStatement {
  public:
      DescribeStatement(Database* aDB) : SQLStatement(aDB, Keywords::describe_kw) {};
      ~DescribeStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  run(std::ostream &aStream);

  };

  class DropStatement : public SQLStatement {
  public:
      DropStatement(Database* aDB) : SQLStatement(aDB, Keywords::drop_kw) {};
      ~DropStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  run(std::ostream &aStream);

  };

  class InsertStatement : public Statement {
  public:
      InsertStatement(Database* aDB) : theDB(aDB), 
            Statement(Keywords::insert_kw), thetableName("") {};
      ~InsertStatement() {};

      StatusResult parse(Tokenizer& aTokenizer);

      std::string                             getTableName() {return thetableName;}
      std::vector<std::string>&               getAttributes() { return theAttributeNames; }
      std::vector<std::vector<std::string>>&  getValues() { return values; }

      StatusResult  run(std::ostream& aStream);

  protected:
      Database*                     theDB;
      std::string                   thetableName;
      std::vector<std::string>      theAttributeNames;
      std::vector<std::vector<std::string>>   values;
  };

  class SelectStatement : public Statement {
  public:
      SelectStatement(Database* aDB);

      ~SelectStatement() {};

      std::shared_ptr<DBQuery>& getQuery() { return theQuery; }
      JoinList& getJoins() { return joins; }

      StatusResult parse(Tokenizer& aTokenizer);
      StatusResult parseClause(Keywords aKeyword, Tokenizer& aTokenizer);
      virtual StatusResult run(std::ostream& aStream);

  protected:
      StatusResult parseSelect(Tokenizer& aTokenizer);
      StatusResult parseEntity(Tokenizer& aTokenizer);
      StatusResult parseWhere(Tokenizer& aTokenizer);
      StatusResult parseOrderBy(Tokenizer& aTokenizer);
      StatusResult parseJoin(Tokenizer& aTokenizer);
      StatusResult parseLimit(Tokenizer& aTokenizer);

      Database     *theDB;
      std::shared_ptr<DBQuery> theQuery;
      JoinList joins;
  };

  class UpdateStatement : public SelectStatement{
  public:
      UpdateStatement(Database* aDB) : SelectStatement(aDB){ stmtType = Keywords::update_kw; };
      ~UpdateStatement() {};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  parseSet(Tokenizer& aTokenizer);
      KeyValues&    getSet() { return theSet; }
      StatusResult  run(std::ostream& aStream);
  protected:
      KeyValues theSet;
  };

  class DeleteStatement : public SelectStatement{
  public:
      DeleteStatement(Database* aDB) : SelectStatement(aDB){ stmtType = Keywords::delete_kw; };
      ~DeleteStatement() {};

      StatusResult  parse(Tokenizer& aTokenizer);

      StatusResult  run(std::ostream& aStream);
  };
}

#endif /* SQLStatement_hpp */