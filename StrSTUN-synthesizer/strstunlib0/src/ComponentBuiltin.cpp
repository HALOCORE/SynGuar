#include "ComponentBuiltin.hpp"
#include "visitorbase.hpp"

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline std::string reverse_String(std::string& temp){
  std::string reversed(temp.rbegin(), temp.rend());
  return reversed;
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;
   char * p;
   strtol(s.c_str(), &p, 10);
   return (*p == 0);
}

void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
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

bool isPrefix(std::string const& prefix, std::string const& fullstr)
{
    if (fullstr.length() < prefix.length()) return false;
    auto res = std::mismatch(prefix.begin(), prefix.end(), fullstr.begin());
    if (res.first == prefix.end()) return true;
    return false;
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::unordered_map<std::string, int> ComponentBuiltin::calculateFlags(
    std::vector<std::unordered_map<std::string, int>*> inputsFlags) {
    std::unordered_map<std::string, int> outputFlags;
    return outputFlags; //test
}


std::vector<std::string> ComponentBuiltin::componentIdsBuiltin = {
    //const
    //constant int
    "I.0",
    //constant string
    "S.\"\"",
    "S.\" \"",

    //----------------- bool 

    //bool bool -> bool
    //"and", //remove and totally
    // "or",

    //bool -> bool
    //"not",

    //int int -> bool
    "==",
    
    //string string -> bool
    "str.prefixof",
    "str.suffixof",
    "str.contains",

    
    //----------------- int 

    //int int -> int
    "+",
    "-",

    //string -> int
    "str.len",
    "str.to.int",
    
    //string string int -> int
    "str.indexof",


    //----------------- string 

    //int -> string
    "int.to.str",

    //string int int -> string
    "str.substr",

    //string int -> string
    "str.at",

    //string string -> string
    "str.++",

    //string string stromg -> string
    "str.replace",
};

std::vector<std::string> ComponentBuiltin::componentIdsBuiltinLazy = {
    "and",
    "or"
};

ComponentBuiltin::ComponentBuiltin(std::string mComponentId) {
    if(ObjBase::isVerbose) std::cerr << "create ComponentBuiltin for " << mComponentId << std::endl;
    this->componentId = mComponentId;
    this->componentType = ComponentType::BUILTIN;
    // if (mComponentId == "and") {
    //     this->inputTypes.push_back(ValType::BOOL);
    //     this->inputTypes.push_back(ValType::BOOL);
    //     this->outputType = ValType::BOOL;
    // } 
    // else if (mComponentId == "or") {
    //     this->inputTypes.push_back(ValType::BOOL);
    //     this->inputTypes.push_back(ValType::BOOL);
    //     this->outputType = ValType::BOOL;
    // } 
    // else if (mComponentId == "not") {
    //     this->inputTypes.push_back(ValType::BOOL);
    //     this->outputType = ValType::BOOL;
    // } 
    if (mComponentId == "==") {
        this->inputTypes.push_back(ValType::INT);
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::BOOL;
    } 
    else if (mComponentId == "str.prefixof" || 
        mComponentId == "str.suffixof" || 
        mComponentId == "str.contains") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::STRING);
        this->outputType = ValType::BOOL;
    }
    else if (mComponentId == "+" || mComponentId == "-") {
        this->inputTypes.push_back(ValType::INT);
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::INT;
    } 
    else if (mComponentId == "str.len" || 
        mComponentId == "str.to.int") {
        this->inputTypes.push_back(ValType::STRING);
        this->outputType = ValType::INT;
    } 
    else if (mComponentId == "str.indexof") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::INT;
    } 
    else if (mComponentId == "int.to.str") {
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::STRING;
    } 
    else if (mComponentId == "str.substr") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::INT);
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::STRING;
    } 
    else if (mComponentId == "str.at") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::INT);
        this->outputType = ValType::STRING;
    } 
    else if (mComponentId == "str.++") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::STRING);
        this->outputType = ValType::STRING;
    } 
    else if (mComponentId == "str.replace") {
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::STRING);
        this->inputTypes.push_back(ValType::STRING);
        this->outputType = ValType::STRING;
    } else {
        //handle constant
        if (mComponentId[0] == 'I' && mComponentId[1] == '.') {
            this->outputType = ValType::INT;
        } 
        else if (mComponentId[0] == 'S' && mComponentId[1] == '.') {
            this->outputType = ValType::STRING;
        }
        else if (mComponentId == "true" || mComponentId == "false") {
            this->outputType = ValType::BOOL;
        }
        else {
            assert(false);
        }
    }
}

ValBase* ComponentBuiltin::evaluate(std::vector<ValBase*> inputValues, int exampleIndex) {
    int inputSize = inputValues.size();
    /**
     * @brief Check if the op is lazy.
     */
    bool isLazyOp = false;
    for(std::string lazyOp : this->componentIdsBuiltinLazy) {
        if(lazyOp == this->componentId) {
            isLazyOp = true;
        }
    }
    /**
     * @brief If any input is error, and is not LazyOp, output error directly.
     */
    if(!isLazyOp) {
        bool anyError = false;
        for (ValBase* v : inputValues) {
            if(v->isErr()) anyError = true;
            else assert(v->isValid());
        }
        if(anyError) {
            if (this->outputType == ValType::INT) return new ValInt(0, ValStatus::ERR);
            if (this->outputType == ValType::BOOL) return new ValBool(false, ValStatus::ERR);
            if (this->outputType == ValType::STRING) return new ValStr("", ValStatus::ERR);
        }
    }
    /**
     * @brief Deal with not-error or lazy situations.
     */
    if (this->componentId == "and") {
        /**
         * @brief Lazy and operator.
         */
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::BOOL);
        assert(inputValues[1]->type == ValType::BOOL);
        ValBool* i0 = reinterpret_cast<ValBool*>(inputValues[0]);
        ValBool* i1 = reinterpret_cast<ValBool*>(inputValues[1]);
        if(i0->isErr()) return new ValBool(false, ValStatus::ERR);
        if(i0->value == false) return new ValBool(false);
        if(i1->isErr()) return new ValBool(false, ValStatus::ERR);
        return new ValBool(i1->value);
    } 
    else if (this->componentId == "or") {
        /**
         * @brief Lazy or operator.
         */
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::BOOL);
        assert(inputValues[1]->type == ValType::BOOL);
        ValBool* i0 = reinterpret_cast<ValBool*>(inputValues[0]);
        ValBool* i1 = reinterpret_cast<ValBool*>(inputValues[1]);
        if(i0->isErr()) return new ValBool(false, ValStatus::ERR);
        if(i0->value == true) return new ValBool(true);
        if(i1->isErr()) return new ValBool(false, ValStatus::ERR);
        return new ValBool(i1->value);
    } 
    else if (this->componentId == "not") {
        assert(inputSize == 1);
        assert(inputValues[0]->type == ValType::BOOL);
        ValBool* i0 = reinterpret_cast<ValBool*>(inputValues[0]);
        //Err: always defined.
        return new ValBool(!i0->value);
    } 
    else if (this->componentId == "==") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::INT);
        assert(inputValues[1]->type == ValType::INT);
        ValInt* i0 = reinterpret_cast<ValInt*>(inputValues[0]);
        ValInt* i1 = reinterpret_cast<ValInt*>(inputValues[1]);
        //Err: always defined.
        return new ValBool(i0->value == i1->value);
    } 
    else if (this->componentId == "str.prefixof") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        //Err: always defined.
        bool isprefix = isPrefix(i0->value, i1->value);
        return new ValBool(isprefix);
    } 
    else if (this->componentId == "str.suffixof") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        //Err: always defined.
        bool issuffix = ends_with(i1->value, i0->value);
        return new ValBool(issuffix);
    } 
    else if (this->componentId == "str.contains") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        //Err: always defined.
        bool isContained = i0->value.find(i1->value) != std::string::npos;
        return new ValBool(isContained);
    } 
    else if (this->componentId == "+") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::INT);
        assert(inputValues[1]->type == ValType::INT);
        ValInt* i0 = reinterpret_cast<ValInt*>(inputValues[0]);
        ValInt* i1 = reinterpret_cast<ValInt*>(inputValues[1]);
        //Err: always defined.
        return new ValInt(i0->value + i1->value);
    } 
    else if (this->componentId == "-") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::INT);
        assert(inputValues[1]->type == ValType::INT);
        ValInt* i0 = reinterpret_cast<ValInt*>(inputValues[0]);
        ValInt* i1 = reinterpret_cast<ValInt*>(inputValues[1]);
        //Err: always defined.
        return new ValInt(i0->value - i1->value);
    } 
    else if (this->componentId == "str.len") {
        assert(inputSize == 1);
        assert(inputValues[0]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        //Err: always defined.
        return new ValInt(i0->value.length());
    }
    else if (this->componentId == "str.to.int") {
        assert(inputSize == 1);
        assert(inputValues[0]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        int ival = 0;
        try {
            ival = std::stoi(i0->value);
            return new ValInt(ival);
        }
        catch(std::invalid_argument& e){
            return new ValInt(0, ValStatus::ERR);
        }
        catch(std::out_of_range& e){
            return new ValInt(0, ValStatus::ERR);
        } 
    }
    else if (this->componentId == "str.indexof") {
        assert(inputSize == 3);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        assert(inputValues[2]->type == ValType::INT);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        ValInt* i2 = reinterpret_cast<ValInt*>(inputValues[2]);
        //Err: always defined.
        if(i1->value.length() == 0) return new ValInt(-1);
        int current_find_idx = 0;
        std::size_t start_pos = 0;
        std::size_t new_pos = -1;
        std::vector<std::size_t> found_poss;
        while(true) {
            new_pos = i0->value.find(i1->value, start_pos);
            if(new_pos == std::string::npos) break;
            found_poss.push_back(new_pos);
            start_pos = new_pos + i1->value.length();
            if(start_pos >= i0->value.length()) break;
        }
        int subidx = i2->value;
        if (subidx < 0) subidx = found_poss.size() + subidx;
        if (subidx < 0 || subidx >= found_poss.size()) return new ValInt(-1);
        return new ValInt(found_poss[subidx]);
    }
    else if (this->componentId == "int.to.str") {
        assert(inputSize == 1);
        assert(inputValues[0]->type == ValType::INT);
        ValInt* i0 = reinterpret_cast<ValInt*>(inputValues[0]);
        return new ValStr(std::to_string(i0->value));
    }
    else if (this->componentId == "str.substr") {
        assert(inputSize == 3);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::INT);
        assert(inputValues[2]->type == ValType::INT);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValInt* i1 = reinterpret_cast<ValInt*>(inputValues[1]);
        ValInt* i2 = reinterpret_cast<ValInt*>(inputValues[2]);
        int start = i1->value;
        int sublen = i2->value;
        if(start < 0) start = start + i0->value.size();
        if(sublen < 0) sublen = 1000000;
        if(i0->value.length() <= start) {
            return new ValStr("");
        }
        return new ValStr(i0->value.substr(start, sublen));
    }
    else if (this->componentId == "str.at") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::INT);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValInt* i1 = reinterpret_cast<ValInt*>(inputValues[1]);
        int pos = i1->value;
        if(pos < 0) pos = pos + i0->value.length();
        if(pos < 0 || pos >= i0->value.length()) {
            return new ValStr("", ValStatus::ERR);
        }
        std::string s(1, i0->value[pos]);
        return new ValStr(s);
    }
    else if (this->componentId == "str.++") {
        assert(inputSize == 2);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        //Err: always defined.
        //if(i0->value.size() + i1->value.size() >= 255) return new ValStr("", ValStatus::ERR);
        return new ValStr(i0->value + i1->value);
    } 
    else if (this->componentId == "str.replace") {
        assert(inputSize == 3);
        assert(inputValues[0]->type == ValType::STRING);
        assert(inputValues[1]->type == ValType::STRING);
        assert(inputValues[2]->type == ValType::STRING);
        ValStr* i0 = reinterpret_cast<ValStr*>(inputValues[0]);
        ValStr* i1 = reinterpret_cast<ValStr*>(inputValues[1]);
        ValStr* i2 = reinterpret_cast<ValStr*>(inputValues[2]);
        //Err: always defined.
        if(i1->value == "") return new ValStr("", ValStatus::ERR);
        if(i0->value == "") return new ValStr("");
        std::string result(i0->value);
        findAndReplaceAll(result, i1->value, i2->value);
        return new ValStr(result);
    }
    else 
    {
        //handle constant
        if (this->componentId[0] == 'I' && componentId[1] == '.') {
            std::string toint = this->componentId.substr(2);
            int ival = std::stoi(toint);
            return new ValInt(ival);
        } 
        else if (this->componentId[0] == 'S' && this->componentId[1] == '.') {
            this->outputType = ValType::STRING;
            int length = this->componentId.length();
            assert(this->componentId[2] == '\"' && this->componentId[length - 1] == '\"');
            std::string sval = this->componentId.substr(3, length - 4);
            return new ValStr(sval);
        }
        else if (this->componentId == "true") {
            return new ValBool(true);
        } else if(this->componentId == "false") {
            return new ValBool(false);
        }
        else {
            assert(false);
        }
    }
}

void ComponentBuiltin::accept(Visitor* visitor) {
    //TODO
    return;
}

