#ifndef COMPONENTBASE
#define COMPONENTBASE

#include "strstunbase.hpp"
#include "strstunvalues.hpp"

const std::string COMPONENT_FLAG_STOP = "STOP";

enum ComponentType : int {
    BUILTIN,
    EXAMPLE_INPUT,
    IF_THEN_ELSE,
    TARGET_FUNC,
    SYNTHESIZED
};

class ComponentBase : public ObjBase {
protected:
    std::string componentId;
    std::vector<ValType> inputTypes;
    ValType outputType;
    ComponentType componentType;
public:
    std::string getComponentId();
    std::vector<ValType> getInputTypes();
    ValType getOutputType();
    ComponentType getCommponentType();
    virtual ValBase* evaluate(std::vector<ValBase*> inputValues, int exampleIndex) = 0;
    virtual std::unordered_map<std::string, int> calculateFlags(
        std::vector<std::unordered_map<std::string, int>*> inputsFlags) = 0;
};

#endif