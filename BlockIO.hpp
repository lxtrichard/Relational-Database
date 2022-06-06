//
//  BlockIO.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <stdio.h>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include <map>
#include "Errors.hpp"
#include "LRUCache.hpp"

namespace ECE141 {
  enum class BlockType {
    data_block='D',
    free_block='F',
    //other types?
    unknown_block='U',
    meta_block='M',
    entity_block='E',
    index_block='I',
  };

  //a small header that describes the block...
  struct BlockHeader {
   
    BlockHeader(BlockType aType=BlockType::data_block)
      : type(static_cast<char>(aType)), id(-1),
        size(0), next(0), pos(0), count(0) {}
    
    BlockHeader(const BlockHeader &aCopy) {
      *this=aCopy;
    }
        
    void empty() {
      type=static_cast<char>(BlockType::free_block);
    }
    
    BlockHeader& operator=(const BlockHeader &aCopy) {
      type=aCopy.type;
      id=aCopy.id;
      size=aCopy.size;
      next=aCopy.next;
      pos=aCopy.pos;
      count=aCopy.count;
      return *this;
    }
   
    char      type;     //char version of block type
    int       id;       //block id
    int       pos;     //pos of block in a chain
    int       count;    //number of blocks in a chain
    uint32_t  size;     //size of block (in bytes)
    uint32_t  next;     //next block in chain
  };

  const size_t kBlockSize = 1024;
  const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);
  //block .................
  class Block {
    public:
    Block(BlockType aType=BlockType::data_block);
    Block(const Block &aCopy);
    
    Block& operator=(const Block &aCopy);
   
    StatusResult write(std::iostream &aStream);
    StatusResult read(std::iostream &aStream);
        
    BlockHeader  header;
    char         payload[kPayloadSize];
  };
  //------------------------------
  class BlockIO {
  public:
    
    BlockIO(std::iostream &aStream);
    
    uint32_t              getBlockCount();
    
    StatusResult  readBlock(uint32_t aBlockNumber, Block &aBlock);
    StatusResult  writeBlock(uint32_t aBlockNumber, Block &aBlock);

  protected:
    std::iostream &stream;
    LRUCache<uint32_t, Block> cache;
  };

}


#endif /* BlockIO_hpp */
