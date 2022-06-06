//
//  Created by rick gessner on 3/23/22.
//

#ifndef AboutMe_hpp
#define AboutMe_hpp

#include <stdio.h>
#include <string>

namespace ECE141 {
  
  class AboutMe {
  public:
    AboutMe() : name("Xiangtian Li"), pid("A59012851") {} 
    
    const std::string& getPID() const {return pid;}
    const std::string& getName() const {return name;}

  protected:
    std::string name;
    std::string pid;
  };

}

#endif /* about_me */
