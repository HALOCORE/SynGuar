#ifndef COMPONENT_SYNTHESIZED
#define COMPONENT_SYNTHESIZED

#include "strstunbase.hpp"
#include "strstunvalues.hpp"
#include "componentbase.hpp"
#include "StrSTUNEvaluator.hpp"

class ComponentSynthesized : public ComponentBase {
public:
    static std::string filename;
    static void readSavedAndInitialize();

    ComponentSynthesized(std::string mComponentId);
    ValBase* evaluate(std::vector<ValBase*> inputValues, int exampleIndex) override;

    static std::vector<std::string> componentIdsSynthesized;

    void accept(Visitor* visitor) override;
    std::unordered_map<std::string, int> calculateFlags(
        std::vector<std::unordered_map<std::string, int>*> inputsFlags) override;

private:
    static StrSTUNEvaluator* evaluator;
    static std::string valTypeToStrType(ValType valType);
    static ValType strTypeToValType(std::string strType);
    static Evalue valBaseToEvalue(ValBase* valBase);
    static ValBase* evalueToValBase(Evalue evalue, ValType valType);
};



#endif