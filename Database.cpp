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

    for (auto &it : theTableIndexes) {
      std::stringstream ss1;
      theStorage.load(ss1, it.second);
      Entity theEntity(it.first);
      theEntity.decode(ss1);
      theEntityList.push_back(theEntity);
    }

  }

  Database::~Database() {
    if(changed) {
      std::stringstream ss;
      this->encode(ss);
      StorageInfo info(0, ss.str().size(), 0, BlockType::meta_block);
      theStorage.save(ss, info);

      for (auto& entity : theEntityList){
        std::stringstream ss1;
        entity.encode(ss1);
        size_t pos = theTableIndexes[entity.getName()];
        StorageInfo info(entity.hashString(), ss1.str().size(), pos, BlockType::entity_block);
        theStorage.save(ss1, info);
      }
    }
    stream.close();
  }
 

  // USE: Call this to dump the db for debug purposes...
  StatusResult Database::dump(std::ostream &anOutput) {    
    return StatusResult{noError};
  }

  // USE: Call this to create a table... 
  StatusResult Database::createTable(std::ostream &anOutput, Entity &anEntity) {
    Timer theTimer;
    anOutput << std::setprecision(3) << std::fixed;
    std::string theName = anEntity.getName();
    if (theTableIndexes.find(theName) != theTableIndexes.end()) {
      return StatusResult{Errors::tableExists};
    }
    uint32_t blockNum = theStorage.getFreeBlock();
    theEntityList.push_back(anEntity);
    theTableIndexes[anEntity.getName()] = blockNum;

    std::stringstream ss;
    anEntity.encode(ss);
    StorageInfo info(anEntity.hashString(), ss.str().size(), blockNum, BlockType::entity_block);
    theStorage.save(ss, info);

    anOutput << "Query OK, 1 row affected (" <<  theTimer.elapsed() << " sec)" << std::endl;
    changed = true;
    return StatusResult{noError};
  }

  StatusResult Database::showTables(std::ostream &anOutput){
    TableView theView(anOutput);
    theView.showTables(name, theTableIndexes);
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
    Timer theTimer;
    anOutput << std::setprecision(3) << std::fixed;
    if (!theTableIndexes.count(aName)) {
      return StatusResult{Errors::unknownTable};
    }
    uint32_t blockNum = theTableIndexes[aName];
    theTableIndexes.erase(aName);
    for (size_t i = 0; i < theEntityList.size(); i++) {
      if (theEntityList[i].getName() == aName) {
        theEntityList.erase(theEntityList.begin() + i);
        break;
      }
    }
    theStorage.releaseBlocks(blockNum);
    changed = true;
    std::stringstream ss;
    this->encode(ss);
    StorageInfo info(0, ss.str().size(), 0, BlockType::meta_block);
    theStorage.save(ss, info);
    anOutput << "Query OK, 1 rows affected (" << theTimer.elapsed() << " sec)" << std::endl;
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

  static bool stob(std::string aStr) {
    if (stoi(aStr))
      return true;
    else
      return false;
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
    Timer theTimer;
    anOutput << std::setprecision(3) << std::fixed;
    Entity* theTable = getEntity(aName);
    if (!theTable) {
      return StatusResult{Errors::unknownTable};
    }
    std::vector<KeyValues> theKeyValueList(aValues.size());
    buildKeyValueList(theKeyValueList, theTable, anAttributeNames, aValues);
    int RowAffected = 0;
    uint32_t theTableHash = theTable->hashString();
    for (auto& keyvalue : theKeyValueList) {
      uint32_t blockNum = theStorage.getNextFreeBlock();
      int id = theTable->getIncrement();
      keyvalue["id"] = id;
      Row theRow(keyvalue, blockNum);

      theRowIndexes[theTable->getName()].push_back(blockNum);
      std::stringstream ss;
      theRow.encode(ss);
      StorageInfo info(theTableHash, ss.str().size(), kNewBlock, BlockType::data_block);
      theStorage.save(ss, info);
      RowAffected += 1;
    }
    changed = true;
    anOutput << "Query OK, " << RowAffected << " rows affected (" <<  theTimer.elapsed() << " sec)" << std::endl;
    return StatusResult{noError};
  }

  StatusResult Database::selectRows(std::ostream &anOutput, std::shared_ptr<DBQuery> aQuery){
    Timer theTimer;
    anOutput << std::setprecision(3) << std::fixed;
    Entity *anEntity = getEntity(aQuery->getEntityName());
    aQuery->setEntity(anEntity);
    uint32_t theHashStr = anEntity->hashString();
    RowCollection theRowCollection;
    // get all data blocks for the entity
    theStorage.each([&theRowCollection, theHashStr](const Block& theBlock, uint32_t theIdx)->bool{
      if (theBlock.header.type == static_cast<char>(BlockType::data_block))
        if (theBlock.header.id == theHashStr){
          std::stringstream ss;
          ss.write(theBlock.payload, theBlock.header.size);
          std::unique_ptr<Row> theRow(new Row());
          theRow->decode(ss);
          theRowCollection.push_back(std::move(theRow));
        }
      return true;
    });

    TabularView theView(anOutput);
    theView.show(aQuery, theRowCollection);
    anOutput << theRowCollection.size() << " rows in set (" << theTimer.elapsed() << " sec)\n";
    return StatusResult{noError};
  };


  StatusResult Database::encode(std::ostream &aWriter) {
    aWriter << name << ' ';
    for (auto& cur : theTableIndexes) {
      aWriter << cur.first << ' ' << cur.second << ' ';
    }
    aWriter << "# ";
    for (auto& cur : theRowIndexes) {
      aWriter << cur.first << ' ';
      for (auto& blockNum : cur.second) {
        aWriter << blockNum << ' ';
      }
      aWriter << "# ";
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
      theTableIndexes[theName] = theBlockNum;
    }
    while (aReader >> temp) {
      if (temp == "#")
          break;
      std::string theName = temp;
      std::vector<uint32_t> theBlockNums;
      while (aReader >> temp) {
        if (temp == "#")
            break;
        uint32_t theBlockNum = std::stoul(temp);
        theBlockNums.push_back(theBlockNum);
      }
      theRowIndexes[theName] = theBlockNums;
    }
    return StatusResult{Errors::noError};
  }
}
