#ifndef COMPONENT_MANAGER
#define COMPONENT_MANAGER

#include "componentbase.hpp"
#include "ComponentBuiltin.hpp"
#include "ComponentInput.hpp"
#include "ComponentIfThenElse.hpp"
#include "ComponentSynthesized.hpp"
#include <unordered_map>

class ComponentManager : public ObjBase {
public:
    ComponentManager(bool isSynthesizedComponentEnabled) noexcept;
    ComponentManager(
        std::vector<std::string> activeCompIds,
        std::vector<int> intConsts,
        std::vector<std::string> strConsts
    ) noexcept;
    void CommonInitialize(bool isSynthesizedComponentEnabled);
    ComponentBase* getComponentById(std::string componentId);
    std::vector<ComponentBase*> getComponentsByIds(std::vector<std::string> componentIds);

    std::vector<std::string> getAcceptTypeComponentIds(ValType tp);
    std::vector<std::string> getConstComponentIds();
    std::vector<std::string> getApplyComponentIds();
    std::vector<std::string> getInputComponentIds();
    
    ComponentBase* addTargetFuncComponent(
        std::vector<ValType> funcInputTypes, 
        ValType funcOutputType, 
        ObjBase* fetchExampleObj,
        std::vector<ValBase*> (*exampleInputFetcher)(ObjBase* fetchExampleObj, int exampleIndex));
    
    ComponentBase* addInputComponent(
        int paramIndex, 
        ValType valType, 
        std::vector<ValBase*>* pInputExamples);

    //void addCustomComponent(std::string, ...)
    void accept(Visitor* visitor) override;

private:
    std::unordered_map<std::string, ComponentBase*> componentMap;
    std::vector<std::string> constComponentIds;
    std::vector<std::string> inputComponentIds;
    std::vector<std::string> applyComponentIds;
    std::vector<std::string> acceptIntComponentIds;
    std::vector<std::string> acceptStrComponentIds;
    std::vector<std::string> acceptBoolComponentIds;
};

#endif