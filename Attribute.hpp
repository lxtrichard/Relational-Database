//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Storage.hpp"

namespace ECE141{
      
/*
  struct Property {
    Property(std::string aName, int aTableId=0) : name(aName), tableId(aTableId), desc(true) {}
    std::string     name;
    int             tableId;
    bool            desc;
  };

  using PropertyList = std::vector<Property>;
*/
  class Attribute : public Storable{
  protected:
    std::string   name;
    DataTypes     type;
    uint16_t      size : 10; //max=1000
    uint16_t      autoIncrement : 1;
    uint16_t      primary : 1;
    uint16_t      nullable: 1;
    //Others?
    bool          hasdefault;
    Value         defaultValue;

  public:
          
    Attribute(DataTypes aType=DataTypes::no_type);
    Attribute(std::string aName, DataTypes aType, uint32_t aSize=0);
    Attribute(const Attribute &aCopy);
    ~Attribute();
    
    Attribute&          setName(std::string &aName);
    Attribute&          setDataType(DataTypes aType);
    Attribute&          setSize(int aSize);
    Attribute&          setAutoIncrement(bool anAuto);
    Attribute&          setPrimaryKey(bool anAuto);
    Attribute&          setNullable(bool aNullable);
    Attribute&          setDefault(bool aDefault);
    Attribute&          setDefaultValue(std::string aValue);
    
    bool                isValid(); //is this Entity valid?
    
    const std::string&  getName() const {return name;}
    DataTypes           getType() const {return type;}
    size_t              getSize() const {return size;}
    void                setDefaultValue(Value aDefault) { defaultValue = aDefault; }
    Value               toValue(const std::string &aValue) const;
    std::string         DValuetoString() const;
    Value               getDefaultValue() const {return defaultValue;}

    bool                isPrimaryKey() const {return primary;}
    bool                isNullable() const {return nullable;}
    bool                isAutoIncrement() const {return autoIncrement;}  
    bool                hasDefault() const {return hasdefault;}  

    StatusResult encode(std::ostream& anOutput) override;
    StatusResult decode(std::istream& anInput) override;
  };
  
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;

}


#endif /* Attribute_hpp */
