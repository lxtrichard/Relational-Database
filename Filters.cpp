//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include "Filters.hpp"
#include <string>
#include <limits>
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Entity.hpp"
#include "Attribute.hpp"
#include "Compare.hpp"

namespace ECE141 {
  
  using Comparitor = bool (*)(Value &aLHS, Value &aRHS);

  bool equals(Value &aLHS, Value &aRHS) {
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isEqual(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool notEqual(Value &aLHS, Value &aRHS){
    return !equals(aLHS,aRHS);
  }

  bool lessThan(Value &aLHS, Value &aRHS){
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isLessThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool greaterThan(Value &aLHS, Value &aRHS){
    bool theResult=false;
    
    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isGreaterThan(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool lessOrEqual(Value &aLHS, Value &aRHS){
    return !greaterThan(aLHS,aRHS);
  }

  bool greaterOrEqual(Value &aLHS, Value &aRHS){
    return !lessThan(aLHS,aRHS);
  }

  static std::map<Operators, Comparitor> comparitors {
    {Operators::equal_op, equals},
    {Operators::notequal_op, notEqual},
    {Operators::lt_op, lessThan},
    {Operators::gt_op, greaterThan},
    {Operators::lte_op, lessOrEqual},
    {Operators::gte_op, greaterOrEqual}
  };

  bool Expression::operator()(KeyValues &aList) {
    Value theLHS{lhs.value};
    Value theRHS{rhs.value};

    if(TokenType::identifier==lhs.ttype) {
      theLHS=aList[lhs.name]; //get row value
    }

    if(TokenType::identifier==rhs.ttype) {
      theRHS=aList[rhs.name]; //get row value
    }

    return comparitors.count(op)
      ? comparitors[op](theLHS, theRHS) : false;
  }
  
  //--------------------------------------------------------------
  
  Filters::Filters()  {}
  
  Filters::Filters(const Filters &aCopy)  {
  }
  
  Filters::~Filters() {
    //no need to delete expressions, they're unique_ptrs!
  }

  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(std::unique_ptr<Expression>(anExpression));
    return *this;
  }

  Logical Filters::getLogic(Keywords aLogic) {
    switch (aLogic){
      case Keywords::and_kw:  return Logical::and_op;
      case Keywords::or_kw:   return Logical::or_op;
      default:                return Logical::no_op;
    }
  }
    
  //compare expressions to row; return true if matches
  bool Filters::matches(KeyValues &aList) const {
    
    //STUDENT: You'll need to add code here to deal with
    //         logical combinations (AND, OR, NOT):
    //         like:  WHERE zipcode=92127 AND age>20
    //          or:   WHERE zipcode=92127 AND NOT age>20 
    bool preResult=false;
    bool theResult=true;
    Logical preLogic=Logical::no_op;
    for(auto &theExpr : expressions) {
      theResult = (*theExpr)(aList);

      // if not
      bool theNot = theExpr->isNot;
      if (theNot){
        theResult = !theResult;
      }

      // combine results
      if (preLogic!=Logical::no_op) {
        if (preLogic==Logical::and_op) {
          theResult=preResult && theResult;
        } 
        else {
          theResult=preResult || theResult;
        }
      }

      // update logic
      preLogic = theExpr->logic;

      // update result
      preResult=theResult;
    }
    return theResult;
  }
 

  //where operand is field, number, string...
  StatusResult parseOperand(Tokenizer &aTokenizer,
                            Entity &anEntity, Operand &anOperand) {
    StatusResult theResult{noError};
    Token &theToken = aTokenizer.current();
    if(TokenType::identifier==theToken.type) {
      if(auto *theAttr=anEntity.getAttribute(theToken.data)) {
        anOperand.ttype=theToken.type;
        anOperand.name=theToken.data; //hang on to name...
        anOperand.entityId= Helpers::hashString(theToken.data.c_str());
        anOperand.dtype=theAttr->getType();
      }
      else {
        anOperand.ttype=TokenType::string;
        anOperand.dtype=DataTypes::varchar_type;
        anOperand.value=theToken.data;
      }
    }
    else if(TokenType::number==theToken.type) {
      anOperand.ttype=TokenType::number;
      anOperand.dtype=DataTypes::int_type;
      if (theToken.data.find('.')!=std::string::npos) {
        anOperand.dtype=DataTypes::float_type;
        anOperand.value=std::stof(theToken.data);
      }
      else anOperand.value=std::stoi(theToken.data);
    }
    else theResult.error=syntaxError;
    if(theResult) aTokenizer.next();
    return theResult;
  }
    
  //STUDENT: Add validation here...
  bool validateOperands(Operand &aLHS, Operand &aRHS, Entity &anEntity) {
    if(TokenType::identifier==aLHS.ttype) { //most common case...
      //STUDENT: Add code for validation as necessary
      if (aLHS.dtype != aRHS.dtype) 
        return false;
      return true;
    }
    else if(TokenType::identifier==aRHS.ttype) {
      //STUDENT: Add code for validation as necessary
      if (aLHS.dtype != aRHS.dtype) 
        return false;
      return true;
    }
    return false;
  }

  //STUDENT: This starting point code may need adaptation...
  StatusResult Filters::parse(Tokenizer &aTokenizer,Entity &anEntity) {
    StatusResult  theResult{noError};
    bool theNot;
    while(theResult && (2<aTokenizer.remaining())) {
      Operand theLHS,theRHS;
      Token &theToken=aTokenizer.current();

      // parse not operator...
      theNot = false;
      if(theToken.keyword==Keywords::not_kw) {
        theNot=true;
        aTokenizer.next();
        theToken = aTokenizer.current();
      }

      // parse operand and operator...
      if(theToken.type!=TokenType::identifier) return theResult;
      if((theResult=parseOperand(aTokenizer,anEntity,theLHS))) {
        Token &theToken=aTokenizer.current();
        if(theToken.type==TokenType::operators) {
          std::string anOP = "";
          while (Helpers::isOperator(theToken.data)){
            anOP += theToken.data;
            aTokenizer.next();
            theToken = aTokenizer.current();
          }
          Operators theOp=Helpers::toOperator(anOP);
          if((theResult=parseOperand(aTokenizer,anEntity,theRHS))) {
            if(validateOperands(theLHS, theRHS, anEntity)) {
              add(new Expression(theLHS, theOp, theRHS));
              if (theNot) expressions.back()->isNot=true; //add NOT operator
              if (aTokenizer.current().keyword == Keywords::and_kw 
                  || aTokenizer.current().keyword == Keywords::or_kw) 
              {
                Logical theLogic = getLogic(aTokenizer.current().keyword);
                expressions.back()->logic = theLogic;
                aTokenizer.next();
              } // add logic
              if(aTokenizer.skipIf(semicolon)) {
                break;
              }
            }
            else theResult.error=syntaxError;
          }
        }
      }
      else theResult.error=syntaxError;
    }
    return theResult;
  }

}

