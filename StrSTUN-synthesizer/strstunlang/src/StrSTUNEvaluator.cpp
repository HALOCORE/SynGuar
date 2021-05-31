#include "StrSTUNEvaluator.hpp"
#include <sstream>

///////////// copied from ComponentBuiltin BEGIN
bool strec_eval_isPrefix(std::string const& prefix, std::string const& fullstr)
{
    if (fullstr.length() < prefix.length()) return false;
    auto res = std::mismatch(prefix.begin(), prefix.end(), fullstr.begin());
    if (res.first == prefix.end()) return true;
    return false;
}

inline bool strec_eval_ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void strec_eval_findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}
///////////// copied from ComponentBuiltin END


//private "static" builtin functions
Evalue builtinAnd(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue boolVal1 = realParams[0].toType(EvalueType::ET_BOOL);
    Evalue boolVal2 = realParams[1].toType(EvalueType::ET_BOOL);
    Evalue boolVal; boolVal.setBool(boolVal1.valBool && boolVal2.valBool);
    return boolVal;
}

Evalue builtinOr(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue boolVal1 = realParams[0].toType(EvalueType::ET_BOOL);
    Evalue boolVal2 = realParams[1].toType(EvalueType::ET_BOOL);
    Evalue boolVal; boolVal.setBool(boolVal1.valBool || boolVal2.valBool);
    return boolVal;
}

Evalue builtinNot(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue boolVal1 = realParams[0].toType(EvalueType::ET_BOOL);
    Evalue boolVal; boolVal.setBool(!boolVal1.valBool);
    return boolVal;
}

Evalue builtinIEqual(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue intVal1 = realParams[0].toType(EvalueType::ET_INT);
    Evalue intVal2 = realParams[1].toType(EvalueType::ET_INT);
    Evalue boolVal; boolVal.setBool(intVal1.valInt == intVal2.valInt);
    return boolVal;
}

Evalue builtinSEqual(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    Evalue boolVal; boolVal.setBool(strVal1.valStr == strVal2.valStr);
    return boolVal;
}

Evalue builtinILessEqual0(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue intVal1 = realParams[0].toType(EvalueType::ET_INT);
    Evalue boolVal; boolVal.setBool(intVal1.valInt <= 0);
    return boolVal;
}

Evalue builtinIsEmptyStr(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue strVal = realParams[0].toType(EvalueType::ET_STR);
    Evalue boolVal; boolVal.setBool(strVal.valStr.size() == 0);
    return boolVal;
}

Evalue builtinPlus(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue intVal1 = realParams[0].toType(EvalueType::ET_INT);
    Evalue intVal2 = realParams[1].toType(EvalueType::ET_INT);
    Evalue intVal; intVal.setInt(intVal1.valInt + intVal2.valInt);
    return intVal;
}

Evalue builtinMinus(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue intVal1 = realParams[0].toType(EvalueType::ET_INT);
    Evalue intVal2 = realParams[1].toType(EvalueType::ET_INT);
    Evalue intVal; intVal.setInt(intVal1.valInt - intVal2.valInt);
    return intVal;
}

Evalue builtinInc(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue intVal = realParams[0].toType(EvalueType::ET_INT);
    intVal.valInt++;
    return intVal;
}

Evalue builtinDec(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue intVal = realParams[0].toType(EvalueType::ET_INT);
    intVal.valInt--;
    return intVal;
}

Evalue builtinTail(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue strVal = realParams[0].toType(EvalueType::ET_STR);
    if(strVal.valStr.size() > 0) {
        strVal.valStr = strVal.valStr.substr(1);
        return strVal;
    } else {
        strVal.isError = true;
        return strVal;
    }
}

Evalue builtinHead(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue strVal = realParams[0].toType(EvalueType::ET_STR);
    if(strVal.valStr.size() > 0) {
        strVal.valStr = strVal.valStr.substr(0, 1);
        return strVal;
    } else {
        strVal.isError = true;
        return strVal;
    }
    
}

Evalue builtinCat(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    Evalue strVal; strVal.setStr(strVal1.valStr + strVal2.valStr);
    return strVal;
}

/////////// new SyGuS components BEGIN
Evalue builtinPrefixOf(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    bool isprefix = strec_eval_isPrefix(strVal1.valStr, strVal2.valStr);
    Evalue bVal; bVal.setBool(isprefix);
    return bVal;
}

Evalue builtinSuffixOf(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    bool isSuffix = strec_eval_ends_with(strVal2.valStr, strVal1.valStr);
    Evalue bVal; bVal.setBool(isSuffix);
    return bVal;
}

Evalue builtinContains(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    bool isContained = strVal1.valStr.find(strVal2.valStr) != std::string::npos;
    Evalue bVal; bVal.setBool(isContained);
    return bVal;
}

Evalue builtinStrlen(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    int ival = strVal1.valStr.length();
    Evalue iVal; iVal.setInt(ival);
    return iVal;
}

Evalue builtinStrToInt(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    int ival = 0;
    Evalue iVal;
    try {
        ival = std::stoi(strVal1.valStr);
        iVal.setInt(ival);
    }
    catch(std::invalid_argument& e){
        iVal.isError = true;
    }
    catch(std::out_of_range& e){
        iVal.isError = true;
    } 
    return iVal;
}

Evalue builtinStrIndexOf(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 3);
    Evalue baseStrVal = realParams[0].toType(EvalueType::ET_STR);
    Evalue subStrVal = realParams[1].toType(EvalueType::ET_STR);
    Evalue idxVal = realParams[2].toType(EvalueType::ET_INT);
    Evalue resultVal;

    if(subStrVal.valStr.length() == 0) {
        resultVal.setInt(-1);
    } else {
        int current_find_idx = 0;
        std::size_t start_pos = 0;
        std::size_t new_pos = -1;
        std::vector<std::size_t> found_poss;
        while(true) {
            new_pos = baseStrVal.valStr.find(subStrVal.valStr, start_pos);
            if(new_pos == std::string::npos) break;
            found_poss.push_back(new_pos);
            start_pos = new_pos + subStrVal.valStr.length();
            if(start_pos >= baseStrVal.valStr.length()) break;
        }
        int subidx = idxVal.valInt;
        if (subidx < 0) subidx = found_poss.size() + subidx;
        if (subidx < 0 || subidx >= found_poss.size()) {
            resultVal.setInt(-1);
        } else {
            resultVal.setInt(found_poss[subidx]);
        }
    }
    return resultVal;
}

Evalue builtinIntToStr(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 1);
    Evalue intVal1 = realParams[0].toType(EvalueType::ET_INT);
    Evalue strVal; strVal.setStr(std::to_string(intVal1.valInt));
    return strVal;
}

Evalue builtinSubstr(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 3);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue intVal2 = realParams[1].toType(EvalueType::ET_INT);
    Evalue intVal3 = realParams[2].toType(EvalueType::ET_INT);
    Evalue resultVal;

    int start = intVal2.valInt;
    int sublen = intVal3.valInt;
    if(start < 0) start = start + strVal1.valStr.size();
    if(sublen < 0) sublen = 1000000;
    if(strVal1.valStr.length() <= start) {
        resultVal.setStr("");
    } else {
        resultVal.setStr(strVal1.valStr.substr(start, sublen));
    }
    return resultVal;
}

Evalue builtinStrAt(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 2);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue intVal2 = realParams[1].toType(EvalueType::ET_INT);
    Evalue resultVal;
    int pos = intVal2.valInt;
    if(pos < 0) pos = pos + strVal1.valStr.length();
    if(pos < 0 || pos >= strVal1.valStr.length()) {
        resultVal.isError = true;
    } else {
        std::string s(1, strVal1.valStr[pos]);
        resultVal.setStr(s);
    }
    return resultVal;
}

Evalue builtinStrReplace(std::vector<Evalue> & realParams) {
    assert(realParams.size() == 3);
    Evalue strVal1 = realParams[0].toType(EvalueType::ET_STR);
    Evalue strVal2 = realParams[1].toType(EvalueType::ET_STR);
    Evalue strVal3 = realParams[2].toType(EvalueType::ET_STR);
    Evalue resultVal;
    if(strVal2.valStr == "") resultVal.isError = true;
    else if(strVal1.valStr == "") resultVal.setStr("");
    else {
        std::string result(strVal1.valStr);
        strec_eval_findAndReplaceAll(result, strVal2.valStr, strVal3.valStr);
        resultVal.setStr(result);
    }
    return resultVal;
}
/////////// new SyGuS components END


// class members

StrSTUNEvaluator::StrSTUNEvaluator (bool isVerbose) {
    this->isVerbose = isVerbose;
    this->builtinFuncMap["and"] = builtinAnd;
    this->builtinFuncMap["or"] = builtinOr;
    this->builtinFuncMap["not"] = builtinNot;
    this->builtinFuncMap["iequal"] = builtinIEqual;
    this->builtinFuncMap["sequal"] = builtinSEqual;
    this->builtinFuncMap["ileq0"] = builtinILessEqual0;
    this->builtinFuncMap["isEmptyStr"] = builtinIsEmptyStr;
    this->builtinFuncMap["plus"] = builtinPlus;
    this->builtinFuncMap["minus"] = builtinMinus;
    this->builtinFuncMap["inc"] = builtinInc;
    this->builtinFuncMap["dec"] = builtinDec;
    this->builtinFuncMap["tail"] = builtinTail;
    this->builtinFuncMap["head"] = builtinHead;
    this->builtinFuncMap["cat"] = builtinCat;
    
    this->builtinFuncMap["=="] = builtinIEqual;
    this->builtinFuncMap["str.prefixof"] = builtinPrefixOf;
    this->builtinFuncMap["str.suffixof"] = builtinSuffixOf;
    this->builtinFuncMap["str.contains"] = builtinContains;
    this->builtinFuncMap["+"] = builtinPlus;
    this->builtinFuncMap["-"] = builtinMinus;
    this->builtinFuncMap["str.len"] = builtinStrlen;
    this->builtinFuncMap["str.to.int"] = builtinStrToInt;
    this->builtinFuncMap["str.indexof"] = builtinStrIndexOf;
    this->builtinFuncMap["int.to.str"] = builtinIntToStr;
    this->builtinFuncMap["str.substr"] = builtinSubstr;
    this->builtinFuncMap["str.at"] = builtinStrAt;
    this->builtinFuncMap["str.++"] = builtinCat;
    this->builtinFuncMap["str.replace"] = builtinStrReplace;
}


void StrSTUNEvaluator::addFunc(ASTFunction* funcAst) {
    std::string funcName = funcAst->funcId->id;
    assert(this->funcMap.find(funcName) == this->funcMap.end());
    this->funcMap[funcName] = funcAst;
    std::cerr << "# num_conds: " << funcAst->expression->num_conds << std::endl;
    std::cerr << "# max_term_size: " << funcAst->expression->max_term_size << std::endl;
    std::cerr << "# total_component_size: " << funcAst->expression->total_component_size << std::endl;
}

bool StrSTUNEvaluator::containsFunc(std::string funcName) {
    if (this->funcMap.find(funcName) != this->funcMap.end()) {
        return true;
    }
    return false;
}

std::vector<std::string> StrSTUNEvaluator::getFuncSignature(std::string funcName) {
    assert(this->funcMap.find(funcName) != this->funcMap.end());
    ASTFunction* func = this->funcMap[funcName];
    std::vector<std::string> signature;
    ASTTypeBasicList* typelist = func->typeDecl->inputTypes;
    while(true) {
        signature.push_back(typelist->head->typeBasic);
        typelist = typelist->tail;
        if (typelist == nullptr) break;
    }
    signature.push_back(func->typeDecl->outputType->typeBasic);
    return signature;
}

Evalue StrSTUNEvaluator::evaluateFunc(std::string funcName, std::vector<Evalue> &realParams) {
    bool anyError = false;
    for(auto& evalue : realParams) {
        if(evalue.isError) anyError = true;
    }
    if (anyError) {
        Evalue result; //unknown type.
        result.isError = true;
        return result;
    }
    if (this->builtinFuncMap.find(funcName) != this->builtinFuncMap.end()) {
        return this->evaluateBuiltinFunc(funcName, realParams);
    } else {
        assert(this->funcMap.find(funcName) != this->funcMap.end());
        return this->evaluateCodeFunc(this->funcMap[funcName], realParams);
    }
}

Evalue StrSTUNEvaluator::evaluateBuiltinFunc(std::string builtinFuncName, std::vector<Evalue> &realParams) {
    if (this->isVerbose) std::cerr << "evaluateBuiltinFunc " << builtinFuncName << std::endl;
    assert(this->builtinFuncMap.find(builtinFuncName) != this->builtinFuncMap.end());
    Evalue (*builtinFunc)(std::vector<Evalue> &) = this->builtinFuncMap[builtinFuncName];
    return (*builtinFunc)(realParams);
}

Evalue StrSTUNEvaluator::evaluateCodeFunc(ASTFunction* func, std::vector<Evalue> &realParams) {
    if (this->isVerbose) std::cerr << "evaluateCodeFunc " << func->funcId->id << ", #realParams " << realParams.size() << std::endl;
    //set params
    std::unordered_map<std::string, Evalue> scalarMap;
    Evalue valZero; valZero.setInt(0);
    Evalue valNil; valNil.setStr("");
    scalarMap["nil"] = valNil;
    scalarMap["zero"] = valZero;

    int paramIdx = 0;
    ASTParamList* paramList = func->paramList;
    ASTTypeBasicList* typeList = func->typeDecl->inputTypes;
    while (true) {
        std::string scalarName = paramList->head->id;
        
        // process val's type.
        assert(realParams.size() > paramIdx);
        Evalue val = realParams[paramIdx];
        std::string typeName = typeList->head->typeBasic;
        if (typeName == "Int") val = val.toType(EvalueType::ET_INT);
        else if(typeName == "Bool") val = val.toType(EvalueType::ET_BOOL);
        else if(typeName == "Str") val = val.toType(EvalueType::ET_STR);
        else assert(false);

        scalarMap[scalarName] = val;
        if (this->isVerbose)  {
            std::cerr << "evaluateCodeFunc add scalar " << scalarName << " [";
            val.print(std::cerr);
            std::cerr << "]" << std::endl;
        }
        
        paramList = paramList->tail;
        typeList = typeList->tail;
        paramIdx++;
        if (paramList == nullptr) break;
        assert(typeList != nullptr);
    }
    assert(typeList == nullptr);
    assert(paramIdx == realParams.size());

    //evaluate func body (expression) with scalarMap. and check type.
    Evalue val = this->evaluateExpr(func->expression, scalarMap);
    std::string typeName = func->typeDecl->outputType->typeBasic;
    if (!val.isError) {
        if (typeName == "Int") assert(val.isInt());
        else if(typeName == "Bool") assert(val.isBool());
        else if(typeName == "Str") assert(val.isStr());
        else assert(false);
    }
    return val;
}

Evalue StrSTUNEvaluator::evaluateExpr(ASTExpression* expr, std::unordered_map<std::string, Evalue> &scalarMap) {
    if (expr->expressionType == ASTExpressionType::ASTEXP_FUNC_APPLY) {
        ASTExpressionFuncApply* funcApExpr = dynamic_cast<ASTExpressionFuncApply*>(expr);
        if(this->isVerbose) std::cerr << "evaluateExpr FuncApply " << funcApExpr->funcId->id << std::endl;
        assert(funcApExpr != nullptr);
        std::vector<Evalue> realParams;
        int exprIdx = 0;
        ASTExpressionList* exprList = funcApExpr->realParams;
        while (true) {
            realParams.push_back(this->evaluateExpr(exprList->head, scalarMap));
            exprList = exprList->tail;
            if (exprList == nullptr) break;
        }
        Evalue applyVal = this->evaluateFunc(funcApExpr->funcId->id, realParams);
        return applyVal;
    } else if (expr->expressionType == ASTExpressionType::ASTEXP_IF_THEN_ELSE) {
        ASTExpressionIfThenElse* ifThenElseExpr = dynamic_cast<ASTExpressionIfThenElse*>(expr);
        if(this->isVerbose) std::cerr << "evaluateExpr IfThenElse " << std::endl;
        assert(ifThenElseExpr != nullptr);
        Evalue condExp = this->evaluateExpr(ifThenElseExpr->condExp, scalarMap);
        if (condExp.isError) {
            Evalue errVal; //unknown type.
            errVal.isError = true;
            return errVal;
        }
        condExp.toType(EvalueType::ET_BOOL);
        if (condExp.valBool) {
            Evalue thenExp = this->evaluateExpr(ifThenElseExpr->thenExp, scalarMap);
            return thenExp;
        } else {
            Evalue elseExp = this->evaluateExpr(ifThenElseExpr->elseExp, scalarMap);
            return elseExp;
        } 
    } else if (expr->expressionType == ASTExpressionType::ASTEXP_SCALAR) {
        ASTExpressionScalar* scalarExpr = dynamic_cast<ASTExpressionScalar*>(expr);
        if(this->isVerbose) std::cerr << "evaluateExpr Scalar " << scalarExpr->scalarName << std::endl;
        assert(scalarExpr != nullptr);
        if(scalarMap.find(scalarExpr->scalarName) != scalarMap.end()) {
            return scalarMap[scalarExpr->scalarName];
        } else if (scalarExpr->scalarName[0] == 'I') {
            assert(scalarExpr->scalarName[1] == '.');
            int ival = std::stoi(scalarExpr->scalarName.substr(2));
            Evalue intConstVal;
            intConstVal.setInt(ival);
            return intConstVal;
        } else if (scalarExpr->scalarName[0] == 'S') {
            assert(scalarExpr->scalarName[1] == '.');
            assert(scalarExpr->scalarName[2] == '\"');
            int scalarLen = scalarExpr->scalarName.length();
            assert(scalarExpr->scalarName[scalarLen - 1] == '\"');
            std::string sval = scalarExpr->scalarName.substr(3, scalarLen-4);
            Evalue strVal;
            strVal.setStr(sval);
            return strVal;
        } else {
            assert(false);
        }
        
    } else assert(false);
}