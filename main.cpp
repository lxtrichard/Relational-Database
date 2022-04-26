//
//  main.cpp
//  Database2
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <functional>

#include "TestManually.hpp"
#include "TestAutomatic.hpp"

//----------------------------------------------

int main(int argc, const char * argv[]) {
        
  srand(static_cast<uint32_t>(time(0)));
    
  if(argc>1) {
    
    std::stringstream theOutput;
    ECE141::TestAutomatic theTests(theOutput);

    using TestCall = std::function<bool()>;
    static std::map<std::string, TestCall> theCalls {
      {"App",         [&](){return theTests.doAppTest();}  },
      {"Compile",     [&](){return theTests.doCompileTest();}  },
      {"Reader",      [&](){return theTests.doReaderTest();}  },
      {"DBCommands",  [&](){return theTests.doDBCommandsTest();}  },
      {"Tables",      [&](){return theTests.doTablesTest();}  },
      {"Insert",      [&](){return theTests.doInsertTest();}  },
      {"Select",      [&](){return theTests.doSelectTest();}  },
    };
    
    std::string theCmd(argv[1]);
    if(theCalls.count(theCmd)) {
      bool theResult = theCalls[theCmd]();
      const char* theStatus[]={"FAIL","PASS"};
      std::cout << theCmd << " test " << theStatus[theResult] << "\n";
      std::cout << "------------------------------\n"
        << theOutput.str() << "\n";
    }
    else std::cout << "Unknown test\n";
  }
  else {
    doManualTesting();
  }
  return 0;
}
