#include "ComponentManager.hpp"
#include "visitorbase.hpp"

ComponentManager::ComponentManager(bool isSynthesizedComponentEnabled) noexcept {
    this->CommonInitialize(isSynthesizedComponentEnabled);
}

void ComponentManager::CommonInitialize(bool isSynthesizedComponentEnabled) {
    /**
     * @brief Create all built-in component and mark const ones first.
     */
    for (auto & id : ComponentBuiltin::componentIdsBuiltin) {
        ComponentBase* comp = new ComponentBuiltin(id);
        this->componentMap[id] = comp;
        if (comp->getInputTypes().size() == 0) {
            this->constComponentIds.push_back(id);
        } else {
            this->applyComponentIds.push_back(id);
        }
        for (auto tp : comp->getInputTypes()) {
            if (tp == ValType::INT) this->acceptIntComponentIds.push_back(id);
            else if (tp == ValType::STRING) this->acceptStrComponentIds.push_back(id);
            else if (tp == ValType::BOOL) this->acceptBoolComponentIds.push_back(id);
            else assert(false);
        }
    }
    /**
     * @brief Create IfThenElse component. 
     * (so far don't add to accept Ids. Not clear about influences.)
     */
    ComponentBase* compBool = new ComponentIfThenElse(ValType::BOOL);
    this->componentMap[compBool->getComponentId()] = compBool;
    ComponentBase* compInt = new ComponentIfThenElse(ValType::INT);
    this->componentMap[compInt->getComponentId()] = compInt;
    ComponentBase* compStr = new ComponentIfThenElse(ValType::STRING);
    this->componentMap[compStr->getComponentId()] = compStr;
    /**
     * @brief Create Synthesized component if isSynthesizedComponentEnabled.
     */
    if(isSynthesizedComponentEnabled) {
        ComponentSynthesized::readSavedAndInitialize();
        for(auto& cid : ComponentSynthesized::componentIdsSynthesized) {
            ComponentBase* compSyn = new ComponentSynthesized(cid);
            this->componentMap[cid] = compSyn;
            assert(compSyn->getInputTypes().size() > 0);
            this->applyComponentIds.push_back(cid);
            for (auto tp : compSyn->getInputTypes()) {
                if (tp == ValType::INT) this->acceptIntComponentIds.push_back(cid);
                else if (tp == ValType::STRING) this->acceptStrComponentIds.push_back(cid);
                else if (tp == ValType::BOOL) this->acceptBoolComponentIds.push_back(cid);
                else assert(false);
            }
        }
    }
}

ComponentManager::ComponentManager(
        std::vector<std::string> activeCompIds,
        std::vector<int> intConsts,
        std::vector<std::string> strConsts) noexcept
{
    //set components
    for(std::string active : activeCompIds) {
        if(active == "=") active = "==";
        int count = std::count(ComponentBuiltin::componentIdsBuiltin.begin(), ComponentBuiltin::componentIdsBuiltin.end(), active);
        if(count == 0) {
            std::cerr << "!!! Component Not Found: " << active << "\n";
            assert(false);
        }
    }
    ComponentBuiltin::componentIdsBuiltin.clear();
    for(std::string active : activeCompIds) {
        if(active == "=") {
            ComponentBuiltin::componentIdsBuiltin.push_back("==");
        } else {
            ComponentBuiltin::componentIdsBuiltin.push_back(active);
        }
    }
    for(int ic : intConsts) {
        ComponentBuiltin::componentIdsBuiltin.push_back("I." + std::to_string(ic));
    }
    for(std::string sc : strConsts) {
        ComponentBuiltin::componentIdsBuiltin.push_back("S.\"" + sc + "\"");
    }
    this->CommonInitialize(false);
}

ComponentBase* ComponentManager::getComponentById(std::string componentId) {
    assert(componentMap.find(componentId) != componentMap.end());
    return componentMap[componentId];
}

std::vector<ComponentBase*> ComponentManager::getComponentsByIds(std::vector<std::string> componentIds) {
    std::vector<ComponentBase*> result;
    for (std::string id : componentIds) {
        result.push_back(this->getComponentById(id));
    }
    return result;
}

std::vector<std::string> ComponentManager::getConstComponentIds() {
    return this->constComponentIds;
}

std::vector<std::string> ComponentManager::getApplyComponentIds() {
    return this->applyComponentIds;
}

std::vector<std::string> ComponentManager::getInputComponentIds() {
    return this->inputComponentIds;
}

std::vector<std::string> ComponentManager::getAcceptTypeComponentIds(ValType tp) {
    if (tp == ValType::BOOL) return acceptBoolComponentIds;
    else if (tp == ValType::INT) return acceptIntComponentIds;
    else if (tp == ValType::STRING) return acceptStrComponentIds;
    else assert(false);
}

ComponentBase* ComponentManager::addInputComponent(int paramIndex, ValType valType, std::vector<ValBase*>* pInputExamples) {
    ComponentBase* comp = new ComponentInput(paramIndex, valType, pInputExamples);
    std::string id = comp->getComponentId();
    this->componentMap[id] = comp;
    this->inputComponentIds.push_back(id);
    return comp;
}

void ComponentManager::accept(Visitor* visitor) {
    for(auto & pair : this->componentMap) {
        ComponentBuiltin* compbin = dynamic_cast<ComponentBuiltin*>(pair.second);
        if (compbin != nullptr) visitor->visit(compbin);
        else {
            ComponentInput* compin = dynamic_cast<ComponentInput*>(pair.second);
            if (compin != nullptr) visitor->visit(compin);
            else {
                ComponentIfThenElse* compif = dynamic_cast<ComponentIfThenElse*>(pair.second);
                if (compif != nullptr) visitor->visit(compif);
                else {
                     ComponentSynthesized* compsyn = dynamic_cast<ComponentSynthesized*>(pair.second);
                    if (compsyn != nullptr) visitor->visit(compsyn);
                    else {
                        std::cerr << "ComponentManager::accept unknown component type." << std::endl;
                    }
                }
            }
        }
    }
    return;
}