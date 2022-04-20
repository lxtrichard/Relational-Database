#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include "keywords.hpp"
#include <iostream>
#include "Statement.hpp"
#include "Attribute.hpp"
#include <vector>
#include "SQLProcessor.hpp"

namespace ECE141 {
  
  class Tokenizer;
  
  class SQLStatement: public Statement {
  public:
    SQLStatement(SQLProcessor &aSQLProcessor, Keywords aStmtType);
    SQLStatement(const Statement &aCopy);
    
    virtual               ~SQLStatement();
    std::string           getTableName() {return thetableName;}
    
    // StatusResult  parse(Tokenizer &aTokenizer);    
  protected:
    std::string thetableName;
    SQLProcessor *theSQLProcessor;
  };

  class CreateStatement : public SQLStatement {
  public:
      CreateStatement(SQLProcessor& aSQLProcessor);
      virtual                 ~CreateStatement();

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  parseAttributes(Tokenizer& aTokenizer);
      StatusResult  parseAttribute(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  parseOptions(Tokenizer& aTokenizer, Attribute& anAttribute);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);

      StatusResult  run(std::ostream& aStream) const;
  protected:
      std::vector<Attribute> attributes;
  };
  
  class ShowStatement : public SQLStatement {
  public:
      ShowStatement(SQLProcessor& aSQLProcessor);
      ~ShowStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);

      StatusResult  run(std::ostream& aStream) const;
  protected:
      std::vector<Attribute> attributes;
  };

  class DescribeStatement : public SQLStatement {
  public:
      DescribeStatement(SQLProcessor& aSQLProcessor);
      ~DescribeStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);

      StatusResult  run(std::ostream& aStream) const;
  protected:
      std::vector<Attribute> attributes;
  };

  class DropStatement : public SQLStatement {
  public:
      DropStatement(SQLProcessor& aSQLProcessor);
      ~DropStatement(){};

      StatusResult  parse(Tokenizer& aTokenizer);
      StatusResult  getVarSize(Tokenizer& aTokenizer, Attribute& anAttribute);

      StatusResult  run(std::ostream& aStream) const;
  protected:
      std::vector<Attribute> attributes;
  };
}

#endif /* SQLStatement_hpp */