//
//  Index.hpp
//  RGAssignment3
//
//  Created by rick gessner on 4/2/21.
//

#ifndef Index_hpp
#define Index_hpp

#include <stdio.h>
#include <map>
#include <map>
#include <functional>
#include "Storage.hpp"
#include "BasicTypes.hpp"
#include "Errors.hpp"
#include "Helpers.hpp"


namespace ECE141 {

  enum class IndexType {intKey=0, strKey};
  
  using IndexVisitor =
    std::function<bool(const IndexKey&, uint32_t)>;
    
  struct Index : public Storable, BlockIterator {
    
    Index(Storage &aStorage, uint32_t aBlockNum=0, std::string aTableName="", 
          std::string aFieldName="", IndexType aType=IndexType::intKey)
        :  storage(aStorage), type(aType), blockNum(aBlockNum), tableName(aTableName), fieldName(aFieldName) {
          changed=false;
          entityId=0;
        }
    Index(const Index& aCopy) : storage(aCopy.storage), data(aCopy.data), type(aCopy.type),
          tableName(aCopy.tableName), fieldName(aCopy.fieldName), blockNum(aCopy.blockNum),
        changed(false) {}
    
    Index& operator=(const Index& aCopy) {
      //storage = aCopy.storage;
      data = aCopy.data;
      type = aCopy.type;
      tableName = aCopy.tableName;
      fieldName = aCopy.fieldName;
      blockNum = aCopy.blockNum;
      changed = false;
      return *this;
    }

    class ValueProxy {
    public:
      Index     &index;
      IndexKey  key;
      IndexType type;
      
      ValueProxy(Index &anIndex, uint32_t aKey)
        : index(anIndex), key(aKey), type(IndexType::intKey) {}
      
      ValueProxy(Index &anIndex, const std::string &aKey)
        : index(anIndex), key(aKey), type(IndexType::strKey) {}
      
      ValueProxy& operator= (uint32_t aValue) {
        index.setKeyValue(key,aValue);
        return *this;
      }
      
      operator IntOpt() {return index.valueAt(key);}
    }; //value proxy
    
    ValueProxy operator[](const std::string &aKey) {
      return ValueProxy(*this,aKey);
    }

    ValueProxy operator[](uint32_t aKey) {
      return ValueProxy(*this,aKey);
    }
      
    uint32_t    getBlockNum() const {return blockNum;}
    std::string getTableName() const {return tableName;}
    std::string getFieldName() const {return fieldName;}
    IndexType   getType() const {return type;}
    bool        isChanged() const {return changed;}
    std::map<IndexKey, uint32_t> getData() const {return data;}

    IndexPairs getIndexPairs() const {
      IndexPairs res;
      for (auto& cur : data) {
        if (type == IndexType::strKey) {
          std::string theKey = std::get<std::string>(cur.first);
          res.push_back({ theKey, std::to_string(cur.second) });
        }
        else {
          std::string theKey = std::to_string(std::get<std::uint32_t>(cur.first));
          res.push_back({ theKey, std::to_string(cur.second) });
        }
      }
      return res;
    }
    
    Index&   setBlockNum(uint32_t aBlockNum) {
               blockNum=aBlockNum;
               return *this;
             }
    
    bool    isChanged() {return changed;}
    Index&  setChanged(bool aChanged) {
      changed=aChanged; return *this;      
    }
        
    StorageInfo getStorageInfo(size_t aSize) {
      //student complete...
      return StorageInfo( Helpers::hashString(tableName.c_str()), aSize, blockNum, BlockType::index_block);
    }
            
    IntOpt valueAt(IndexKey &aKey) {
      return exists(aKey) ? data[aKey] : (IntOpt)(std::nullopt);
    }

    bool setKeyValue(IndexKey &aKey, uint32_t aValue) {
      data[aKey]=aValue;
      return changed=true; //side-effect indended!
    }
        
    StatusResult erase(const std::string &aKey) {
      //student implement
      if (data.count(aKey)){
        data.erase(aKey);
        changed=true;
      }
      return StatusResult{Errors::noError};
    }

    StatusResult erase(uint32_t aKey) {
      //student implement
      if (data.count(aKey)){
        data.erase(aKey);
        changed=true;
      }
      return StatusResult{Errors::noError};
    }

    size_t getSize() {return data.size();}
    
    bool exists(IndexKey &aKey) {
      return data.count(aKey);
    }
      
    StatusResult encode(std::ostream &anOutput) override {
      anOutput << tableName << ' ' << int(type) << ' ' << fieldName << ' ' << blockNum << ' ';

      anOutput << data.size() << ' ';
      for (auto thePair : data) {
        switch (thePair.first.index()) {
        case 0:
          anOutput << "i " << std::get<uint32_t>(thePair.first);
          break;
        case 1:
          anOutput << "s " << std::get<std::string>(thePair.first);
          break;
        }
        anOutput << ' ' << thePair.second << ' ';
      }
      return StatusResult{Errors::noError};
    }
    
    StatusResult decode(std::istream &anInput) override {
      std::string temp;
      anInput >> temp;
      tableName = temp;

      anInput >> temp;
      type = IndexType{ std::stoi(temp) };

      anInput >> temp;
      fieldName = temp;

      anInput >> temp;
      blockNum = std::stol(temp);

      uint32_t    theIKey;
      std::string theSKey;
      char        theType;
      uint32_t    theValue;
      size_t      theCount;

      anInput >> theCount;
      for (size_t i = 0; i < theCount; i++) {
        anInput >> theType;
        IndexKey theKey;
        switch (theType) {
        case 'i':
          anInput >> theIKey >> theValue;
          theKey = theIKey;
          break;
        case 's':
          anInput >> theSKey >> theValue;
          theKey = theSKey;
          break;
        }
        data[theKey] = theValue;
      }
      return StatusResult{Errors::noError};
    }
    
    //visit blocks associated with index
    bool each(const BlockVisitor& aVisitor) override {
      Block theBlock;
      for(auto thePair : data) {
        if(storage.readBlock(thePair.second, theBlock)) {
          if(!aVisitor(theBlock,thePair.second)) {return false;}
        }
      }
      return true;
    }

    //visit index values (key, value)...
    bool eachKV(IndexVisitor aCall) {
      for(auto thePair : data) {
        if(!aCall(thePair.first,thePair.second)) {
          return false;
        }
      }
      return true;
    }
    
  protected:
    
    Storage                       &storage;
    std::map<IndexKey, uint32_t>  data;
    IndexType                     type;
    std::string                   tableName;
    std::string                   fieldName;
    bool                          changed;
    uint32_t                      blockNum; //where index storage begins
    uint32_t                      entityId;
  }; //index

  using IndexMap = std::map<std::string, std::unique_ptr<Index> >;
}


#endif /* Index_hpp */
