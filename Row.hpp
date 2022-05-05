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
#include "Storage.hpp"

//feel free to use this, or create your own version...

namespace ECE141 {

  class Row : public Storable {
  public:

    Row() :blockNumber(0) {};
    Row(const Row &aRow);
    Row(KeyValues &aData, uint32_t aBlockNumber) 
       : data(aData), blockNumber(aBlockNumber) {};
    
   // Row(const Attribute &anAttribute); //maybe?
    
    ~Row();
    
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
      //STUDENT: What other methods do you require?
                          
    Row&                set(const std::string &aKey,
                            const Value &aValue);
    bool                updateData(KeyValues &aData);
    void                setData(const std::string &aKey,
                            const Value &aValue);
        
    KeyValues&          getData() {return data;}
    Value               getValue(const std::string &aKey) {return data[aKey];};
    uint32_t            getBlockNumber() {return blockNumber;}
    
    //uint32_t            entityId; //hash value of entity?
    //uint32_t            blockNumber;
    StatusResult encode(std::ostream& anOutput) override;
    StatusResult decode(std::istream& anInput) override;

  protected:
    KeyValues           data;
    uint32_t            blockNumber;
  };

  //-------------------------------------------

  using RowCollection = std::vector<std::unique_ptr<Row> >;
  using OrderedRow = std::map<std::string, RowCollection>;

}
#endif /* Row_hpp */
