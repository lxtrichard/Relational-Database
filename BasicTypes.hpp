//
//  BasicTypes.hpp
//  RGAssignement1
//
//  Created by rick gessner on 3/9/21.
//

#ifndef BasicTypes_h
#define BasicTypes_h

#include <string>
#include <variant>
#include <map>
#include<optional>
#include <vector>
#include <iomanip>


namespace ECE141 {

  enum class DataTypes {
     no_type='N',  bool_type='B', datetime_type='D',
      float_type='F', int_type='I', varchar_type='V',
  };

  using StringList = std::vector<std::string>;
  using StringMap = std::map<std::string, std::string>;
  using StringOpt = std::optional<std::string>;
  using IntOpt    = std::optional<uint32_t>;
  using Value = std::variant<bool, int, double, std::string>;
  using KeyValues = std::map<const std::string, Value>;
  using KeyIndexes = std::map<const std::string, uint32_t>;
  using RowIndexes = std::map<const std::string, std::vector<uint32_t>>;
  using IndexKey = std::variant<uint32_t, std::string>;
  using IndexPairs = std::vector<std::pair<std::string, std::string>>;
}
#endif /* BasicTypes_h */
