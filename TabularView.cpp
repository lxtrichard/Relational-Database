#include <algorithm>
#include "TabularView.hpp"

namespace ECE141{
  TabularView::TabularView(std::ostream &anOutput) : View(anOutput) {}

  static std::ostream& operator<< (std::ostream& out, const Value& aValue) {
    std::visit([&out](auto const& aValue)
    { out << aValue; }, aValue);
    return out;
  }

  static void reorderRow(std::shared_ptr<DBQuery>& aQuery, RowCollection& aCollection, OrderedRow& anOrderedRow) {
    std::string orderBy = "id";
    if (aQuery->getOrderBy() != "id") {
      orderBy = aQuery->getOrderBy();
    }

    for (auto& row : aCollection) {
      std::stringstream ss;
      Value aValue = row->getValue(orderBy);
      anOrderedRow[aValue].push_back(std::move(row));
    }
  }
  
  // Get the width of the columns
  static std::map<std::string, size_t> getAttMaxSize(RowCollection& aRows, const size_t kRowWidth) {
    std::map<std::string, size_t> res;
    for (auto& row : aRows) {
      for (auto& cur : row->getData()) {
        std::string* str = std::get_if<std::string>(&cur.second);
        res[cur.first] = std::max(res[cur.first], cur.first.size() + 1);
        if (str)
          res[cur.first] = std::max(res[cur.first], str->size() + 1);
        else
          res[cur.first] = std::max(kRowWidth, res[cur.first]);
      }
    }
    return res;
  }

  static StringList getSelectList(std::shared_ptr<DBQuery>& aQuery){
    bool selectAll = aQuery->getSelectAll();
    StringList selectList;
    if (selectAll){
      for (auto& att : aQuery->getEntity()->getAttributes())
        selectList.push_back(att.getName());
    }
    else{
      selectList = aQuery->getSelects();
    }
    if (!std::count(selectList.begin(), selectList.end(), "id"))
      selectList.insert(selectList.begin(), "id");
    return selectList;
  }

  TabularView& TabularView::showSeparator(std::shared_ptr<DBQuery>& aQuery){
    bool selectAll = aQuery->getSelectAll();
    StringList selectList = getSelectList(aQuery);
    output << "+";

    for (auto& cur : selectList) {
      if (selectAll || std::count(selectList.begin(), selectList.end(), cur)) {
        for (int i=0;i<=widths[cur];i++)
          output << "-";
        output << "+";
      }
    }
    output << std::endl;
    return *this;
  }

  TabularView& TabularView::showHeader(std::shared_ptr<DBQuery>& aQuery) {
    bool selectAll = aQuery->getSelectAll();
    StringList selectList = getSelectList(aQuery);

    for (auto& cur : selectList) {
      if (selectAll || std::count(selectList.begin(), selectList.end(), cur)) {
        output << "| ";
        output << std::setw(widths[cur]) << std::left << cur;
      }
    }
    output <<  "|" << std::endl;
    return *this;
  }
  
  TabularView& TabularView::showRow(std::shared_ptr<DBQuery>& aQuery, std::unique_ptr<Row>& aRow) {
    bool selectAll = aQuery->getSelectAll();
    StringList selectList = getSelectList(aQuery);
    KeyValues data = aRow->getData();


    for (auto& cur : selectList) {
      if (selectAll || std::count(selectList.begin(), selectList.end(), cur)) {
        output << "| ";
        output << std::setw(widths[cur]) << std::left;
        if (data.find(cur) == data.end()) {
          output << "NULL";
        }
        else if (data[cur].index()==0){
          bool* val = std::get_if<bool>(&data[cur]);
          if (*val==true)
            output << "true";
          else
            output << "false";
        }
        else{
           output << data[cur];
        }
      }
    }
    output <<  "|" << std::endl;
    return *this;
  }

  bool TabularView::show(std::shared_ptr<DBQuery>& aQuery, RowCollection &aRows){
    const size_t kRowWidth = 8;
    widths = getAttMaxSize(aRows, kRowWidth);
    StringList selectList = getSelectList(aQuery);
    for (auto& cur : selectList) {
      widths[cur] = std::max(cur.size() + 1, widths[cur]);
    }
    showSeparator(aQuery);
    showHeader(aQuery);
    showSeparator(aQuery);
    OrderedRow orderedRows;
    reorderRow(aQuery, aRows, orderedRows);
    uint32_t RowCount = 0;
    uint32_t theLimit = aQuery->getLimit();
    for(auto &field : orderedRows) {
      if (RowCount>=theLimit) break;
      for (auto& theRow : field.second){
        if (RowCount>=theLimit) break;
        showRow(aQuery, theRow);
        RowCount++;
      }
    }
    showSeparator(aQuery);
    return true;
  }
}