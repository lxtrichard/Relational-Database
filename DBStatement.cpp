#include "DBStatement.hpp"
#include "Tokenizer.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "Database.hpp"

namespace ECE141
{
  DBStatement::DBStatement(Keywords aStmtType) : Statement(aStmtType){}

  StatusResult DBStatement::run(std::ostream &aStream) {
    return StatusResult{noError};
  }

  static bool stob(std::string aStr) {
    return stoi(aStr);
  }

  // ---------------  CreateStatement  -------------------- //

  StatusResult CreateDBStatement::parse(Tokenizer &aTokenizer){
    if (!aTokenizer.skipIf(Keywords::create_kw))
      return StatusResult{Errors::unknownCommand};

    if (!aTokenizer.skipIf(Keywords::database_kw))
      return StatusResult{Errors::unknownCommand};
    
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    
    setDBName(aTokenizer.current().data);
    aTokenizer.next();

    return StatusResult{Errors::noError};
  }

  // ---------------  DropStatement  ------------------ //
  StatusResult DropDBStatement::parse(Tokenizer &aTokenizer){
    
    if (!aTokenizer.skipIf(Keywords::drop_kw))
      return StatusResult{Errors::unknownCommand};

    if (!aTokenizer.skipIf(Keywords::database_kw))
      return StatusResult{Errors::unknownCommand};
    
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    
    setDBName(aTokenizer.current().data);
    aTokenizer.next();

    return StatusResult{Errors::noError};
  }

  // ---------------  ShowStatement  ------------------ //
  StatusResult ShowDBStatement::parse(Tokenizer &aTokenizer){
    
    if (!aTokenizer.skipIf(Keywords::show_kw))
      return StatusResult{Errors::unknownCommand};

    if (!aTokenizer.skipIf(Keywords::databases_kw))
      return StatusResult{Errors::unknownCommand};

    return StatusResult{Errors::noError};
  }

  // ---------------  UseStatement  ------------------ //
  StatusResult UseStatement::parse(Tokenizer &aTokenizer){
    
    if (!aTokenizer.skipIf(Keywords::use_kw))
      return StatusResult{Errors::unknownCommand};
    
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    
    setDBName(aTokenizer.current().data);
    aTokenizer.next();

    return StatusResult{Errors::noError};
  }

  // ---------------  DumpStatement  ------------------ //
  StatusResult DumpStatement::parse(Tokenizer &aTokenizer){
    
    if (!aTokenizer.skipIf(Keywords::dump_kw))
      return StatusResult{Errors::unknownCommand};

    if (!aTokenizer.skipIf(Keywords::database_kw))
      return StatusResult{Errors::unknownCommand};
    
    if (aTokenizer.current().type != TokenType::identifier)
      return StatusResult{Errors::identifierExpected};
    
    setDBName(aTokenizer.current().data);
    aTokenizer.next();

    return StatusResult{Errors::noError};
  }

} // namespace ECE141
