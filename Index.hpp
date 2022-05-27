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


namespace ECE141 {

  enum class IndexType {intKey=0, strKey};
  
  using IndexVisitor =
    std::function<bool(const IndexKey&, uint32_t)>;
    
  struct Index : public Storable, BlockIterator {
    
    Index(Storage &aStorage, uint32_t aBlockNum=0,
          IndexType aType=IndexType::intKey)
        :  storage(aStorage), type(aType), blockNum(aBlockNum) {
          changed=false;
          entityId=0;
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
      
    uint32_t getBlockNum() const {return blockNum;}
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
      return StatusResult{Errors::noError};
    }

    StatusResult erase(uint32_t aKey) {
      //student implement
      return StatusResult{Errors::noError};
    }

    size_t getSize() {return data.size();}
    
    bool exists(IndexKey &aKey) {
      return data.count(aKey);
    }
      
    StatusResult encode(std::ostream &anOutput) override {
      //student implement
      return StatusResult{Errors::noError};
    }
    
    StatusResult decode(std::istream &anInput) override {
      //student implement...
      return StatusResult{Errors::noError};
    }
    
    //visit blocks associated with index
    bool each(BlockVisitor aVisitor) override {
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
    std::string                   name;
    bool                          changed;
    uint32_t                      blockNum; //where index storage begins
    uint32_t                      entityId;
  }; //index

  using IndexMap = std::map<std::string, std::unique_ptr<Index> >;

}


#endif /* Index_hpp */
