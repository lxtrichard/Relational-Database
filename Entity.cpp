//
//  Entity.cpp
//  PA3
//
//  Created by rick gessner on 3/2/22.
//

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include "Entity.hpp"

namespace ECE141 {

 //STUDENT: Implement this class...

  Entity::Entity(const std::string aName) :
    name(aName),  autoincr(1) {}

  Entity::Entity(const Entity &aCopy) {
    name=aCopy.name;
    autoincr=aCopy.autoincr;
    for (auto& attr : aCopy.attributes) {
      attributes.push_back(attr);
    }
  }

  uint32_t Entity::hashString() {
    const char * str = name.c_str();
    uint32_t h{0};
    unsigned char *p;
    const int gMultiplier = 37;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }
 
  Entity& Entity::addAttribute(const Attribute &anAttribute) {
    if(!getAttribute(anAttribute.getName())) {
      attributes.push_back(anAttribute);
    }
    return *this;
  }

  const Attribute* Entity::getAttribute(const std::string &aName) const {
    for (auto& att : attributes) {
      if (att.getName()==aName) {
        return &att;
      }
    }
    return nullptr;
  }
  
  // USE: ask the entity for name of primary key (may not have one...)
  const Attribute* Entity::getPrimaryKey() const {   
    for (auto& att : attributes) {
      if (att.isPrimaryKey()) {
        return &att;
      }
    }
    return nullptr;
  }

  StatusResult Entity::encode(std::ostream &anOutput) {
    // anOutput << "Version 0.4";
    anOutput << name << ' ' << autoincr << ' ';

    for (auto attribute : attributes) {
      attribute.encode(anOutput);
    }

    anOutput << '#' << ' '; //an eof flag

    return StatusResult{noError};
  }

  StatusResult Entity::decode(std::istream &anInput) {
    // std::string temp;
    // anInput >> temp;
    anInput >> name;

    std::string temp;
    anInput >> temp;
    autoincr = stoul(temp);

    anInput.get(); //eat the space

    //decode attributes
    while (anInput.peek() != '#') {
      Attribute att;
      att.decode(anInput);
      attributes.push_back(att);
    }

    return StatusResult{noError};
  }

}
