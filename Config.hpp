//
//  Config.hpp
//
//  Created by rick gessner on 2/27/22.
//

#ifndef Config_h
#define Config_h
#include <sstream>
#include "Timer.hpp"

namespace ECE141 {

  enum class CacheType : int {block=0, row, view};

  struct Config {

    static const char* getDBExtension() {return ".db";}
    static size_t cacheSize[3];

    static const std::string getStoragePath() {        
      #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        //STUDENT: If you're on windows, return a path to folder on your machine...
        return "D:/ProgramData/Database";
      
      #elif __APPLE__ || defined __linux__ || defined __unix__
        return std::string("/tmp");
      #endif
    }
    
    static std::string getDBPath(const std::string &aDBName) {
      std::ostringstream theStream;
      theStream << Config::getStoragePath() << "/" << aDBName << ".db";
      return theStream.str();
    }
      
    static Timer& getTimer() {
      static Timer theTimer;
      return theTimer;
    }
    
    //cachetype: block, row, view...
    static size_t getCacheSize(CacheType aType) {
      return cacheSize[(int)aType];
    }

    static void setCacheSize(CacheType aType, size_t aSize) {
      cacheSize[(int)aType]=aSize;
    }
    
    //cachetype: block, row, view...
    static bool useCache(CacheType aType) {
      return cacheSize[(int)aType]>0;
    }

    static bool useIndex() {return true;}
        
  };

}

#endif /* Config_h */
