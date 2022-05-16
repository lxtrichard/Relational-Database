#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Database.hpp"

namespace ECE141
{
  SQLStatement::SQLStatement(Keywords aStmtType) : Statement(aStmtType){}

  static bool stob(std::string aStr) {
    if (stoi(aStr))
      return true;
    else
      return false;
  }

  // ---------------  CreateStatement  -------------------- //

  StatusResult CreateStatement::parse(Tokenizer &aTokenizer){
    Token &theToken = aTokenizer.peek(1);
    if (theToken.keyword == Keywords::table_kw){
      theToken = aTokenizer.peek(2);
      if (theToken.type == TokenType::identifier){
        aTokenizer.next(3);
        thetableName = theToken.data;
        if (aTokenizer.skipIf('(')){
          StatusResult theResult = parseAttributes(aTokenizer);
          return theResult;
        }
      }
    }
    return StatusResult{Errors::unknownCommand};
  }

  StatusResult  CreateStatement::parseAttributes(Tokenizer &aTokenizer){
    StatusResult theResult{Errors::noError};
    while (theResult && aTokenizer.more()){
      Token &theToken = aTokenizer.current();
      if (theToken.type == TokenType::identifier){
        Attribute theAttribute(theToken.data, DataTypes::no_type);
        aTokenizer.next();
        theResult = parseAttribute(aTokenizer, theAttribute);
      }
      else if (aTokenizer.skipIf(')')){
        aTokenizer.next();
        return StatusResult{Errors::noError};
      }
      else {
        theResult = StatusResult{Errors::syntaxError};
      }
    }
    return StatusResult{Errors::unknownCommand};
  };

  StatusResult  CreateStatement::parseAttribute(Tokenizer &aTokenizer, Attribute &anAttribute){
    StatusResult theResult{Errors::noError};
    if (aTokenizer.more()){
      Token &theToken = aTokenizer.current();
      if (Helpers::isDatatype(theToken.keyword)){
        anAttribute.setDataType(Helpers::KeywordToDatatypes(theToken.keyword));
        aTokenizer.next();
        if (anAttribute.getType() == DataTypes::varchar_type){
          theResult = getVarSize(aTokenizer, anAttribute);
        }
        if (theResult){
          theResult = parseOptions(aTokenizer, anAttribute);
          if (theResult){
            attributes.push_back(anAttribute);
            return theResult;
          }
          else{
            return StatusResult{Errors::syntaxError};
          }
        }
      }
    }
    return StatusResult{Errors::unknownType};
  }

  StatusResult  CreateStatement::parseOptions(Tokenizer &aTokenizer, Attribute &anAttribute){
    StatusResult theResult{Errors::noError};
    while (theResult && aTokenizer.more()){
      Token &theToken = aTokenizer.current();
      if (theToken.type==TokenType::keyword){
        switch (theToken.keyword)
        {
        case Keywords::primary_kw:
          aTokenizer.next();
          theToken = aTokenizer.current();
          if (theToken.keyword == Keywords::key_kw) {
            anAttribute.setPrimaryKey(true);
          }
          else {
            theResult = StatusResult{ Errors::keywordExpected };
            return theResult;
          }
          break;
        case Keywords::auto_increment_kw:
          anAttribute.setAutoIncrement(true);
          break;
        case Keywords::not_kw:
          {
            aTokenizer.next();
            theToken = aTokenizer.current();
            if (theToken.keyword == Keywords::null_kw){
              anAttribute.setNullable(false);
            }
            else {
              theResult = StatusResult{Errors::syntaxError};
              return theResult;
            }
          }
          break;
        case Keywords::default_kw:
          {
            anAttribute.setDefault(true);
            if (aTokenizer.more()){
              aTokenizer.next();
              theToken = aTokenizer.current();
              // anAttribute.setDefaultValue(theToken.data);
              switch (anAttribute.getType())
              {
              case DataTypes::bool_type:
                anAttribute.setDefaultValue(aTokenizer.current().keyword == Keywords::true_kw);
                break;
              case DataTypes::datetime_type:
                anAttribute.setDefaultValue(Helpers::getCurrentTime());
                break;
              case DataTypes::float_type:
                anAttribute.setDefaultValue(std::stod(aTokenizer.current().data));
                break;
              case DataTypes::int_type:
                anAttribute.setDefaultValue(std::stoi(aTokenizer.current().data));
                break;
              case DataTypes::varchar_type:
                anAttribute.setDefaultValue(aTokenizer.current().data.substr(0, anAttribute.getSize()));
                break;
              default:
                break;
              }

            }
            else {
              theResult = StatusResult{Errors::syntaxError};
              return theResult;
            }
          }
          break;
        
        default:
          break;
        }
        aTokenizer.next();
      }
      else if (theToken.data[0]==','){
        aTokenizer.next();
        return theResult;
      }
      else if (theToken.data[0]==')'){
        return theResult;
      }
      else{
        theResult = StatusResult{Errors::syntaxError};
        return theResult;
      }
    }
    return theResult;
  }

  StatusResult CreateStatement::getVarSize(Tokenizer &aTokenizer, Attribute &anAttribute){
    Token &theToken = aTokenizer.current();
    if (theToken.data[0] == '('){
      aTokenizer.next();
      theToken = aTokenizer.current();
      if (theToken.type == TokenType::number){
        anAttribute.setSize(atoi(theToken.data.c_str()));
        aTokenizer.next();
        if (aTokenizer.skipIf(')')){
          return StatusResult{Errors::noError};
        }
      }
    }
    return StatusResult{Errors::syntaxError};
  };

  // ---------------  ShowStatement  -------------------- //
  StatusResult ShowStatement::parse(Tokenizer &aTokenizer){
    Token &theToken = aTokenizer.peek(1);
    if (theToken.keyword == Keywords::tables_kw){
      aTokenizer.next(2);
      return StatusResult{Errors::noError};
    }
    return StatusResult{Errors::unknownCommand};
  }

  // ---------------  DescribeStatement  ------------------ //
  StatusResult DescribeStatement::parse(Tokenizer &aTokenizer){
    Token &theToken = aTokenizer.peek(1);
    if (theToken.type == TokenType::identifier){
      thetableName = theToken.data;
      aTokenizer.next(2);
      return StatusResult{Errors::noError};
    }
    return StatusResult{Errors::unknownCommand};
  }

  // ---------------  DropStatement  ------------------ //
  StatusResult DropStatement::parse(Tokenizer &aTokenizer){
    Token &theToken = aTokenizer.peek(1);
    if (theToken.keyword == Keywords::table_kw){
      theToken = aTokenizer.peek(2);
      if (theToken.type == TokenType::identifier){
        thetableName = theToken.data;
        aTokenizer.next(3);
        return StatusResult{Errors::noError};
      }
    }
    return StatusResult{Errors::unknownCommand};
  }

  // ---------------  InsertStatement  ------------------ //
  StatusResult InsertStatement::parse(Tokenizer &aTokenizer){
    int anIndex = 3;
    Token &theToken = aTokenizer.peek(1);

    if (theToken.keyword == Keywords::into_kw){
      theToken = aTokenizer.peek(2);
      if (theToken.type == TokenType::identifier){

        // TODO: Need to put a check if table is present in the DB
        thetableName = theToken.data;
        theToken = aTokenizer.peek(3);

        if (theToken.data[0]=='(') {
          while(aTokenizer.peek(anIndex).data[0]!=')') {
            ++anIndex;

            if(aTokenizer.peek(anIndex).type != TokenType::identifier) {
              return StatusResult{Errors::invalidArguments};
            }

            theToken = aTokenizer.peek(anIndex);
            theAttributeNames.push_back(theToken.data);
            anIndex++;

            if(aTokenizer.peek(anIndex).data[0]==')') {
              break;
            }
            if(aTokenizer.peek(anIndex).data[0]!=',') {
              return StatusResult{Errors::invalidArguments};
            }
          }

        }

        anIndex++;

        theToken = aTokenizer.peek(anIndex);

        if(theToken.keyword == Keywords::values_kw) {
          anIndex++;
          while(aTokenizer.peek(anIndex).data[0]!=';') {
            if(aTokenizer.peek(anIndex).data[0]!='(') {
              return StatusResult{Errors::invalidArguments};
            }

            anIndex++;
            std::vector<std::string> theValueNames;
            while(aTokenizer.peek(anIndex).data[0]!=')') {
              
              if(aTokenizer.peek(anIndex).type != TokenType::identifier && aTokenizer.peek(anIndex).type != TokenType::number) {
                return StatusResult{Errors::invalidArguments};
              }
              theToken = aTokenizer.peek(anIndex);
              theValueNames.push_back(theToken.data);

              anIndex++;
              
              if(aTokenizer.peek(anIndex).data[0]==')') {
                break;
              }
              if(aTokenizer.peek(anIndex).data[0]!=',') {
                return StatusResult{Errors::invalidArguments};
              }
              anIndex++;
            }

            if (theAttributeNames.size()!=theValueNames.size()) {
              return StatusResult{Errors::invalidArguments};
            }

            // make a Row object
            // vector of struct info: a pair of key value
            // make a vector of rows.
            anIndex++;
            values.push_back(theValueNames);
            if(aTokenizer.peek(anIndex).data[0]==';') {
              break;
            }
            if (aTokenizer.peek(anIndex).data[0]==',') {
              anIndex++;
            }
          } 
        }
      }
    }
    aTokenizer.next(aTokenizer.size()-1);
    return StatusResult{Errors::noError};
  }

  // ---------------  SelectStatement  ------------------ //
  SelectStatement::SelectStatement(Database* aDB) : theDB(aDB), 
        Statement(Keywords::select_kw), theQuery(new DBQuery()) {};

  StatusResult SelectStatement::parse(Tokenizer &aTokenizer){
    StatusResult theResult = StatusResult{Errors::noError};
    // Parse Select
    theResult = parseSelect(aTokenizer);
    if (!theResult)
      return theResult;
    // Parse Entity Name
    theResult = parseEntity(aTokenizer);
    while (aTokenizer.more() && aTokenizer.current().data[0]!=';'){
      Keywords theKeyword = aTokenizer.current().keyword;
      theResult = parseClause(theKeyword, aTokenizer);
      if (!theResult)
        return theResult;
    }
    return StatusResult{Errors::noError};
  }

  StatusResult SelectStatement::parseSelect(Tokenizer& aTokenizer){
    if (!aTokenizer.skipIf(Keywords::select_kw))
      return StatusResult{Errors::keyExpected};

    if (!aTokenizer.skipIf('*')){
      while (aTokenizer.current().keyword != Keywords::from_kw){
        if (aTokenizer.skipIf(','))
          continue;
        if (aTokenizer.current().type != TokenType::identifier)
          return StatusResult{Errors::identifierExpected};

        theQuery->setSelects(aTokenizer.current().data);
        aTokenizer.next();
      }
    }
    else{
      theQuery->setSelectAll(true);
      aTokenizer.skipTo(Keywords::from_kw);
    }

    return StatusResult{Errors::noError};
  };

  StatusResult SelectStatement::parseEntity(Tokenizer &aTokenizer){
    aTokenizer.skipIf(Keywords::from_kw);
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    theQuery->setEntityName(aTokenizer.current().data);
    Entity *anEntity = theDB->getEntity(aTokenizer.current().data);
    theQuery->setEntity(anEntity);
    aTokenizer.next();
    return StatusResult{Errors::noError};
  }
  
  StatusResult SelectStatement::parseClause(Keywords aKeyword, Tokenizer& aTokenizer){
    using Clauseparser = StatusResult (SelectStatement::*)(Tokenizer&);
    static std::map<Keywords, Clauseparser> theClauseMap = {
      {Keywords::where_kw, &SelectStatement::parseWhere},
      {Keywords::group_kw, &SelectStatement::parseGroupBy},
      {Keywords::order_kw, &SelectStatement::parseOrderBy},
      {Keywords::limit_kw, &SelectStatement::parseLimit}
    };
    if (theClauseMap.count(aKeyword)) {
      return (this->*theClauseMap[aKeyword])(aTokenizer);
    }
    return StatusResult{Errors::unknownCommand};
  }

  StatusResult SelectStatement::parseWhere(Tokenizer &aTokenizer){
    aTokenizer.skipIf(Keywords::where_kw);
    StatusResult theResult = theQuery->parseFilters(aTokenizer);
    return theResult;
  }

  StatusResult SelectStatement::parseGroupBy(Tokenizer &aTokenizer){
    return StatusResult{Errors::unknownCommand};
  }

  StatusResult SelectStatement::parseOrderBy(Tokenizer &aTokenizer){
    if (aTokenizer.skipIf(Keywords::order_kw))
      if (aTokenizer.skipIf(Keywords::by_kw))
      {
        if (aTokenizer.current().type == TokenType::identifier) {
          theQuery->setOrderBy(aTokenizer.current().data);
          aTokenizer.next();
          return StatusResult{Errors::noError};
        }
      }
    return StatusResult{Errors::unknownCommand};
  }

  StatusResult SelectStatement::parseLimit(Tokenizer &aTokenizer){
    aTokenizer.skipIf(Keywords::limit_kw);
    if (aTokenizer.current().type == TokenType::number) {
      theQuery->setLimit(std::stoi(aTokenizer.current().data));
      aTokenizer.next();
      return StatusResult{Errors::noError};
    }
    return StatusResult{Errors::unknownCommand};
  }

  // ---------------  UpdateStatement  ------------------ //
  StatusResult UpdateStatement::parse(Tokenizer& aTokenizer){
    StatusResult theResult = StatusResult{Errors::noError};
    // Parse Update
    if (!aTokenizer.skipIf(Keywords::update_kw))
      return StatusResult{Errors::keyExpected};
    // Parse Entity Name
    theQuery->setEntityName(aTokenizer.current().data);
    Entity *anEntity = theDB->getEntity(aTokenizer.current().data);
    theQuery->setEntity(anEntity);
    aTokenizer.next();
    // Parse Set
    if (!(theResult = parseSet(aTokenizer)))
      return theResult;
    // Parse Where
    if (aTokenizer.current().keyword == Keywords::where_kw) {
      if (!(theResult = parseWhere(aTokenizer)))
        return theResult;
    }
    return StatusResult{Errors::noError};
  }

  StatusResult UpdateStatement::parseSet(Tokenizer& aTokenizer){
    if (!aTokenizer.skipIf(Keywords::set_kw))
      return StatusResult{Errors::keyExpected};
    Entity *anEntity = theQuery->getEntity();

    std::string theFieldName;
    std::string theValue;
    std::map<DataTypes, std::function<void()>> theMap = {
      {DataTypes::bool_type,      [&]() { theSet.insert({theFieldName, stob(theValue)}); } },
      {DataTypes::int_type,       [&]() { theSet.insert({theFieldName, std::stoi(theValue)}); } },
      {DataTypes::float_type,     [&]() { theSet.insert({theFieldName, std::stod(theValue)}); } },
      {DataTypes::varchar_type,   [&]() { theSet.insert({theFieldName, theValue}); } },
      {DataTypes::datetime_type,  [&]() { theSet.insert({theFieldName, theValue}); } }
    };

    while (aTokenizer.current().type != TokenType::keyword && aTokenizer.current().data[0] != ';') {
      theFieldName = aTokenizer.current().data;
      aTokenizer.next();
      // Check if field exists
      const Attribute *anAttribute = anEntity->getAttribute(theFieldName);
      if (anAttribute==nullptr)
        return StatusResult{Errors::unknownAttribute};

      if (!aTokenizer.skipIf('='))
        return StatusResult{Errors::operatorExpected};
      
      if (aTokenizer.current().type != TokenType::number && 
          aTokenizer.current().type != TokenType::identifier) {
        return StatusResult{Errors::unexpectedValue};
      }

      theValue = aTokenizer.current().data;
      aTokenizer.next();      

      theMap[anAttribute->getType()]();      
    }
    return StatusResult{Errors::noError};
  }

  // ---------------  DeleteStatement  ------------------ //
  StatusResult DeleteStatement::parse(Tokenizer& aTokenizer){
    StatusResult theResult = StatusResult{Errors::noError};
    // Parse Delete
    if (!aTokenizer.skipIf(Keywords::delete_kw))
      return StatusResult{Errors::keyExpected};
    // Parse Entity Name
    if (!(theResult = parseEntity(aTokenizer)))
      return theResult;
    // Parse Where
    if (aTokenizer.current().keyword == Keywords::where_kw) {
      if (!(theResult = parseWhere(aTokenizer)))
        return theResult;
    }
    return StatusResult{Errors::noError};
  }

} // namespace ECE141
