#include "ComponentSynthesized.hpp"
#include "visitorbase.hpp"

#include <iostream>
#include <fstream>

std::vector<std::string> ComponentSynthesized::componentIdsSynthesized;
std::string ComponentSynthesized::filename = "./strec-saved.txt";
StrSTUNEvaluator* ComponentSynthesized::evaluator = nullptr;

ComponentSynthesized::ComponentSynthesized(std::string mComponentId) {
    this->componentId = mComponentId;
    this->componentType = ComponentType::SYNTHESIZED;
    assert(this->evaluator->containsFunc(mComponentId));
    std::vector<std::string> sig = this->evaluator->getFuncSignature(mComponentId);
    this->outputType = ComponentSynthesized::strTypeToValType(sig.back());
    sig.pop_back();
    std::vector<ValType> inputTypes;
    for(auto& s : sig) {
        inputTypes.push_back(ComponentSynthesized::strTypeToValType(s));
    }
    this->inputTypes = inputTypes;
    std::cerr << "[ComponentSynthesized] create " << this->componentId << ", #param " << inputTypes.size() << std::endl;
}

void ComponentSynthesized::readSavedAndInitialize() {
    //create evaluator
    ComponentSynthesized::evaluator = new StrSTUNEvaluator(ObjBase::isVerbose);
    //parse file
    std::cerr << "[ComponentSynthesized][readSavedAndInitialize] read file: " << ComponentSynthesized::filename << std::endl;
    Parser myParser;
    std::ifstream funcfile;
    funcfile.open(ComponentSynthesized::filename);
    myParser.setIO(funcfile, std::cerr);
    int result = myParser.parse();
    funcfile.close();
    std::cerr << "[ComponentSynthesized][readSavedAndInitialize] Parsing " << (result ? "Empty/Error" : "OK") << std::endl;
    if(result == 0) {
        //get parse result
        ASTFunctionList* root = dynamic_cast<ASTFunctionList*>(myParser.root);
        assert(root != nullptr);
        if (ObjBase::isVerbose) {
            std::cerr << "[ComponentSynthesized][readSavedAndInitialize] Parsed Code:" << std::endl;
            (*root) >> std::cerr;
            std::cerr << std::endl;
        }
        ASTFunctionList* funcList = root;
        //add functions to evaluator
        while(true) {
            ASTFunction* head = funcList->head;
            evaluator->addFunc(head);
            std::string funcId = head->funcId->id;
            std::cerr << "[ComponentSynthesized] add func:" << funcId << std::endl;
            ComponentSynthesized::componentIdsSynthesized.push_back(funcId);
            funcList = funcList->tail;
            if (funcList == nullptr) break;
        }
    }
}

ValBase* ComponentSynthesized::evaluate(std::vector<ValBase*> inputValues, int exampleIndex) {
    std::vector<Evalue> inputs;
    assert(inputValues.size() == this->inputTypes.size());
    for (int i = 0; i < this->inputTypes.size(); i++) {
        std::string currentType = ComponentSynthesized::valTypeToStrType(this->inputTypes[i]);
        ValBase* currentVal = inputValues[i];
        if(!currentVal->isValid()) {
            /**
             * @brief If any input is not valid, return err directly.
             */
            if(this->outputType == ValType::BOOL) return new ValBool(false, ValStatus::ERR);
            else if (this->outputType == ValType::INT) return new ValInt(0, ValStatus::ERR);
            else if (this->outputType == ValType::STRING) return new ValStr("", ValStatus::ERR);
            else assert(false);
        }
        Evalue inputVal = ComponentSynthesized::valBaseToEvalue(currentVal);
        inputs.push_back(inputVal);
    }
    Evalue result = this->evaluator->evaluateFunc(this->componentId, inputs);
    return ComponentSynthesized::evalueToValBase(result, this->outputType);
}

std::unordered_map<std::string, int> ComponentSynthesized::calculateFlags(std::vector<std::unordered_map<std::string, int>*> inputsFlags) {
    assert(inputsFlags.size() == this->inputTypes.size());
    return std::unordered_map<std::string, int>();
}


void ComponentSynthesized::accept(Visitor* visitor) {
    return;
}

std::string ComponentSynthesized::valTypeToStrType(ValType valType) {
    if (valType == ValType::STRING) return "Str";
    else if (valType == ValType::INT) return "Int";
    else if (valType == ValType::BOOL) return "Bool";
    else assert(false);
}

ValType ComponentSynthesized::strTypeToValType(std::string strType) {
    if (strType == "Str") return ValType::STRING;
    else if (strType == "Int") return ValType::INT;
    else if (strType == "Bool") return ValType::BOOL;
    else assert(false);
}

Evalue ComponentSynthesized::valBaseToEvalue(ValBase* valBase) {
    assert(valBase->isValid());
    if (valBase->type == ValType::BOOL) {
        ValBool* vbool = dynamic_cast<ValBool*>(valBase);
        assert(vbool != nullptr);
        Evalue val;
        val.setBool(vbool->value);
        return val;
    } else if (valBase->type == ValType::INT) {
        ValInt* vint = dynamic_cast<ValInt*>(valBase);
        assert(vint != nullptr);
        Evalue val;
        val.setInt(vint->value);
        return val;
    } else if (valBase->type == ValType::STRING) {
        ValStr* vstr = dynamic_cast<ValStr*>(valBase);
        assert(vstr != nullptr);
        Evalue val;
        val.setStr(vstr->value);
        return val;
    } else {
        assert(false);
    }
}

ValBase* ComponentSynthesized::evalueToValBase(Evalue evalue, ValType valType) {
    if (valType == ValType::BOOL) {
        assert(evalue.isBool() || (evalue.isError));
        return new ValBool(evalue.valBool, evalue.isError ? ValStatus::ERR : ValStatus::VALID);
    } else if (valType == ValType::INT) {
        assert(evalue.isInt() || (evalue.isError));
        return new ValInt(evalue.valInt, evalue.isError ? ValStatus::ERR : ValStatus::VALID);
    }else if (valType == ValType::STRING) { 
        assert(evalue.isStr() || (evalue.isError));
        return new ValStr(evalue.valStr, evalue.isError ? ValStatus::ERR : ValStatus::VALID);
    }
    else assert(false);
}