//
//  AboutUs.hpp
//  About your team members...
//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <stdio.h>
#include <string>
#include <optional>

namespace ECE141 {
  
  using StringOpt = std::optional<std::string>;
  
  class AboutUs {
  public:
    AboutUs()=default;
    ~AboutUs()=default;
    
    size_t getTeamSize() {return 2;} //STUDENT: update this...
    
    StringOpt getName(size_t anIndex) const {
      //return name of student anIndex N (or nullopt)
      switch (anIndex) {
        case 0: return "Xiangtian";
        case 1: return "Harshit Bhadani";
        default: return std::nullopt;
      }
    }
    StringOpt getGithubUsername(size_t anIndex) const {
      //return github username of student anIndex N (or nullopt)
      switch (anIndex) {
        case 0: return "lxtrichard";
        case 1: return "harshitb8";
        default: return std::nullopt;
      }
    }

  };

}

#endif /* about_me */
