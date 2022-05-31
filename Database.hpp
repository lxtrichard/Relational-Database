//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"
#include "View.hpp"
#include "Row.hpp"
#include "DBQuery.hpp"
#include "TabularView.hpp"
#include "Joins.hpp"
#include "Index.hpp"

namespace ECE141 {

  class Database : public Storable{
  public:
    
            Database(const std::string aPath, CreateDB);
            Database(const std::string aPath, OpenDB);
            ~Database();

    StatusResult    dump(std::ostream &anOutput);
    std::string     getName() {return name;}
    Entity*         getEntity(const std::string &aName);

    StatusResult    createTable(std::ostream &anOutput, Entity &anEntity);
    StatusResult    showTables(std::ostream &anOutput);
    StatusResult    describeTable(std::ostream &anOutput, const std::string &aName);
    StatusResult    dropTable(std::ostream &anOutput, const std::string &aName);
    StatusResult    insertRows(std::ostream &anOutput, 
                      const std::string &aName,
                      const std::vector<std::string> anAttributeNames, 
                      const std::vector<std::vector<std::string>>& aValues);
    StatusResult    selectRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery);
    StatusResult    selectJoins(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery, JoinList& aJoins);
    StatusResult    updateRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery, KeyValues &anUpdateSet);
    StatusResult    deleteRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery);

    IndexPairs      getIndex(const std::string &aName, std::vector<std::string> aFields);
    IndexPairs      getAllIndex();
    void            insertIndexes(std::vector<Index*> anIndexes, KeyValues& aKeyValue, uint32_t blockNum);
    void            deleteIndexes(KeyValues& aKeyValue);
    void            deleteAllIndexes(std::string aTableName);
    StatusResult    showIndexes(std::ostream &anOutput);
    StatusResult    showIndex(std::ostream &anOutput, std::string aTableName, std::string aFieldName);

    StatusResult    encode(std::ostream &aWriter) override;
    StatusResult    decode(std::istream &aReader) override;

  protected:
    RowCollection   findRows(std::shared_ptr<DBQuery> aQuery);
    
    std::string     name;
    std::fstream    stream;   //low level stream used by storage...
    Storage         theStorage;
    bool            changed;  //might be helpful, or ignore if you prefer.

    std::vector<Entity> theEntityList;
    KeyIndexes          theEntityIndexes;
    std::set<uint32_t>  theIndexBlockNum; //block numbers of index blocks
    std::vector<Index>  theIndexList; // index list
  };

}
#endif /* Database_hpp */
