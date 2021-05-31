#ifndef COMPONENT_BUILTIN
#define COMPONENT_BUILTIN

#include "componentbase.hpp"

/**
 * @brief All builtin operations is implemented in this class.
 * See .cpp file for detail.
 */
class ComponentBuiltin : public ComponentBase {
public:
    ComponentBuiltin(std::string mComponentId);
    ValBase* evaluate(std::vector<ValBase*> inputValues, int exampleIndex) override;
    static std::vector<std::string> componentIdsBuiltin;
    static std::vector<std::string> componentIdsBuiltinLazy;
    static std::vector<std::vector<std::vector<std::string>>> flagCalcInfo;
    void accept(Visitor* visitor) override;
    std::unordered_map<std::string, int> calculateFlags(
        std::vector<std::unordered_map<std::string, int>*> inputsFlags) override;
};

#endif