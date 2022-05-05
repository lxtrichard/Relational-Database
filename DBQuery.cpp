#include "DBQuery.hpp"

namespace ECE141
{
  DBQuery::DBQuery() : theEntityName(""), isAll(false), theEntity(nullptr), theOrderBy("id") {}

  DBQuery::DBQuery(const DBQuery &aCopy) {
    *this = aCopy;
  }

  DBQuery::~DBQuery() {}

  DBQuery& DBQuery::operator=(const DBQuery &aCopy) {
    theEntityName = aCopy.theEntityName;
    theAttNames = aCopy.theAttNames;
    isAll = aCopy.isAll;
    return *this;
  }

  DBQuery& DBQuery::setSelectAll(bool aValue){
    isAll = aValue;
    return *this;
  }

  DBQuery& DBQuery::setSelects(const std::string &aField){
    isAll = false;
    theAttNames.push_back(aField);
    return *this;
  }  

  DBQuery& DBQuery::setEntityName(const std::string &aName){
    theEntityName = aName;
    return *this;
  }

  DBQuery& DBQuery::setEntity(Entity *aEntity){
    theEntity = aEntity;
    return *this;
  }

  DBQuery& DBQuery::setOrderBy(const std::string &aField){
    theOrderBy = aField;
    return *this;
  }

} // namespace ECE141
