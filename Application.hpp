//
//  AppProcessor.hpp
//  Database1
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Application_hpp
#define Application_hpp

#include <stdio.h>
#include <sstream>
#include "CmdProcessor.hpp"
#include "DBProcessor.hpp"
#include "SQLProcessor.hpp"

namespace ECE141 {

  class Application : public CmdProcessor {
  public:
    
    Application(std::ostream &anOutput);
    virtual ~Application();

      //app api...
            std::string   getVersion() {return "0.6";}
    
    virtual StatusResult  handleInput(std::istream &anInput);
            CmdProcessor* recognizes(Tokenizer &aTokenizer) override;
            Statement*    makeStatement(Tokenizer &aTokenizer,
                                        StatusResult &aResult) override;
            StatusResult  run(Statement *aStmt) override;
  protected:
    DBProcessor theDBProcessor;
    SQLProcessor theSQLProcessor;
  };
  
}

#endif /* Application_hpp */
