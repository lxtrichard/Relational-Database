
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
#include "DBQuery.hpp"
#include "Index.hpp"

namespace ECE141 {

  //STUDENT: Feel free to use this, or build your own...


  // USE: general tabular view (with columns)
  class TabularView : public View {
  public:
    TabularView(std::ostream &anOutput);

    ~TabularView() {}

    TabularView& showSeparator(std::shared_ptr<DBQuery>& aQuery);
    TabularView& showHeader(std::shared_ptr<DBQuery>& aQuery);
    TabularView& showRow(std::shared_ptr<DBQuery>& aQuery, std::unique_ptr<Row>& aRow);
    TabularView& showIndexes(std::vector<Index> &anIndexes);
    TabularView& showIndex(Index &anIndex);

    bool show(std::shared_ptr<DBQuery>& aQuery, RowCollection &aRows);
    
  protected:
    std::map<std::string, size_t>     widths;
  };

}

#endif /* TabularView_h */
