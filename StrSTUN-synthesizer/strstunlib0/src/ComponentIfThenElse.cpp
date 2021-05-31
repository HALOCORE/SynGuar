#include "ComponentIfThenElse.hpp"
#include "visitorbase.hpp"

std::vector<std::string> ComponentIfThenElse::componentIdByValType = {
    "ifThenElseINT", //0 = INT
    "ifThenElseBOOL", // 1 = BOOL
    "ifThenElseSTRING" // 2 = STRING
};

ComponentIfThenElse::ComponentIfThenElse(ValType valType) {
    this->componentId = this->componentIdByValType[valType];
    this->componentType = ComponentType::IF_THEN_ELSE;
    this->outputType = valType;
    this->inputTypes.push_back(ValType::BOOL);
    this->inputTypes.push_back(valType);
    this->inputTypes.push_back(valType);
}

ValBase* ComponentIfThenElse::evaluate(std::vector<ValBase*> inputValues, int exampleIndex) {
    assert(inputValues.size() == 3);
    ValBool* boolVal = dynamic_cast<ValBool*>(inputValues[0]);
    ValBase* thenVal = inputValues[1];
    ValBase* elseVal = inputValues[2];
    assert(boolVal != nullptr);
    if(boolVal->status != ValStatus::VALID) {
        assert(boolVal->status == ValStatus::ERR);
        if (this->outputType == ValType::INT) return new ValInt(0, ValStatus::ERR);
        else if (this->outputType == ValType::BOOL) return new ValBool(false, ValStatus::ERR);
        else if (this->outputType == ValType::STRING) return new ValStr("", ValStatus::ERR);
        else assert(false);
    }
    assert(thenVal->type == elseVal->type && thenVal->type == this->outputType);
    if (boolVal->value) return thenVal;
    else return elseVal;
}

void ComponentIfThenElse::accept(Visitor* visitor) {
    return;
}

std::unordered_map<std::string, int> ComponentIfThenElse::calculateFlags(
    std::vector<std::unordered_map<std::string, int>*> inputsFlags) {
    assert(inputsFlags.size() == 3);
    return std::unordered_map<std::string, int>();
}
