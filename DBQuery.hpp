#ifndef DBQuery_hpp
#define DBQuery_hpp

#include <stdio.h>
#include <string>
#include <optional>
#include <vector>
#include "Attribute.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "Tokenizer.hpp"
#include "Filters.hpp"

namespace ECE141
{
  class DBQuery
  {
  public:
    DBQuery();
    DBQuery(const DBQuery &aCopy);
    ~DBQuery();
    DBQuery &operator=(const DBQuery &aCopy);

    // getters
    bool        getSelectAll() { return isAll; };
    StringList  getSelects() { return theAttNames; };
    std::string getEntityName() { return theEntityName; };
    Entity*     getEntity() { return theEntity; };
    std::string getOrderBy() { return theOrderBy; };
    uint32_t    getLimit() { return theLimit; };
    StringList  getExcludes() { return theExcludes; }; // return excludes

    // setters
    DBQuery& setSelectAll(bool aValue);
    DBQuery& setSelects(const std::string &aField);
    DBQuery& setEntityName(const std::string &aName);
    DBQuery& setEntity(Entity *aEntity);
    DBQuery& setOrderBy(const std::string &aField);
    DBQuery& setLimit(uint32_t aLimit);
    DBQuery& setExcludes(const std::string &aField);

    // filters
    StatusResult parseFilters(Tokenizer& aTokenizer);
    bool         matches(KeyValues &aList);

  protected:
    bool        isAll;
    std::string theEntityName;
    Entity*     theEntity;
    StringList  theAttNames;
    std::string theOrderBy;
    Filters     theFilters;
    uint32_t    theLimit;
    StringList  theExcludes; // excludes
  };
} // namespace ECE141


#endif /* DBQuery_hpp */