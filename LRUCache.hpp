#ifndef LRUCache_h
#define LRUCache_h

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <utility>

#include "BasicTypes.hpp"
#include "Config.hpp"
#include "Errors.hpp"

namespace ECE141 {
  template<typename KeyT, typename ValueT>
  class LRUCache {
  public:
        
    //OCF 
    LRUCache(size_t aMaxSize=0) : maxsize(aMaxSize) {}
    ~LRUCache() {}

    void    put(const KeyT &key, const ValueT& value){
      if(theCache.find(key)!=theCache.end()) {
        theCacheList.splice(theCacheList.begin(),theCacheList,theCache[key]);
        theCache[key]->second = value;
        return;
      }
      if(theCacheList.size() == maxsize) {
        int theKey = theCacheList.back().first;
        theCacheList.pop_back();
        theCache.erase(theKey);
      }
      theCacheList.push_front({key,value});
      theCache[key] = theCacheList.begin();
    }

    ValueT& get(const KeyT& key){
      auto it = theCache.find(key);
      theCacheList.splice(theCacheList.begin(), theCacheList, it->second);
      return it->second->second;
    }

    bool    contains(const KeyT& key) const{
      if (theCache.count(key) > 0) {
        return true;
      }
      return false;
    }
    
    size_t  size() const{
      return maxsize;
    } //current size

    LRUCache& setMaxSize(size_t aMaxSize){
      maxsize = aMaxSize;
      return *this;
    }

  protected:
    size_t maxsize; //prevent cache from growing past this size...

    //data members here...
    std::list< std::pair<KeyT,ValueT> > theCacheList;
    std::unordered_map<KeyT, decltype(theCacheList.begin())> theCache;
  };
}

#endif /* LRUCache_h */