//
//  BlockIO.cpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#include "BlockIO.hpp"
#include <cstring>

namespace ECE141 {

  Block::Block(BlockType aType) : header(aType){
    payload[0] = '\0';
  }

  Block::Block(const Block &aCopy) {
    header = aCopy.header;
    memcpy(payload, aCopy.payload, kPayloadSize);
  }

  Block& Block::operator=(const Block& aCopy) {
    header = aCopy.header;
    memcpy(payload, aCopy.payload, kPayloadSize);
    return *this;
  }
  
  //---------------------------------------------------

  StatusResult Block::write(std::iostream &aStream) {
    return StatusResult{Errors::noError};
  }

  StatusResult Block::read(std::iostream &aStream) {
    return StatusResult{Errors::noError};
  }

  //---------------------------------------------------

  BlockIO::BlockIO(std::iostream &aStream) : stream(aStream) {
    cache.setMaxSize(Config::getCacheSize(CacheType::block));
  }

  StatusResult write(std::iostream &aStream, Block &aBlock) {
    aStream.write((char*)(&aBlock), sizeof(aBlock));
    return StatusResult{Errors::noError};
  }

  StatusResult read(std::iostream &aStream, Block &aBlock) {
    aStream.read((char*)(&aBlock), sizeof(aBlock));
    return StatusResult{Errors::noError};
  }

  // USE: read and write data a given block (after seek) ---------------------------------------
  StatusResult BlockIO::writeBlock(uint32_t aBlockNum, Block &aBlock) {
    int thePos = aBlockNum * kBlockSize;
    // aBlock.header.Id = aBlockNum;
    stream.seekp(thePos,stream.beg);
    write(stream, aBlock);
    return StatusResult{Errors::noError};
  }

  StatusResult BlockIO::readBlock(uint32_t aBlockNumber, Block &aBlock) {
    // try get from block cache
    if (Config::useCache(CacheType::block)) {
      if (cache.contains(aBlockNumber)) {
        aBlock = cache.get(aBlockNumber);
      }
      int thePos = aBlockNumber * kBlockSize;
      stream.seekp(thePos,stream.beg);
      read(stream, aBlock);   
      cache.put(aBlockNumber, aBlock);
    }
    else {
      int thePos = aBlockNumber * kBlockSize;
      stream.seekp(thePos,stream.beg);
      read(stream, aBlock);   
    }   
    return StatusResult{Errors::noError};
  }

  // USE: count blocks in file ---------------------------------------
  uint32_t BlockIO::getBlockCount()  {
    stream.seekg(stream.tellg(), std::ios::beg);
    stream.seekg(0, std::ios::end);
    int thePos = (int)stream.tellg();
    return thePos / sizeof(Block); //What should this be?
  }

}
