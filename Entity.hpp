//
//  Entity.hpp
//  Assignment3
//
//  Created by rick gessner on 3/18/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include <vector>
#include <optional>
#include <memory>
#include <string>

#include "Attribute.hpp"
#include "Errors.hpp"
#include "BasicTypes.hpp"

namespace ECE141 {
  
  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;

  //------------------------------------------------

  class Entity : public Storable{
  public:
                          Entity(const std::string aName);
                          Entity(const Entity &aCopy);
    
                          ~Entity() {};
    
    const std::string&    getName() const {return name;}
    uint32_t              hashString();
        
    const AttributeList&  getAttributes() const {return attributes;}
    Entity&               addAttribute(const Attribute &anAttribute);
    Entity&               removeAttribute(const std::string &aName);
    const Attribute*      getAttribute(const std::string &aName) const;
    const Attribute*      getPrimaryKey() const;

    uint32_t              getIncrement() {return autoincr++;}


    StatusResult encode(std::ostream& anOutput) override;
    StatusResult decode(std::istream& anInput) override;
           
  protected:
        
    AttributeList   attributes;
    std::string     name;
    uint32_t        autoincr;  //auto_increment
  };
  
}
#endif /* Entity_hpp */
