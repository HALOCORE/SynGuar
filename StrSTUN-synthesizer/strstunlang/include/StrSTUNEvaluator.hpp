#ifndef STREC_EVALUATOR
#define STREC_EVALUATOR

#include "Parser.ih"
#include "Scanner.ih"
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>

enum EvalueType {
    ET_INT,
    ET_BOOL,
    ET_STR,
    ET_UNKNOWN,
};

struct Evalue {
public:
    inline Evalue() {this->type = EvalueType::ET_UNKNOWN; this->isError = false;};
    inline void setBool(bool value, bool isErr = false) {this->type = EvalueType::ET_BOOL; this->valBool = value; this->isError = isErr;}; // std::cerr<< "Evalue set bool called:" << value << std::endl;
    inline void setInt(int value, bool isErr = false) {this->type = EvalueType::ET_INT; this->valInt = value; this->isError = isErr;}; // std::cerr<< "Evalue set int called:" << value << std::endl;
    inline void setStr(std::string value, bool isErr = false) {this->type = EvalueType::ET_STR; this->valStr = value; this->isError = isErr;}; //std::cerr << "Evalue set string called:" << value << std::endl;
    EvalueType type;
    bool isError;
    int valInt;
    bool valBool;
    std::string valStr;
    
    inline bool isBool() {return this->type == EvalueType::ET_BOOL;}
    inline bool isInt() {return this->type == EvalueType::ET_INT;}
    inline bool isStr() {return this->type == EvalueType::ET_STR;}
    inline bool isUnknown() {return this->type == EvalueType::ET_UNKNOWN;}
    inline Evalue toType(EvalueType type) {
        assert(!this->isError);
        Evalue val;
        val.type = this->type;
        val.valBool = this->valBool;
        val.valInt = this->valInt;
        val.valStr = this->valStr;
        if (val.type == type) return val;
        assert(val.type == EvalueType::ET_UNKNOWN);
        if (type == EvalueType::ET_BOOL) {
            val.type = EvalueType::ET_BOOL;
            if(val.valStr == "true" || val.valStr == "1") val.valBool = true;
            else if (val.valStr == "false" || val.valStr == "0") val.valBool = false;
            else assert(false);
        } else if (type == EvalueType::ET_INT) {
            val.type = EvalueType::ET_INT;
            val.valInt = std::stoi(val.valStr);
        } else if (type == EvalueType::ET_STR) {
            val.type = EvalueType::ET_STR;
        } else assert(false);
        return val;
    }
    inline Evalue toTypeByTypeString(std::string typeString) {
        assert(!this->isError);
        if (typeString == "Bool") {return this->toType(EvalueType::ET_BOOL);}
        else if (typeString == "Int") {return this->toType(EvalueType::ET_INT);}
        else if (typeString == "Str") {return this->toType(EvalueType::ET_STR);}
        else assert(false);
    }
    inline void print(std::ostream &out, bool withType = true) {
        if (this->type == EvalueType::ET_BOOL) out << (withType ? "Bool " : "") << (this->isError ? "<ERROR>" : (this->valBool ? "true" : "false"));
        else if (this->type == EvalueType::ET_INT) out << (withType ? "Int " : "") << (this->isError ? "<ERROR>" : std::to_string(this->valInt));
        else if (this->type == EvalueType::ET_STR) out << (withType ? "Str " : "") << (this->isError ? "<ERROR>" : this->valStr);
        else if (this->type == EvalueType::ET_UNKNOWN) out << (withType ? "Unknown " : "") << (this->isError ? "<ERROR>" : this->valStr);
        else assert(false);
    }
};

class StrSTUNEvaluator {
public:
    StrSTUNEvaluator(bool isVerbose = false);
    void addFunc(ASTFunction* funcAst);
    bool containsFunc(std::string funcName);
    std::vector<std::string> getFuncSignature(std::string funcName);
    Evalue evaluateFunc(std::string funcName, std::vector<Evalue> &realParams);
private:
    std::unordered_map<std::string, Evalue (*) (std::vector<Evalue> &realParams)> builtinFuncMap;
    std::unordered_map<std::string, ASTFunction*> funcMap;
    bool isVerbose;
    
    Evalue evaluateBuiltinFunc(std::string builtinFuncName, std::vector<Evalue> &realParams);
    Evalue evaluateCodeFunc(ASTFunction* func, std::vector<Evalue> &realParams);
    Evalue evaluateExpr(ASTExpression* expr, std::unordered_map<std::string, Evalue> &scalarMap);
};

#endif