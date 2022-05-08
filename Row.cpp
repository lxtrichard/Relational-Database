//
//  Row.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//


#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Row.hpp"


namespace ECE141 {
  
  Row::Row(const Row &aRow) : data(aRow.data), blockNumber(aRow.blockNumber) {}

  Row::~Row() {}

  bool Row::operator==(Row &aCopy) const {return false;}

  //STUDENT: What other methods do you require?
                      
  Row& Row::set(const std::string &aKey,const Value &aValue) {
    return *this;
  }

  bool Row::updateData(KeyValues &aData) {
    for (auto &i : aData) {
      if (data.count(i.first)) {
        data[i.first] = i.second;
      }
      else{
        return false;
      }
    }
    return true;
  }

  void Row::setData(const std::string &aKey,const Value &aValue) {
    data[aKey] = aValue;
  }

  template<typename T>
  static char vtype(const T& aVar) {
    static char theTypes[] = { 'b','i','d','s' };
    return theTypes[aVar.index()];
  }

  static std::ostream& operator<< (std::ostream& out, const Value& aValue) {
    std::visit([theType = vtype(aValue), &out](auto const& aValue)
    { out << theType << ' ' << '\"' << aValue << '\"'; }, aValue);
    return out;
  }
  
  StatusResult Row::encode(std::ostream& anOutput){
    anOutput << blockNumber << " ";
    for (auto &i : data) {
      anOutput << i.first << " ";
      if (i.second.index() != 3)
        anOutput << i.second << " ";
      else
        anOutput << i.second << " ";
    }
    return StatusResult();
  }

  static bool stob(std::string aStr) {
    if (stoi(aStr))
      return true;
    else
      return false;
  }

  static std::string readValue(std::istream& aReader) {
    std::string theRes = "";
    aReader.get();
    aReader.get();
    while (!aReader.eof() && aReader.peek() != '\"') {
      char ch = aReader.get();
      theRes += ch;
    }
    aReader.get();
    return theRes;
  }

  StatusResult Row::decode(std::istream& aReader) {
    std::string temp;
    std::string key;
    aReader >> temp;
    blockNumber = stoul(temp);
    while (aReader >> temp) {
      key = temp;
      aReader >> temp;
      char theType = temp[0];
      //aReader >> temp;
      temp = readValue(aReader);
      switch (theType) {
        case 'b':
          data[key] = stob(temp);
          break;
        case 'i':
          data[key] = std::stoi(temp);
          break;
        case 'd':
          data[key] = std::stod(temp);
          break;
        case 's':
          data[key] = temp;
          break;
      }
    }
    return StatusResult{};
  }


    
}
