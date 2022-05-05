//
//  View.hpp
//  RGAssignment2
//
//  Created by rick gessner on 2/27/21.
//

#ifndef View_h
#define View_h


#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include "BasicTypes.hpp"
#include "BlockIO.hpp"
#include "Entity.hpp"
#include "Config.hpp"
#include "FolderReader.hpp"
#include "Timer.hpp"
#include "Helpers.hpp"


namespace ECE141 {

  //completely generic view, which you will subclass to show information
  class View {
  public:
    View(std::ostream& anOutput) : output(anOutput) {}
    
    ~View() {}

  protected:
    std::ostream& output;
  };
  
  class DBView : public View {
  public:
    DBView(std::ostream& anOutput, std::string aPath = Config::getStoragePath(), std::string anExtension = Config::getDBExtension())
        : View(anOutput), path(aPath), extension(anExtension) {}

    ~DBView() {}

    //show all databases
    void showDBs(){
      Timer theTimer;
      output << std::setprecision(3) << std::fixed;
      output << "+--------------------+" << std::endl;
      output << "| Database           |" << std::endl;
      output << "+--------------------+" << std::endl;
      int theCount = 0;
      FolderReader theReader(path.c_str());
      theReader.each(".db",[&](const std::string &aName) {
      output << "| " << std::left << std::setw(19) << aName << "|" << std::endl;
      theCount++;
      return true;
      });
      output << "+--------------------+" << std::endl;
      output << theCount << " rows in set (" << theTimer.elapsed() << " secs)" << std::endl;
    };

    void dumpDB(const std::string &aName){
      Timer theTimer;
      output << std::setprecision(3) << std::fixed;
      output << "+----------------+--------+" << std::endl;
      output << "| Type           | Id     |" << std::endl;
      output << "+----------------+--------+" << std::endl;
      std::string thePath = Config::getDBPath(aName);
      std::fstream stream;
      BlockIO theBlockIO(stream);
      stream.open(thePath, std::fstream::binary | std::fstream::in | std::fstream::out);
      uint32_t theBlockNum = theBlockIO.getBlockCount();
      for (size_t i = 0; i < theBlockNum; i++) {
        Block theBlock;
        theBlockIO.readBlock(i, theBlock);
        // convert blocktype into string
        BlockType theType = static_cast<BlockType>(theBlock.header.type);
        std::string theTypeName;
        switch(theType) {
          case BlockType::data_block:     theTypeName = "Data"; break;
          case BlockType::free_block:     theTypeName = "Free"; break;
          case BlockType::unknown_block:  theTypeName = "Unknown"; break;
          case BlockType::meta_block:     theTypeName = "Meta"; break;
          case BlockType::entity_block:     theTypeName = "Entity"; break;
        }
        output << "| " << std::setw(15) << std::left << theTypeName << "| " 
            << std::setw(7) << std::left << theBlock.header.id << "|" << std::endl;
        output << "+----------------+--------+" << std::endl;
      }
      output << theBlockNum <<" rows in set (" << theTimer.elapsed() << " sec) " << std::endl;
    }

  private:
    std::string path;
    std::string extension;
  };

  class TableView : public View{
  public:
    TableView(std::ostream& anOutput) : View(anOutput) {}
    ~TableView() {}

    void showTables(std::string aName, KeyIndexes theTableIndexes){
      Timer theTimer;
      output << std::setprecision(3) << std::fixed;
      output << "+----------------------+" << std::endl;
      output << "| Tables_in_" << std::setw(10) << std::left << aName << " |" << std::endl;
      output << "+----------------------+" << std::endl;
      for (auto& cur : theTableIndexes) {
        output << "| " << std::setw(20) << std::left << cur.first <<  " |" << std::endl;
      }
      output << "+----------------------+" << std::endl;
      output << theTableIndexes.size() <<" rows in set (" <<  theTimer.elapsed() << " sec)" << std::endl;
    };

    void describeTables(Entity* anEntity){
      Timer theTimer;
      output << std::setprecision(3) << std::fixed;
      output << "+----------------+--------------+------+-----+---------+-----------------------------+" << std::endl;
      output << "| Field          | Type         | Null | Key | Default | Extra                       |" << std::endl;
      output << "+----------------+--------------+------+-----+---------+-----------------------------+" << std::endl;
      Helpers aHelper;
      for (auto attribute : anEntity->getAttributes()) {
        std::string theType = aHelper.dataTypeToString(attribute.getType());
        if (attribute.getType() == DataTypes::varchar_type) {
          theType.append("(");
          theType.append(std::to_string(attribute.getSize()));
          theType.append(")");
        }

        output << "| " << std::setw(15) << std::left << attribute.getName() << "| " 
                  << std::setw(13) << std::left << theType << "| ";
        if (attribute.isNullable()) {
          output << std::setw(5) << std::left << "YES" << "| ";
        } 
        else {
          output << std::setw(5) << std::left << "NO" << "| ";
        }

        if (attribute.isPrimaryKey()) {
          output << std::setw(4) << std::left << "YES" << "| ";
        } 
        else {
          output << std::setw(4) << std::left << "" << "| ";
        }

        output << std::setw(8) << std::left;
        if (attribute.hasDefault()) {
          output << attribute.DValuetoString() << "| ";
        } 
        else {
          output << "NULL" << "| ";
        }

        output << std::setw(28) << std::left << "" << "|" ;
        output << std::endl;
      }
      output << "+----------------+--------------+------+-----+---------+-----------------------------+" << std::endl;
      output << anEntity->getAttributes().size() << " rows in set ("<<  theTimer.elapsed() << " sec)" << std::endl;
    }
  };
}

#endif /* View_h */
