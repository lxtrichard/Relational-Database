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

namespace ECE141 {
  
  class Tokenizer;
  
  class SQLStatement: public Statement {
  public:
    SQLStatement(Keywords aStmtType);
    
    ~SQLStatement(){};
    std::string           getTableName() {return thetableName;}
    std::vector<Attribute> getAttributes() {return attributes;}

    // StatusResult  parse(Tokenizer &aTokenizer);    
  protected:
    std::string thetableName;
    std::vector<Attribute> attributes;
  };

  class CreateStatement : public SQLStatement {
  public:
      CreateStatement() : SQLStatement(Keywords::create_kw) {};
      ~CreateStatement(){};
      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  parseAttributes(Tokenizer& aTokenizer);
      StatusResult  parseAttribute(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  parseOptions(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);
  };
  
  class ShowStatement : public SQLStatement {
  public:
      ShowStatement() : SQLStatement(Keywords::show_kw) {};
      ~ShowStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

  class DescribeStatement : public SQLStatement {
  public:
      DescribeStatement() : SQLStatement(Keywords::describe_kw) {};
      ~DescribeStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

  class DropStatement : public SQLStatement {
  public:
      DropStatement() : SQLStatement(Keywords::drop_kw) {};
      ~DropStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
  };

  class InsertStatement : public Statement {
  public:
      InsertStatement() : Statement(Keywords::insert_kw), thetableName("") {};
      ~InsertStatement() {};

      StatusResult parse(Tokenizer& aTokenizer);

      std::string                             getTableName() {return thetableName;}
      std::vector<std::string>&               getAttributes() { return theAttributeNames; }
      std::vector<std::vector<std::string>>&  getValues() { return values; }

  protected:
      std::string                             thetableName;
      std::vector<std::string>                theAttributeNames;
      std::vector<std::vector<std::string>>   values;
  };

  class SelectStatement : public Statement {
  public:
      SelectStatement(Database* aDB);

      ~SelectStatement() {};

      StatusResult parse(Tokenizer& aTokenizer);
      StatusResult parseClause(Keywords aKeyword, Tokenizer& aTokenizer);

      std::shared_ptr<DBQuery>& getQuery() { return theQuery; }

  protected:
      StatusResult parseSelect(Tokenizer& aTokenizer);
      StatusResult parseEntity(Tokenizer& aTokenizer);
      StatusResult parseWhere(Tokenizer& aTokenizer);
      StatusResult parseOrderBy(Tokenizer& aTokenizer);
      StatusResult parseGroupBy(Tokenizer& aTokenizer);
      StatusResult parseLimit(Tokenizer& aTokenizer);
      Database     *theDB;
      std::shared_ptr<DBQuery> theQuery;
  };
}

#endif /* SQLStatement_hpp */