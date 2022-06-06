//
//  Database.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <algorithm>
#include "Database.hpp"
#include "Config.hpp"

namespace ECE141 {
  
  Database::Database(const std::string aName, CreateDB)
    : name(aName), changed(true), theStorage(stream) {
    std::string thePath = Config::getDBPath(name);
    //create for read/write
    stream.clear();
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);
    stream.close();
    stream.open(thePath.c_str(), std::fstream::binary | std::fstream::binary | std::fstream::in | std::fstream::out);
    
    std::stringstream ss;
    if (encode(ss) == Errors::noError) {
      StorageInfo info(0, ss.str().size(), kNewBlock, BlockType::meta_block);
      theStorage.save(ss, info);
    }
  }

  Database::Database(const std::string aName, OpenDB)
    : name(aName), changed(false), theStorage(stream) {
    std::string thePath = Config::getDBPath(name);
    stream.open(thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
    
    std::stringstream ss;
    if (theStorage.load(ss, 0) == Errors::noError) {
      decode(ss);
    }
    
    // load the entities of the database into theEntityList
    for (auto &it : theEntityIndexes) {
      std::stringstream ss1;
      theStorage.load(ss1, it.second);
      Entity theEntity(it.first);
      theEntity.decode(ss1);
      theEntityList.push_back(theEntity);
    }

    // load the indexes of the database into theIndexMap
    for (auto& cur : theIndexBlockNum){
      std::stringstream ss2;
      theStorage.load(ss2, cur);
      Index theIndex(theStorage, cur);
      theIndex.decode(ss2);
      theIndexList.push_back(theIndex);
    }
  }

  Database::~Database() {
    if(changed) {
      std::stringstream ss;
      this->encode(ss);
      StorageInfo info(0, ss.str().size(), 0, BlockType::meta_block);
      theStorage.save(ss, info);
      
      // write the entities of the database from theEntityList
      for (auto& entity : theEntityList){
        std::stringstream ss1;
        entity.encode(ss1);
        size_t pos = theEntityIndexes[entity.getName()];
        StorageInfo info(entity.hashString(), ss1.str().size(), pos, BlockType::entity_block);
        theStorage.save(ss1, info);
      }

      // write the indexes of the database from theIndexMap
      for (auto& cur : theIndexList){
        if (cur.isChanged()){
          std::stringstream ss2;
          cur.encode(ss2);
          StorageInfo theInfo = cur.getStorageInfo(ss2.str().size());
          theStorage.save(ss2, theInfo);
        }
      }
    }
    stream.close();
  }

  // Get Index
  IndexPairs Database::getIndex(const std::string &aName, std::vector<std::string> aFields) {
    IndexPairs res;
    for (auto& index : theIndexList){
      if (index.getTableName() == aName){
        if (std::find(aFields.begin(), aFields.end(), index.getFieldName()) != aFields.end()){
          IndexPairs thePairs = index.getIndexPairs();
          res.insert(res.end(), thePairs.begin(), thePairs.end());
        }
      }
    }
    return res;
  }

  IndexPairs Database::getAllIndex(){
    IndexPairs res;
    for (auto& index : theIndexList){
      IndexPairs thePairs = index.getIndexPairs();
      res.insert(res.end(), thePairs.begin(), thePairs.end());
    }
    return res;
  }

  // insert Index
  void Database::insertIndexes(std::vector<Index*> anIndexes, KeyValues& aKeyValue, uint32_t blockNum){
    for (auto* index : anIndexes) {
      std::string theField = index->getFieldName();
      if (index->getType() == IndexType::intKey) {
        uint32_t theKey = std::get<int>(aKeyValue[theField]);
        Index::ValueProxy theProxy = (*index)[theKey];
        theProxy = blockNum;
      }
      else {
        std::string theKey = std::get<std::string> (aKeyValue[theField]);
        Index::ValueProxy theProxy = (*index)[theKey];
        theProxy = blockNum;
      }
    }
  }

  // remove Indexes
  void Database::deleteIndexes(KeyValues& aKeyValue, std::string aTableName){
    for (auto& index : theIndexList){
      if (index.getTableName()!=aTableName)
        continue;
      std::string theField = index.getFieldName();
      if (index.getType() == IndexType::intKey) {
        uint32_t theKey = std::get<int>(aKeyValue[theField]);
        index.erase(theKey);
      }
      else {
        std::string theKey = std::get<std::string> (aKeyValue[theField]);
        index.erase(theKey);
      }
    }
  }

  void Database::deleteAllIndexes(std::string aTableName){
    std::vector<Index> newIndexes;
    for (auto& index : theIndexList){
      if (index.getTableName() != aTableName){
        newIndexes.push_back(index);
      }
      else{
        theStorage.releaseBlocks(index.getBlockNum());
        theIndexBlockNum.erase(index.getBlockNum());
      }
    }
    theIndexList = newIndexes;
  }

  StatusResult Database::showIndexes(std::ostream &anOutput){
    TabularView theView(anOutput);
    theView.showIndexes(theIndexList);
    size_t theRowNum = theIndexList.size();
    anOutput << theRowNum << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
    return StatusResult{Errors::noError};
  }

  StatusResult Database::showIndex(std::ostream &anOutput, std::string aTableName, std::string aFieldName){
    for (auto& index : theIndexList){
      if (index.getTableName() == aTableName && index.getFieldName() == aFieldName){
        TabularView theView(anOutput);
        theView.showIndex(index);
        size_t theRowNum = index.getSize();
        anOutput << theRowNum << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
        return StatusResult{Errors::noError};
      }
    }
    return StatusResult{Errors::noError};
  }
  
  static bool stob(std::string aStr) {
    return stoi(aStr);
  }

  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::dump(std::ostream &anOutput) {    
    return StatusResult{noError};
  }

  // USE: Call this to create a table... 
  StatusResult Database::createTable(std::ostream &anOutput, Entity &anEntity) {
    anOutput << std::setprecision(3) << std::fixed;
    std::string theName = anEntity.getName();
    if (theEntityIndexes.find(theName) != theEntityIndexes.end()) {
      return StatusResult{Errors::tableExists};
    }
    // write Entity 
    uint32_t blockNum = theStorage.getFreeBlock();
    theEntityList.push_back(anEntity);
    theEntityIndexes[anEntity.getName()] = blockNum;

    std::stringstream ss;
    anEntity.encode(ss);
    StorageInfo info(anEntity.hashString(), ss.str().size(), blockNum, BlockType::entity_block);
    theStorage.save(ss, info);

    // create an Index
    const Attribute* primaryAtt = anEntity.getPrimaryKey();
    IndexType theType;
    if (primaryAtt->getType() == DataTypes::int_type)
      theType = IndexType::intKey;
    else
      theType = IndexType::strKey; 

    // add index
    uint32_t theBlockNum = theStorage.getFreeBlock();
    Index theIndex(theStorage, theBlockNum, theName, primaryAtt->getName(), theType);
    theIndexList.push_back(theIndex);
    theIndexBlockNum.insert(theBlockNum);

    // write index to file 
    std::stringstream ss1;
    theIndex.encode(ss1);
    StorageInfo info1 = theIndex.getStorageInfo(ss1.str().size());
    theStorage.save(ss1, info1);

    // write meta block
    std::stringstream ss2;
    this->encode(ss2);
    StorageInfo info2(0, ss2.str().size(), 0, BlockType::meta_block);
    theStorage.save(ss2, info2);

    anOutput << "Query OK, 1 row affected (" <<  Config::getTimer().elapsed() << " sec)" << std::endl;
    changed = true;
    return StatusResult{noError};
  }

  StatusResult Database::alterTable(std::ostream &anOutput, std::string theTableName, std::vector<Attribute> theAttributes) {
    anOutput << std::setprecision(3) << std::fixed;
    if (theEntityIndexes.find(theTableName) == theEntityIndexes.end()) {
      return StatusResult{Errors::unknownTable};
    }
    Entity* theEntity = getEntity(theTableName);
    for (auto& att : theAttributes) {
      theEntity->addAttribute(att);
    }
    uint32_t theEntityBlockNum = theEntityIndexes[theTableName];
    // write Entity
    std::stringstream ss;
    theEntity->encode(ss);
    StorageInfo info(theEntity->hashString(), ss.str().size(), theEntityBlockNum, BlockType::entity_block);
    theStorage.save(ss, info);

    anOutput << "Query OK, 0 row affected (" <<  Config::getTimer().elapsed() << " sec)" << std::endl;
    changed = true;
    return StatusResult{noError};
  }

  StatusResult Database::showTables(std::ostream &anOutput){
    TableView theView(anOutput);
    theView.showTables(name, theEntityIndexes);
    return StatusResult{noError};
  }

  StatusResult Database::describeTable(std::ostream &anOutput, const std::string &aName){
    Entity* theEntity = getEntity(aName);
    bool result = theEntity;
    if (result){
      TableView theView(anOutput);
      theView.describeTables(theEntity);
    }
    else{
      std::cout << "Query failed, table not found";
      return StatusResult{Errors::unknownTable};
    }
    return StatusResult{noError};
  }

  StatusResult Database::dropTable(std::ostream &anOutput, const std::string &aName){
    anOutput << std::setprecision(3) << std::fixed;
    if (!theEntityIndexes.count(aName)) {
      return StatusResult{Errors::unknownTable};
    }
    // remove the rows
    std::shared_ptr<DBQuery> aQuery(new DBQuery());
    aQuery->setEntityName(aName);
    RowCollection theRowCollection = findRows(aQuery);
    for (auto& theRow : theRowCollection) {
      theStorage.releaseBlocks(theRow->getBlockNumber());
    }

    // remove the entity
    uint32_t blockNum = theEntityIndexes[aName];
    theEntityIndexes.erase(aName);
    for (size_t i = 0; i < theEntityList.size(); i++) {
      if (theEntityList[i].getName() == aName) {
        theEntityList.erase(theEntityList.begin() + i);
        break;
      }
    }
    theStorage.releaseBlocks(blockNum);

    // remove the index
    deleteAllIndexes(aName);

    // rewrite the meta block
    std::stringstream ss;
    this->encode(ss);
    StorageInfo info(0, ss.str().size(), 0, BlockType::meta_block);
    theStorage.save(ss, info);
    // show the result
    anOutput << "Query OK, "<< theRowCollection.size()+1 << " rows affected (" << Config::getTimer().elapsed() << " sec)" << std::endl;
    changed = true;
    return StatusResult{noError};
  }

  Entity* Database::getEntity(const std::string &aName){
    for (auto& entity : theEntityList) {
      if (entity.getName() == aName) {
        return &entity;
      }
    }
    return nullptr;
  }

  static void buildKeyValueList(std::vector<KeyValues> &aList, 
      Entity* anEntity,
      const std::vector<std::string> anAttributeNames, 
      const std::vector<std::vector<std::string>>& aValues) {
    int n = aValues.size(); // number of rows
    int m = aValues[0].size(); // number of cols
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        auto* theAtt = anEntity->getAttribute(anAttributeNames[j]);
        switch (theAtt->getType()) {
          case DataTypes::bool_type:
            aList[i][theAtt->getName()] = stob(aValues[i][j]);
            break;
          case DataTypes::int_type:
            aList[i][theAtt->getName()] = std::stoi(aValues[i][j]);
            break;
          case DataTypes::float_type:
            aList[i][theAtt->getName()] = std::stof(aValues[i][j]);
            break;
          case DataTypes::varchar_type:
            aList[i][theAtt->getName()] = aValues[i][j].substr(0, theAtt->getSize());
            break;
          case DataTypes::datetime_type:
            aList[i][theAtt->getName()] = aValues[i][j];
            break;
          default:
            break;
        }
      }
    }
  }

  StatusResult Database::insertRows(std::ostream &anOutput, 
                const std::string &aName,
                const std::vector<std::string> anAttributeNames, 
                const std::vector<std::vector<std::string>>& aValues){
    anOutput << std::setprecision(3) << std::fixed;
    Entity* theTable = getEntity(aName);
    if (!theTable) {
      return StatusResult{Errors::unknownTable};
    }
    std::vector<KeyValues> theKeyValueList(aValues.size());
    buildKeyValueList(theKeyValueList, theTable, anAttributeNames, aValues);
    int RowAffected = 0;
    uint32_t theTableHash = theTable->hashString();

    std::vector<Index*> theTableIndexes;
    for (auto& theIndex : theIndexList) {
      if (theIndex.getTableName() == theTable->getName())
        theTableIndexes.push_back(&theIndex);
    }

    for (auto& keyvalue : theKeyValueList) {
      uint32_t blockNum = theStorage.getFreeBlock();
      int id = theTable->getIncrement();
      keyvalue["id"] = id;
      Row theRow(keyvalue, blockNum);

      // theRowIndexes[theTable->getName()].push_back(blockNum);
      insertIndexes(theTableIndexes, keyvalue, blockNum);
      
      std::stringstream ss;
      theRow.encode(ss);
      StorageInfo info(theTableHash, ss.str().size(), blockNum, BlockType::data_block);
      theStorage.save(ss, info);
      RowAffected += 1;
    }

    // write index to file 
    for (auto theIndex : theTableIndexes) {
      std::stringstream ss1;
      theIndex->encode(ss1);
      StorageInfo info1 = theIndex->getStorageInfo(ss1.str().size());
      theStorage.save(ss1, info1);
    }

    // write meta block
    std::stringstream ss2;
    this->encode(ss2);
    StorageInfo info2(0, ss2.str().size(), 0, BlockType::meta_block);
    theStorage.save(ss2, info2);

    changed = true;
    anOutput << "Query OK, " << RowAffected << " rows affected (" <<  Config::getTimer().elapsed() << " sec)" << std::endl;
    return StatusResult{noError};
  }

  RowCollection Database::findRows(std::shared_ptr<DBQuery> aQuery){
    Entity *anEntity = getEntity(aQuery->getEntityName());
    uint32_t theHashStr = anEntity->hashString();
    RowCollection theRowCollection;
    std::string primaryKey = anEntity->getPrimaryKey()->getName(); // the primary key
    // get all data blocks for the entity
    for (auto& index : theIndexList) {
      if (index.getTableName() == aQuery->getEntityName() && index.getFieldName() == primaryKey) {
        index.each([&](const Block& theBlock, uint32_t theIdx)->bool
        {
          std::stringstream ss;
          ss.write(theBlock.payload, theBlock.header.size);
          std::unique_ptr<Row> theRow(new Row());
          theRow->decode(ss);
          if (aQuery->matches(theRow->getData())){
            theRowCollection.push_back(std::move(theRow));
          }
          return true;
        }
        );
      }
    }
    return theRowCollection;
  }

  StatusResult Database::selectRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery){
    anOutput << std::setprecision(3) << std::fixed;

    // find rows that match the query
    RowCollection theRowCollection = findRows(aQuery);

    TabularView theView(anOutput);
    theView.show(aQuery, theRowCollection);
    // get the size of the selected rows
    size_t theRowNum = std::min(theRowCollection.size(), static_cast<size_t>(aQuery->getLimit()));
    anOutput << theRowNum << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
    return StatusResult{noError};
  }

  static RowCollection combineRows(JoinMap& theLeftMap, JoinMap& theRightMap){
    RowCollection theJoinRows;
    Value theKey;
    for (auto& cur : theLeftMap){
      // iterate the left map
      theKey = cur.first;
      for (auto& lRow : cur.second){
        // iterate the left rows
        auto& theRightRows = theRightMap[theKey];
        if (theRightRows.size()==0){
          theJoinRows.push_back(std::move(lRow));
        }

        for (auto& rRow : theRightRows){
          // iterate the right rows
          Row theRow;
          KeyValues& leftData = lRow->getData();
          KeyValues& theData = theRow.getData();
          for (auto& cur1 : rRow->getData()){
            if (leftData.count(cur1.first) == 0)
              leftData[cur1.first] = cur1.second;
          }
          for (auto& cur2 : leftData){
            theData[cur2.first] = cur2.second;
          }
          theJoinRows.push_back(std::make_unique<Row>(theRow));
        }
      }
    }
    return theJoinRows;
  }

  StatusResult Database::selectJoins(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery, JoinList& aJoins){
    anOutput << std::setprecision(3) << std::fixed;

    std::string leftTableName = aQuery->getEntityName();
    std::string rightTableName = aJoins[0].table;

    std::shared_ptr<DBQuery> theLeftQuery(new DBQuery());
    std::shared_ptr<DBQuery> theRightQuery(new DBQuery());
    theLeftQuery->setEntityName(leftTableName);
    theRightQuery->setEntityName(rightTableName);

    // find rows that match the query
    RowCollection theLeftRowCollection = findRows(theLeftQuery);
    RowCollection theRightRowCollection = findRows(theRightQuery);

    // get the key map of the right table
    JoinMap theLeftMap;
    Value theKey;
    for (auto& lRow : theLeftRowCollection){
      theKey = lRow->getData()[aJoins[0].lhs.fieldName];
      theLeftMap[theKey].push_back(std::move(lRow));
    }

    // get the key map of the right table
    JoinMap theRightMap;
    for (auto& rRow : theRightRowCollection){
      theKey = rRow->getData()[aJoins[0].rhs.fieldName];
      theRightMap[theKey].push_back(std::move(rRow));
    }

    // combine the rows
    RowCollection theJoinRows = combineRows(theLeftMap, theRightMap);
    TabularView theView(anOutput);
    theView.show(aQuery, theJoinRows);
    anOutput << theJoinRows.size() << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
    return StatusResult{noError};
  }

  StatusResult Database::updateRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery, KeyValues &anUpdateSet){
    anOutput << std::setprecision(3) << std::fixed;
    Entity *anEntity = getEntity(aQuery->getEntityName());
    uint32_t theHashStr = anEntity->hashString();
    
    // find rows that match the query
    RowCollection theRowCollection = findRows(aQuery);

    // update rows
    for (auto& theRow : theRowCollection) {
      for (auto& theKeyValue : anUpdateSet) {
        theRow->setData(theKeyValue.first, theKeyValue.second);
      }
      std::stringstream ss;
      theRow->encode(ss);
      theStorage.releaseBlocks(theRow->getBlockNumber());
      StorageInfo info(theHashStr, ss.str().size(), theRow->getBlockNumber(), BlockType::data_block);
      theStorage.save(ss, info);
    }
    anOutput << "Query Ok. " << theRowCollection.size() << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
    changed = true;
    return StatusResult{noError};
  }
  
  StatusResult Database::deleteRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery){
    anOutput << std::setprecision(3) << std::fixed;

    // find rows that match the query
    RowCollection theRowCollection = findRows(aQuery);

    // delete rows
    for (auto& theRow : theRowCollection) {
      deleteIndexes(theRow->getData(), aQuery->getEntityName()); // delete indexes
      theStorage.releaseBlocks(theRow->getBlockNumber());
    }
    
    anOutput << "Query Ok. " << theRowCollection.size() << " rows in set (" << Config::getTimer().elapsed() << " sec)\n";
    changed = true;
    return StatusResult{noError};
  }


  // encode and decode
  StatusResult Database::encode(std::ostream &aWriter) {
    aWriter << name << ' ';
    for (auto& cur : theEntityIndexes) {
      aWriter << cur.first << ' ' << cur.second << ' ';
    }
    aWriter << "# ";
    for (auto& cur : theIndexBlockNum) {          
          aWriter << cur << ' ';
      }
    aWriter << "# ";
    return StatusResult{Errors::noError};
  }

  StatusResult Database::decode(std::istream &aReader) {
    aReader >> name;
    std::string temp;
    while (aReader >> temp) {
      if (temp == "#")
          break;
      std::string theName = temp;
      aReader >> temp;
      uint32_t theBlockNum = std::stoul(temp);
      theEntityIndexes[theName] = theBlockNum;
    }
    while (aReader >> temp) {
      if (temp == "#")
          break;
      uint32_t theBlockNum = std::stoul(temp);
      theIndexBlockNum.insert(theBlockNum);
    }
    return StatusResult{Errors::noError};
  }
}
