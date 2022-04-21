//
//  ParseHelper.hpp
//  RGAssignment4
//
//  Created by rick gessner on 4/18/21.
//

#ifndef ParseHelper_hpp
#define ParseHelper_hpp

#include <stdio.h>
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Tokenizer.hpp"
#include "Attribute.hpp"

namespace ECE141 {

  //-------------------------------------------------
  
  class Entity;
  
  struct ParseHelper {
            
    ParseHelper(Tokenizer &aTokenizer) : tokenizer(aTokenizer) {}
                
    StatusResult parseAttributeOptions(Attribute &anAttribute);

    StatusResult parseAttribute(Attribute &anAttribute);

    StatusResult parseIdentifierList(StringList &aList);

    StatusResult parseKeyValues(KeyValues &aList,Entity &anEnity);
    
    StatusResult parseValueList(StringList &aList);
        
  protected:
    Tokenizer &tokenizer;
  };

}

#endif /* ParseHelper_hpp */

