#include "componentbase.hpp"

std::string ComponentBase::getComponentId() {
    return this->componentId;
}

ComponentType ComponentBase::getCommponentType() {
    return this->componentType;
}

std::vector<ValType> ComponentBase::getInputTypes() {
    return this->inputTypes;
}

ValType ComponentBase::getOutputType() {
    return this->outputType;
}