#include "ComponentInput.hpp"
#include "visitorbase.hpp"

ComponentInput::ComponentInput(int paramIndex, ValType valType, std::vector<ValBase*>* pInputExamples) {
    this->componentId = "input" + std::to_string(paramIndex);
    this->paramIndex = paramIndex;
    this->outputType = valType;
    this->pInputExamples = pInputExamples;
    this->componentType = ComponentType::EXAMPLE_INPUT;
}

ValBase* ComponentInput::evaluate(std::vector<ValBase*> inputValues, int exampleIndex) {
    assert(exampleIndex < this->pInputExamples->size());
    return this->pInputExamples->at(exampleIndex);
}

void ComponentInput::accept(Visitor* visitor) {
    return;
}

std::unordered_map<std::string, int> ComponentInput::calculateFlags(
    std::vector<std::unordered_map<std::string, int>*> inputsFlags) {
    assert(inputsFlags.size() == 0);
    return std::unordered_map<std::string, int>();
}

int ComponentInput::getParamIndex() {
    return this->paramIndex;
}