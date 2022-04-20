//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include <iostream>
#include "Attribute.hpp"

namespace ECE141 {

  Attribute::Attribute(DataTypes aType)
    : type(aType),size(0),autoIncrement(0),primary(0),nullable(1),hasdefault(false),defaultValue(0) {}
 
  Attribute::Attribute(std::string aName, DataTypes aType, uint32_t aSize)  {
    name=aName;
    type=aType;
    size=aSize;
    autoIncrement=0;
    primary=0;
    nullable=1;
    hasdefault=false;
    defaultValue=0;
  }
 
  Attribute::Attribute(const Attribute &aCopy)  {
    name=aCopy.name;
    type=aCopy.type;
    size=aCopy.size;
    autoIncrement=aCopy.autoIncrement;
    primary=aCopy.primary;
    nullable=aCopy.nullable;
    hasdefault=aCopy.hasdefault;
    defaultValue=aCopy.defaultValue;
  }
 
  Attribute::~Attribute()  {
  }
 
  Attribute& Attribute::setName(std::string &aName)  {
    return *this;
  }
 
  Attribute& Attribute::setDataType(DataTypes aType) {
    type=aType;
    return *this;
  }

  Attribute& Attribute::setSize(int aSize) {
    size=aSize; return *this;
  }

  Attribute& Attribute::setAutoIncrement(bool anAuto) {
    autoIncrement=anAuto; return *this;
  }

  Attribute& Attribute::setPrimaryKey(bool aPrimary) {
    primary=aPrimary; return *this;
  }

  Attribute& Attribute::setNullable(bool aNullable) {
    nullable=aNullable; return *this;
  }

  Attribute& Attribute::setDefault(bool aDefault) {
    hasdefault=aDefault; return *this;
  }

  Attribute& Attribute::setDefaultValue(std::string aValue){
    defaultValue=aValue; return *this;
  }

  bool Attribute::isValid() {
    return true;
  }
   
  Value Attribute::toValue(const std::string &aValue) const {
    //might need to do type-casting here...    
    return Value{aValue};
  }

  std::string Attribute::DValuetoString() const {
    switch (type)
    {
    case DataTypes::int_type:
      return std::to_string(std::get<int>(defaultValue));
    case DataTypes::float_type:
    {
      std::string temp = std::to_string(std::get<double>(defaultValue));
      return temp.substr(0, temp.find(".") + 2 + 1);
    }
    case DataTypes::bool_type:
      return std::get<bool>(defaultValue) ? "true" : "false";
    default:
      return std::get<std::string>(defaultValue);
    }
  }

  template<typename T>
  static char vtype(const T& aVar) {
    static char theTypes[] = { 'b','i','d','s' };
    return theTypes[aVar.index()];
  }

  static std::ostream& operator<< (std::ostream& out, const Value& aValue) {
    std::visit([theType = vtype(aValue), &out](auto const& aValue)
    { out << theType << ' ' << aValue; }, aValue);
    return out;
  }

  StatusResult Attribute::encode(std::ostream& anOutput) {
    anOutput  << name << ' '
              << char(type) << ' '
              << size << ' '
              << autoIncrement << ' '
              << primary << ' '
              << nullable << ' '
              << hasdefault << ' '
              << defaultValue << ' ';
    return StatusResult{};
  }
    
  //string to boolean
  static bool stob(std::string aStr) {
    if (stoi(aStr))
      return true;
    else
      return false;
  }

  StatusResult Attribute::decode(std::istream& aReader) {
    std::string temp;
    
    //name
    aReader >> name;

    //type
    aReader >> temp;
    type = DataTypes{ temp[0] };

    //size
    aReader >> temp;
    size = std::stoi(temp);

    //auto_increment
    aReader >> temp;
    autoIncrement = stob(temp);

    //primary_key
    aReader >> temp;
    primary = stob(temp);

    //nullable
    aReader >> temp;
    nullable = stob(temp);

    //has default
    aReader >> temp;
    hasdefault = stob(temp);

    //defaultValue
    aReader >> temp;
    char theType = temp[0];
    aReader >> temp;
    switch (theType) {
    case 'b':
      defaultValue = stob(temp);
      break;
    case 'i':
      defaultValue = std::stoi(temp);
      break;
    case 'd':
      defaultValue = std::stod(temp);
      break;
    case 's':
      defaultValue = temp;
      break;
    }

    //eat the space
    aReader.get();

    return StatusResult{};
  }

}
