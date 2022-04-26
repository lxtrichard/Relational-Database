//
//  FolderView.hpp
//  Assignment2
//
//  Created by rick gessner on 2/15/21.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderView_h
#define FolderView_h

#include "FolderReader.hpp"
#include "View.hpp"
#include <iomanip>

namespace ECE141 {

  // USE: provide view class that lists db files in storage path...
  class FolderView {
  public:
    FolderView(const char *aPath, const char *anExtension="db")
      : reader(aPath), extension(anExtension) {}
   
    virtual bool show(std::ostream &anOutput) {
      reader.each(extension, [&](const std::string &aName) {
        anOutput << aName << std::endl;
        return true;
      });
      return true;
    }
    
    FolderReader  reader;
    const char    *extension;
    
  };

}

#endif /* FolderView_h */
