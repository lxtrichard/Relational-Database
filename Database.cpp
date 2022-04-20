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
        StorageInfo info(pos, ss1.str().size(), pos, BlockType::data_block);
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
    std::string theName = anEntity.getName();
    if (theTableIndexes.find(theName) != theTableIndexes.end()) {
      return StatusResult{Errors::tableExists};
    }
    uint32_t blockNum = theStorage.getFreeBlock();
    theEntityList.push_back(anEntity);
    theTableIndexes[anEntity.getName()] = blockNum;

    std::stringstream ss;
    anEntity.encode(ss);
    StorageInfo info(blockNum, ss.str().size(), blockNum, BlockType::entity_block);
    theStorage.save(ss, info);

    anOutput << "Query OK, 1 row affected (" << theTimer.elapsed() << " sec)" << std::endl;
    changed = true;
    return StatusResult{noError};
  }

  StatusResult Database::showTables(std::ostream &anOutput){
    Timer theTimer;

    anOutput << "+----------------------+" << std::endl;
    anOutput << "| Tables_in_" << std::setw(10) << std::left << name << " |" << std::endl;
    anOutput << "+----------------------+" << std::endl;
    for (auto& cur : theTableIndexes) {
      anOutput << "| " << std::setw(20) << std::left << cur.first <<  " |" << std::endl;
    }
    anOutput << "+----------------------+" << std::endl;
    anOutput << theTableIndexes.size() <<" rows in set ("<< theTimer.elapsed() << " sec.)" << std::endl;
    stream.close();
    return StatusResult{noError};
  }

  StatusResult Database::describeTable(std::ostream &anOutput, const std::string &aName){
    Timer theTimer;
    for (auto &theEntity : theEntityList) {
      if (theEntity.getName() == aName) {
        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;
        anOutput << "| Field     | Type         | Null | Key | Default | Extra                       |" << std::endl;
        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;
        Helpers aHelper;
        for (auto attribute : theEntity.getAttributes()) {
          std::string theType = aHelper.dataTypeToString(attribute.getType());
          if (attribute.getType() == DataTypes::varchar_type) {
            theType.append("(");
            theType.append(std::to_string(attribute.getSize()));
            theType.append(")");
          }

          anOutput << "| " << std::setw(10) << std::left << attribute.getName() << "| " 
                   << std::setw(13) << std::left << theType << "| ";
          if (attribute.isNullable()) {
            anOutput << std::setw(5) << std::left << "YES" << "| ";
          } 
          else {
            anOutput << std::setw(5) << std::left << "NO" << "| ";
          }

          if (attribute.isPrimaryKey()) {
            anOutput << std::setw(4) << std::left << "YES" << "| ";
          } 
          else {
            anOutput << std::setw(4) << std::left << "" << "| ";
          }

          anOutput << std::setw(8) << std::left;
          if (attribute.hasDefault()) {
            anOutput << attribute.DValuetoString() << "| ";
          } 
          else {
            anOutput << "NULL" << "| ";
          }

          anOutput << std::setw(28) << std::left << "" << "|" ;
          anOutput << std::endl;
        }
        anOutput << "+-----------+--------------+------+-----+---------+-----------------------------+" << std::endl;
        anOutput << theEntity.getAttributes().size() << " rows in set ("<< theTimer.elapsed() << " sec.)" << std::endl;
        return StatusResult{noError};
      }
    }
    return StatusResult{Errors::unknownTable};
  }

  StatusResult Database::dropTable(std::ostream &anOutput, const std::string &aName){
    Timer theTimer;
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
    for (auto entity : theEntityList) {
      if (entity.getName() == aName) {
        return &entity;
      }
    }
  }

  StatusResult Database::encode(std::ostream &anOutput) {
    anOutput << name << ' ';
    for (auto& cur : theTableIndexes) {
      anOutput << cur.first << ' ' << cur.second << ' ';
    }
    anOutput << "# ";
    return StatusResult{Errors::noError};
  }

  StatusResult Database::decode(std::istream &anInput) {
    anInput >> name;
    std::string temp;
    while (anInput >> temp) {
      if (temp == "#")
          break;
      std::string theName = temp;
      anInput >> temp;
      uint32_t theBlockNum = std::stoul(temp);
      theTableIndexes[theName] = theBlockNum;
    }
    return StatusResult{Errors::noError};
  }
}
