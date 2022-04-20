//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include <memory>
#include "CmdProcessor.hpp"
#include "Statement.hpp"

namespace ECE141 {
    
  CmdProcessor::CmdProcessor(std::ostream &anOutput)
    : output(anOutput){
  }

  CmdProcessor::~CmdProcessor() {}
  
  CmdProcessor* CmdProcessor::recognizes(Tokenizer &aTokenizer) {
    return nullptr;
  }

}
