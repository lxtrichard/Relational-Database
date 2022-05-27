#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Database.hpp"

namespace ECE141
{
  SQLStatement::SQLStatement(Database* aDB, Keywords aStmtType) 
      : theDB(aDB), Statement(aStmtType){}

  StatusResult SQLStatement::run(std::ostream &aStream) const {
    return StatusResult{noError};
  }

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

  StatusResult CreateStatement::run(std::ostream &aStream){
    Entity* theEntity = new Entity(thetableName);
    for (auto &theAttribute : attributes){
      theEntity->addAttribute(theAttribute);
    }
    StatusResult theResult = theDB->createTable(aStream, *theEntity);
    return theResult;
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

  StatusResult  ShowStatement::run(std::ostream &aStream){
    StatusResult theResult = theDB->showTables(aStream);
    return theResult;
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

  StatusResult  DescribeStatement::run(std::ostream &aStream){
    const std::string &aName = thetableName;
    StatusResult theResult = theDB->describeTable(aStream, aName);
    return theResult;
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

  StatusResult DropStatement::run(std::ostream &aStream){
    const std::string &aName = thetableName;
    StatusResult theResult = theDB->dropTable(aStream, aName);
    return theResult;
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

  StatusResult InsertStatement::run(std::ostream &aStream){
    StatusResult theResult = theDB->insertRows(aStream, thetableName, theAttributeNames, values);
    return theResult;
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
    if (!theResult)
      return theResult;
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
    // check if entity exists
    if (anEntity == nullptr)
      return StatusResult{Errors::unknownEntity};

    theQuery->setEntity(anEntity);
    aTokenizer.next();
    return StatusResult{Errors::noError};
  }
  
  StatusResult SelectStatement::parseClause(Keywords aKeyword, Tokenizer& aTokenizer){
    using Clauseparser = StatusResult (SelectStatement::*)(Tokenizer&);
    static std::map<Keywords, Clauseparser> theClauseMap = {
      {Keywords::where_kw, &SelectStatement::parseWhere},
      {Keywords::order_kw, &SelectStatement::parseOrderBy},
      {Keywords::limit_kw, &SelectStatement::parseLimit},
      {Keywords::join_kw, &SelectStatement::parseJoin},
      {Keywords::left_kw, &SelectStatement::parseJoin},
      {Keywords::right_kw, &SelectStatement::parseJoin},
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

  static StatusResult parseTableName(Tokenizer& aTokenizer, std::string& aTableName){
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    aTableName = aTokenizer.current().data;
    aTokenizer.next();
    return StatusResult{Errors::noError};
  }

  static StatusResult parseTableField(Tokenizer& aTokenizer, TableField& aTableField){
    std::string temp = aTokenizer.current().data;

    if (temp.find('.') == std::string::npos)
      return StatusResult{ Errors::syntaxError };

    aTableField.tableName = temp.substr(0, temp.find('.'));
    aTableField.fieldName = temp.substr(temp.find('.')+1, std::string::npos);
    aTokenizer.next();

    return StatusResult{Errors::noError};
  }

  //jointype JOIN tablename ON table1.field=table2.field
  StatusResult SelectStatement::parseJoin(Tokenizer &aTokenizer) {
    Token &theToken = aTokenizer.current();
    StatusResult theResult{joinTypeExpected}; //add joinTypeExpected to your errors file if missing...

    Keywords theJoinType{Keywords::join_kw}; //could just be a plain join
    if(in_array<Keywords>(gJoinTypes, theToken.keyword)) {
      theJoinType=theToken.keyword;
      aTokenizer.next(1); //yank the 'join-type' token (e.g. left, right)
      if(aTokenizer.skipIf(Keywords::join_kw)) {
        std::string theTable;
        if((theResult=parseTableName(aTokenizer, theTable))) {
          Join theJoin(theTable, theJoinType);
          theResult = StatusResult{keyExpected}; //on...
          if(aTokenizer.skipIf(Keywords::on_kw)) { //LHS field = RHS field
            TableField LHS("");
            if((theResult=parseTableField(aTokenizer, theJoin.lhs))) {
              if(aTokenizer.skipIf('=')) {
                if((theResult=parseTableField(aTokenizer, theJoin.rhs))) {
                  if (theJoinType == Keywords::right_kw) { // convert to right join
                    // reset the right table
                    std::string temp = theJoin.table;
                    theJoin.table = theQuery->getEntityName();
                    // reset the left table
                    theQuery->setEntityName(temp);
                    Entity *anEntity = theDB->getEntity(temp);
                    theQuery->setEntity(anEntity);                    
                  }
                  if (theJoin.lhs.tableName == theJoin.table && 
                      theJoin.rhs.tableName == theQuery->getEntityName()) { // swap the lhs and rhs
                    TableField tempField = theJoin.lhs;
                    theJoin.lhs = theJoin.rhs;
                    theJoin.rhs = tempField;
                  }
                  joins.push_back(theJoin);
                }
              }
            }
          }
        }
      }
    }
    return theResult;
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

  StatusResult SelectStatement::run(std::ostream& aStream){
    StatusResult theResult = StatusResult{Errors::noError};
    if (joins.size() > 0) {
      theResult = theDB->selectJoins(aStream, theQuery, joins);
    }
    else{
      theResult = theDB->selectRows(aStream, theQuery);
    }
    return theResult;
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
      aTokenizer.skipIf(','); // skip comma

      theMap[anAttribute->getType()]();      
    }
    return StatusResult{Errors::noError};
  }

  StatusResult UpdateStatement::run(std::ostream& aStream){
    StatusResult theResult = theDB->updateRows(aStream, theQuery, theSet);
    return theResult;
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

  StatusResult DeleteStatement::run(std::ostream& aStream){
    StatusResult theResult = theDB->deleteRows(aStream, theQuery);
    return theResult;
  }

} // namespace ECE141
