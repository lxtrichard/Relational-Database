//
//  TestAutomatic.hpp
//  RGAssignement1
//
//  Created by rick gessner on 2/26/21.
//

#ifndef TestAutomatic_h
#define TestAutomatic_h

#include <iostream>
#include <sstream>
#include <vector>
#include "Application.hpp"
#include "AboutUs.hpp"
#include "Errors.hpp"
#include "Config.hpp"
#include "FolderReader.hpp"

void showErrors(ECE141::StatusResult &aResult, std::ostream &anOutput) {
  
  static std::map<ECE141::Errors, std::string> theMessages = {
    {ECE141::illegalIdentifier, "Illegal identifier"},
    {ECE141::unknownIdentifier, "Unknown identifier"},
    {ECE141::databaseExists, "Database exists"},
    {ECE141::tableExists, "Table Exists"},
    {ECE141::syntaxError, "Syntax Error"},
    {ECE141::unknownCommand, "Unknown command"},
    {ECE141::unknownDatabase,"Unknown database"},
    {ECE141::unknownTable,   "Unknown table"},
    {ECE141::unknownError,   "Unknown error"}
  };

  std::string theMessage="Unknown Error";
  if(theMessages.count(aResult.error)) {
    theMessage=theMessages[aResult.error];
  }
  anOutput << "Error (" << aResult.error << ") " << theMessage << "\n";
}

namespace ECE141 {
  
  using StringMap = std::map<std::string, std::string>;
  using CountList = std::vector<int>;

  void showErrors(ECE141::StatusResult &aResult, std::ostream &anOutput) {
    
    static std::map<ECE141::Errors, std::string> theMessages = {
      {ECE141::illegalIdentifier, "Illegal identifier"},
      {ECE141::unknownIdentifier, "Unknown identifier"},
      {ECE141::databaseExists, "Database exists"},
      {ECE141::tableExists, "Table Exists"},
      {ECE141::syntaxError, "Syntax Error"},
      {ECE141::unknownCommand, "Unknown command"},
      {ECE141::unknownDatabase,"Unknown database"},
      {ECE141::unknownTable,   "Unknown table"},
      {ECE141::unknownError,   "Unknown error"}
    };

    std::string theMessage="Unknown Error";
    if(theMessages.count(aResult.error)) {
      theMessage=theMessages[aResult.error];
    }
    anOutput << "Error (" << aResult.error << ") "
      << theMessage << "\n";
  }

  class TestAutomatic {
  protected:
    std::ostream &output;
    
  public:
    
    TestAutomatic(std::ostream &anOutput) : output(anOutput) {}
    
    ~TestAutomatic() {std::cout << "Test Version 1.2\n";}

    
    void addUsersTable(std::ostream &anOutput) {
      anOutput << "create table Users (";
      anOutput << " id int NOT NULL auto_increment primary key,";
      anOutput << " first_name varchar(50) NOT NULL,";
      anOutput << " last_name varchar(50),";
      anOutput << " zipcode int);\n";
    }

    void addPaymentsTable(std::ostream &anOutput) {
      anOutput << "create table Payments (";
      anOutput << " id int NOT NULL auto_increment primary key,";
      anOutput << " user_id int,";
      anOutput << " paydate int,";
      anOutput << " amount int);\n";
    }

    void addAccountsTable(std::ostream &anOutput) {
      anOutput << "create table Accounts (";
      anOutput << " id int NOT NULL auto_increment primary key,";
      anOutput << " account_type varchar(25) NOT NULL,";
      anOutput << " amount int);\n";
    }
    
    void addBooksTable(std::ostream &anOutput) {
      anOutput << "create table Books (";
      anOutput << " id int NOT NULL auto_increment primary key,";
      anOutput << " title varchar(25) NOT NULL,";
      anOutput << " subtitle varchar(25),";
      anOutput << " user_id int);\n";
    }
    
    bool doCompileTest() {
      AboutUs theAbout;
      auto theCount=theAbout.getTeamSize();
      std::vector<std::string> theNames;
      for(size_t i=0;i<theCount;i++) {
        if(auto theName=theAbout.getName(i))
          theNames.push_back(*theName);
      }
      return theNames.size()>0;
    }

    bool doScriptTest(std::istream &anInput, std::ostream &anOutput) {
      ECE141::Application   theApp(anOutput);
      ECE141::StatusResult  theResult{};
      std::string theCommand;
      
      while(theResult && anInput) {
        std::getline (anInput, theCommand);
        std::stringstream theStream(theCommand);
        anOutput << theCommand << "\n";
        theResult=theApp.handleInput(theStream);
        if(theResult==ECE141::userTerminated) {
          theResult.error=Errors::noError;
          break;
        }
        else if(!theResult) {
          showErrors(theResult, anOutput);
        }
      }
      return theResult;
    }
    
    //----------------------------------------------

    bool doAppTest() {

      std::string theInput("version; help; quit;");
      std::stringstream theStream(theInput);
      std::stringstream theOutput;
      bool theResult=doScriptTest(theStream,theOutput);
      std::string temp=theOutput.str();
      output << temp << "\n";
      
      if(theResult) {
        const char* theLines[]={
          theInput.c_str(),
          "Version 0.3", "Help system available",
          "DB::141 is shutting down"
        };
        
        std::stringstream theStream(temp);
        std::string theLine;
          
        for(auto *theNext : theLines) {
          std::getline(theStream,theLine);
          if(theLine!=theNext) return false;
        }
        
        return true;
      }
      
      return false;
    }
        
    //---------------------------------------------------
    

    enum states {wasUnknown, wasAlter, wasCreate, wasDescribe,
        wasDelete, wasDrop, wasDump, wasInsert, wasSelect,
        wasShow, wasUpdate, wasUse};
    
    //convert from string (word) to states enum...
    states getTerm(const std::string &aWord) {
      static std::map<std::string, states> gTerms = {
        {"alter",states::wasAlter},
        {"create",states::wasCreate},
        {"delete",states::wasDelete},
        {"describe",states::wasDescribe},
        {"drop",states::wasDrop},
        {"dump",states::wasDump},
        {"insert",states::wasInsert},
        {"select",states::wasSelect},
        {"show",states::wasShow},
        {"update",states::wasUpdate},
        {"use",states::wasUse},
      };
      return gTerms.count(aWord) ? gTerms[aWord]
                                 : states::wasUnknown;
    }
    
    //extract count (numeric) from line
    int getQueryCount(const std::string &aLine) {
      std::stringstream temp(aLine);
      std::string theWord;
      int theNumber;
      temp >> theWord >> theWord >> theNumber;
      return theNumber;
    }
    
    //extract first word (l/c) from line...
    std::string getFirst(const std::string &aLine) {
      std::stringstream temp(aLine);
      std::string theFirst;
      temp >> theFirst;
      std::transform(theFirst.begin(),
                     theFirst.end(), theFirst.begin(),
          [](unsigned char c){ return std::tolower(c); });
      return theFirst;
    }
    
    //validates output of assignment2
    bool hwIsValid(std::istream &aStream, CountList &aCounts) {
      bool theResult=true;
      states theState=states::wasUnknown;
      
      std::string theLine;
      while(theResult && aStream) {
        std::getline(aStream, theLine);
        if(theLine.size()>2) {
          //std::cout << theLine << "\n";
          std::string theWord=getFirst(theLine);
          switch(theState) {
            case wasUnknown:
              theState=getTerm(theWord); break;
            case wasUse:
              //if(theWord=="database") {theState=wasUnknown;}
              theState=wasUnknown;
              break;
            case wasAlter:
            case wasCreate:
            case wasDelete:
            case wasInsert:
            case wasUpdate:
            case wasDrop:
              if(theWord=="query") {
                aCounts.push_back(getQueryCount(theLine));
                theState=wasUnknown;
              }
              else return false;
              break;
            case wasDescribe:
            case wasDump:
            case wasSelect:
            case wasShow:
              if(std::isdigit(theWord[0])) {
                aCounts.push_back(stoi(theWord));
                theState=wasUnknown;
              }
              else if(wasUnknown!=getTerm(theWord)) {
                return false; //unexpected kw...
              }
              break;
            default: break;
          }
        }
      }
      return theResult;
    }
    using FileList = std::vector<std::string>;
    
    bool hasFiles(FileList &aFilelist) {
      for(auto theFile : aFilelist) {
        std::string thePath=Config::getDBPath(theFile);
        std::ifstream theStream(thePath);
        if(!theStream) return false;
      }
      return true;
    }
    
    std::string getRandomDBName(char aChar) {
      uint32_t theCount=rand() % 99999;
      return std::string("testdb_"+std::to_string(theCount+1000)+aChar);
    }
    
    bool compareCounts(CountList &aList1, CountList &aList2, const size_t aPos) {
      bool theResult=true;
      for(size_t i=0;i<aList1.size();i++) {
        if(theResult) {
          theResult=i==aPos ? aList1[i]<aList2[i] : aList1[i]==aList2[i];
        }
      }
      return theResult;
    }
    
    size_t countDBFiles() {
      std::string thePath=Config::getStoragePath();
      FolderReader theReader(thePath.c_str());
      size_t theCount{0};
      theReader.each(Config::getDBExtension(),
                     [&](const std::string &aName) {
        theCount++;
        return true;
      });
      return theCount;
    }
    
  //----------------------------------------------
    bool createFile(const std::string &aPath, const std::string &aName) {
      std::string temp(aPath);
      temp+='/';
      temp+=aName;
      std::fstream output(temp, std::fstream::out | std::fstream::trunc);
      output << "some text\n";
      return true;
    }
    
    //----------------------------------------------

    bool doReaderTest() {
      std::string thePath=Config::getStoragePath();
      createFile(thePath,"test1.txt");
      
      FolderReader theReader(thePath.c_str());
      size_t theCount{0};
      theReader.each(".txt",[&](const std::string &aName) {
        theCount++;
        return true;
      });
      return theCount>0;
    }
    
    //----------------------------------------------

    bool doDBCommandsTest() {
      
      std::vector<std::string> theFiles;
      
      size_t theDBCount=countDBFiles();
      
      char theType='A';
      theFiles.push_back(getRandomDBName(theType));
      theFiles.push_back(getRandomDBName(theType));
      theFiles.push_back(getRandomDBName(theType));

      std::stringstream theStream1;
      theStream1 << "CREATE DATABASE "+theFiles[0] << ";\n";
      theStream1 << "create database "+theFiles[1] << ";\n";
      theStream1 << "CrEaTe dAtABaSe "+theFiles[2] << ";\n";
      theStream1 << "use "+theFiles[0] << ";\n";
      theStream1 << "shoW databaseS;\n";
   
      std::string temp(theStream1.str());
      std::stringstream theInput(temp);
      std::stringstream theOutput1;
      bool theResult=doScriptTest(theStream1,theOutput1) && hasFiles(theFiles);
      if(theResult) {
        auto temp=theOutput1.str();
        output << temp; //show user...
        
        CountList theCounts;
        if((theResult=hwIsValid(theOutput1,theCounts))) {
          CountList theOpts{1,1,1, static_cast<int>(3+theDBCount)};
          if(!compareCounts(theCounts,theOpts,100)) {
            return false; //something is wrong...
          }
        }

        std::stringstream theStream2;
        theStream2 << "DRop dataBASE "+theFiles[1] << ";\n";
        theStream2 << "shoW databaseS;\n";
        theStream2 << "dump database "+theFiles[0] << ";\n";
        theStream2 << "drop database "+theFiles[0] << ";\n";
        theStream2 << "drop database "+theFiles[2] << ";\n";

        std::stringstream theOutput2;
        if((theResult=doScriptTest(theStream2,theOutput2))) {
          temp=theOutput2.str();
          //std::cout << temp << "\n"; //DEBUG!
          
          output << temp; //show user...
          std::stringstream theOutput(temp);
          size_t thePrevDBCount=theCounts[3];
          theCounts.clear();
          if((theResult=hwIsValid(theOutput,theCounts))) {
            CountList theOpts2{0,3,0,0,0};
            theOpts2[1]=static_cast<int>(thePrevDBCount-1);
            //theOpts[5]=theOpts[3]-1;
            theResult=compareCounts(theCounts,theOpts2,100);
          }
          
        }
      }
      return theResult;
    }
    
    //----------------------------------------------

    bool doTablesTest() {
      
      std::string theDBName(getRandomDBName('B'));
  
      std::stringstream theStream1;
      theStream1 << "create database " << theDBName << ";\n";
      theStream1 << "use " << theDBName << ";\n";
      
      addUsersTable(theStream1);
      addAccountsTable(theStream1);
      addPaymentsTable(theStream1);
   
      theStream1 << "show tables;\n";
      theStream1 << "describe Accounts;\n";
      theStream1 << "drop table Accounts;\n";
      theStream1 << "show tables;\n";
      theStream1 << "drop database " << theDBName << ";\n";
      theStream1 << "quit;\n";

      std::string temp(theStream1.str());
      std::stringstream theInput(temp);
      std::stringstream theOutput1;
      bool theResult=doScriptTest(theInput,theOutput1);
      if(theResult) {
        std::string tempStr=theOutput1.str();
        std::stringstream theOutput2(tempStr);
        //std::cout << tempStr << "\n";
        
        CountList theCounts;
        if((theResult=hwIsValid(theOutput2,theCounts))) {
          static CountList theOpts{1,1,1,1,3,3,1,2,0};
          theResult=compareCounts(theCounts,theOpts,20);
          output << tempStr << "\n";
        }
      }
      return theResult;
    }

  };

}


#endif /* TestAutomatic_h */
