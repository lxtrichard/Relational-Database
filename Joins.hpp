//
//  Join.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/21/21.
//

#ifndef Join_h
#define Join_h

#include <string>
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "keywords.hpp"
#include "Row.hpp"

namespace ECE141 {
  struct TableField{
    TableField(const TableField& aCopy) : tableName(aCopy.tableName), fieldName(aCopy.fieldName) {}

    TableField(const std::string &aTableName = "", const std::string &aFieldName = "") : 
      tableName(aTableName), fieldName(aFieldName) {}

    std::string tableName;
    std::string fieldName;
  };

  class Join  {
  public:
    Join(const std::string &aTable, Keywords aType)
      : table(aTable), joinType(aType) {}
            
    std::string table;
    Keywords    joinType;  //left, right, etc...
    TableField  lhs;
    TableField  rhs;
  };

  using JoinList = std::vector<Join>;
  using JoinMap = std::map<Value, std::vector<std::unique_ptr<Row>>>;

}

#endif /* Join_h */
