#ifndef COMPONENT_INPUT
#define COMPONENT_INPUT

#include "componentbase.hpp"

class ComponentInput : public ComponentBase {
public:
    ComponentInput(int paramIndex, ValType valType, std::vector<ValBase*>* pInputExamples);
    ValBase* evaluate(std::vector<ValBase*> inputValues, int exampleIndex) override;
    void accept(Visitor* visitor) override;
    std::unordered_map<std::string, int> calculateFlags(
        std::vector<std::unordered_map<std::string, int>*> inputsFlags) override;
    int getParamIndex();
private:
    int paramIndex;
    std::vector<ValBase*>* pInputExamples;
};

#endif