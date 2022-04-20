//
//  Storage.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {

  //storage class?
  Storage::Storage(std::iostream &aStream) : BlockIO(aStream) {
  }

  Storage::~Storage() {
  }

  uint32_t Storage::getFreeBlock() {
    uint32_t res = getBlockCount();

    if (availBlocks.size()>0) { //pop an empty block number if there are any
      res = *availBlocks.begin();
      availBlocks.erase(res);          
    }

    if (availBlocks.size() == 0) { //put the next empty block into availBlocks
      availBlocks.insert(getBlockCount() + 1);
    }

    return res;
  }

  StatusResult Storage::releaseBlocks(uint32_t aBlockNum) {
    Block theBlock;
    Block freeBlock;
    bool more = true;
    while (more){
      readBlock(aBlockNum, theBlock);
      auto nextBlockNum = theBlock.header.next;
      writeBlock(aBlockNum, freeBlock);
      availBlocks.insert(aBlockNum);
      aBlockNum = nextBlockNum;
      more = (nextBlockNum != 0);
    }
    return StatusResult{Errors::noError};
  }

  StatusResult Storage::save(std::iostream &aStream, StorageInfo &anInfo) {
    size_t aStreamSize = anInfo.size;

    size_t blockIndex = 0;
    if (anInfo.start==kNewBlock)
      blockIndex = getFreeBlock();
    else
      blockIndex = anInfo.start;
    
    size_t pos = 0;
    size_t blockCount = anInfo.size / kPayloadSize;
    if (anInfo.size % kPayloadSize)
      ++blockCount;
    
    while (pos < blockCount){
      Block theBlock(anInfo.type);
      theBlock.header = BlockHeader(anInfo.type);
      theBlock.header.pos = pos;
      theBlock.header.count = blockCount;
      theBlock.header.id = anInfo.id;
      theBlock.header.next = availBlocks.size() > 0 ? *availBlocks.begin() : getBlockCount();
      theBlock.header.size = aStreamSize >= kPayloadSize ? kPayloadSize : aStreamSize;

      if (pos + 1 == blockCount){
        availBlocks.insert(theBlock.header.next);
        theBlock.header.next = 0;
      }

      if (aStreamSize >= kPayloadSize)
        aStream.read(theBlock.payload, kPayloadSize);
      else
        aStream.read(theBlock.payload, aStreamSize);
      writeBlock(blockIndex, theBlock);
      ++pos;
      aStreamSize -= kPayloadSize;
      blockIndex = theBlock.header.next;      
    }
    return StatusResult{Errors::noError};
  }

  StatusResult Storage::load(std::iostream &aStream, uint32_t aStartBlockNum){
    Block theBlock;
    bool more = true;
    while (more){
      readBlock(aStartBlockNum, theBlock);
      aStream.write(theBlock.payload, theBlock.header.size);
      aStartBlockNum = theBlock.header.next;
      more = (aStartBlockNum != 0);
    }
    return StatusResult{Errors::noError};
  }
}

