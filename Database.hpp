//
//  Database.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <fstream> 
#include "Storage.hpp"
#include "Entity.hpp"
#include "Helpers.hpp"

namespace ECE141 {

  class Database : public Storable{
  public:
    
            Database(const std::string aPath, CreateDB);
            Database(const std::string aPath, OpenDB);
            ~Database();

    StatusResult    dump(std::ostream &anOutput);
    std::string     getName() {return name;}
    Entity*         getEntity(const std::string &aName);

    StatusResult    createTable(std::ostream &anOutput, Entity &anEntity);
    StatusResult    showTables(std::ostream &anOutput);
    StatusResult    describeTable(std::ostream &anOutput, const std::string &aName);
    StatusResult    dropTable(std::ostream &anOutput, const std::string &aName);

    StatusResult    encode(std::ostream &anOutput) override;
    StatusResult    decode(std::istream &anInput) override;

  protected:
    
    std::string     name;
    std::fstream    stream;   //low level stream used by storage...
    Storage         theStorage;
    bool            changed;  //might be helpful, or ignore if you prefer.
    std::vector<Entity> theEntityList;
    KeyIndexes      theTableIndexes;
  };

}
#endif /* Database_hpp */
