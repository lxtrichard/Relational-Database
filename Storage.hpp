//
//  Storage.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <string>
#include <fstream>
#include <iostream>
#include <deque>
#include <stack>
#include <set>
#include <optional>
#include <functional>
#include "BlockIO.hpp"
#include "Errors.hpp"

namespace ECE141 {
      
  struct CreateDB {}; //tags for db-open modes...
  struct OpenDB {};

  //not required -- but we discussed in class...
  using BlockList = std::set<uint32_t>;
  const int32_t kNewBlock=-1;
  
  class Storable {
  public:
    virtual StatusResult  encode(std::ostream &anOutput)=0;
    virtual StatusResult  decode(std::istream &anInput)=0;
  };
  
  struct StorageInfo {
    StorageInfo(size_t anId, size_t aSize, uint32_t aStart=0, BlockType aType=BlockType::data_block)
    : id(anId), size(aSize), start(aStart), type(aType)  {}
    uint32_t  start;
    size_t    size;
    size_t    id;
    BlockType type;
  };

  // USE: A storage class, might be helpful...
  class Storage : public BlockIO {
  public:
    Storage(std::iostream &aStream);
    ~Storage();

    StatusResult    save(std::iostream &aStream, StorageInfo &anInfo);
    StatusResult    load(std::iostream &aStream, uint32_t aStartBlockNum);

    StatusResult    releaseBlocks(uint32_t aBlockNum);
    uint32_t        getFreeBlock();
  protected:
    BlockList     availBlocks;
  };

}


#endif /* Storage_hpp */
