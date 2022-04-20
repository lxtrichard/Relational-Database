
//
//  TabularView.hpp
//
//  Created by rick gessner on 4/1/22.
//  Copyright © 2022 rick gessner. All rights reserved.
//

#ifndef TabularView_h
#define TabularView_h

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include "View.hpp"
#include "Row.hpp"

namespace ECE141 {

  //STUDENT: Feel free to use this, or build your own...


  // USE: general tabular view (with columns)
  class TabularView : public View {
  public:
    
    TabularView(std::ostream &anOutput) {}
              
    // USE: create header for tablular view...
    TabularView& showHeader() {
      return *this;
    }
    
    // USE: create header for tablular view...
    TabularView& showFooter() {
      return *this;
    }
    
    // USE: this function shows all the fields in a row...
    TabularView& showRow(Row &aRow) {
      return *this;
    }
        
    // USE: this is the main show() for the view, where it presents all the rows...
    bool show() {
      std::string theSeparator=getSeparator();
      
      showHeader();
      for(auto &theRow : rows) {
        showRow(*theRow);
      }
      showFooter();
      anOutput << rows.size() << " rows in set ("
        << std::fixed << elapsed << " sec.)\n";
      return true;
    }
    
  protected:
    //Entity              &entity; //if necessary?
    RowCollection       &rows;
    std::string         separator;
    std::vector<int>    widths;
    std::ostream        &output;
  };

}

#endif /* TabularView_h */
