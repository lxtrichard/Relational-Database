//
//  Row.hpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Attribute.hpp"

//feel free to use this, or create your own version...

namespace ECE141 {

  class Row  {
  public:

    Row(uint32_t entityId=0);
    Row(const Row &aRow);
    
   // Row(const Attribute &anAttribute); //maybe?
    
    ~Row();
    
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
      //STUDENT: What other methods do you require?
                          
    Row&                set(const std::string &aKey,
                            const Value &aValue);
        
    KeyValues&          getData() {return data;}
    
    //uint32_t            entityId; //hash value of entity?
    //uint32_t            blockNumber;

  protected:
    KeyValues           data;
  };

  //-------------------------------------------

  using RowCollection = std::vector<std::unique_ptr<Row> >;


}
#endif /* Row_hpp */
