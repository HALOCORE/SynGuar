#ifndef COMPONENT_IFTHENELSE
#define COMPONENT_IFTHENELSE

#include "componentbase.hpp"

class ComponentIfThenElse : public ComponentBase {
public:
    ComponentIfThenElse(ValType valType);
    ValBase* evaluate(std::vector<ValBase*> inputValues, int exampleIndex) override;
    static std::vector<std::string> componentIdByValType;

    void accept(Visitor* visitor) override;
    std::unordered_map<std::string, int> calculateFlags(
        std::vector<std::unordered_map<std::string, int>*> inputsFlags) override;
};

#endif