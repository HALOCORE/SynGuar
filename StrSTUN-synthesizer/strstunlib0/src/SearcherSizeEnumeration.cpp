#include "SearcherSizeEnumeration.hpp"

SearcherSizeEnumeration::SearcherSizeEnumeration(
    ObjBase* environment,
    std::vector<ComponentBase*> applyComponents,
    std::vector<Program*> size0Proggrams,
    Program* (*pCreateAndCheckProgram)(ObjBase* env, ComponentBase* mComponent, std::vector<Program*> mInputPrograms),
    int (*pGetPreScore)(Program* prog)) : applyingComponents(applyComponents) {
    this->environment = environment;
    this->pCreateAndCheckProgram = pCreateAndCheckProgram;
    this->pGetPreScore = pGetPreScore;
    /**
     * @brief Init progs by size. Note that program with size 0 doesn't exist,
     * no use nullptr instead.
     */
    this->progsBySizeBool.push_back(nullptr);
    this->progsBySizeBool.push_back(new std::vector<std::tuple<int, Program*>>());
    this->progsBySizeInt.push_back(nullptr);
    this->progsBySizeInt.push_back(new std::vector<std::tuple<int, Program*>>());
    this->progsBySizeStr.push_back(nullptr);
    this->progsBySizeStr.push_back(new std::vector<std::tuple<int, Program*>>());
    /**
     * @brief Fill programs with size zero. After that, 
     * set componentIndex to 0, increase currentSize, and add new vecs.
     */
    this->currentSize = 1;
    for (Program* prog : size0Proggrams) {
        this->putProgramByCurrentSize(prog);
    }
    this->currentComponentIndex = 0;
    this->currentSize++;
    this->progsBySizeBool.push_back(new std::vector<std::tuple<int, Program*>>());
    this->progsBySizeInt.push_back(new std::vector<std::tuple<int, Program*>>());
    this->progsBySizeStr.push_back(new std::vector<std::tuple<int, Program*>>());
}

void SearcherSizeEnumeration::accept(Visitor* visitor) {
    return;
}

void SearcherSizeEnumeration::enumerateToSize(int size) {
    assert(size > 1);
    while(true) {
        if (this->currentComponentIndex >= this->applyingComponents.size()) {
            if(this->currentSize == size) {
                break;
            }
        }
        this->step();
        PerfCounter::print(std::cerr);
    }
}

void SearcherSizeEnumeration::step() {
    /**
     * @brief Generate all programs for a given component and size.
     */
    assert(this->currentProgramParams.size() == 0);
    if (this->currentComponentIndex >= this->applyingComponents.size()) {
        /**
         * @brief If all component is enumerated, sort results on currentsize,
         * and increase currentSize, reset currentComponentIndex, 
         * and create new vecs for new size.
         */
        assert(this->currentComponentIndex == this->applyingComponents.size());
        /**
         * @brief First, sort each type of programs at this size, 
         */
        auto progVeci = this->getProgramsBySizeAndValType(ValType::INT, this->currentSize);
        std::sort(progVeci->begin(), progVeci->end());
        auto progVecb = this->getProgramsBySizeAndValType(ValType::BOOL, this->currentSize);
        std::sort(progVecb->begin(), progVecb->end());
        auto progVecs = this->getProgramsBySizeAndValType(ValType::STRING, this->currentSize);
        std::sort(progVecs->begin(), progVecs->end());

        /**
         * @brief Second, update componentIndex and currentSize.
         */
        this->currentComponentIndex = 0;
        this->currentSize++;
        
        /**
         * @brief Third, create vecs for new size.
         */
        assert (this->progsBySizeInt.size() == this->currentSize);
        this->progsBySizeInt.push_back(new std::vector<std::tuple<int, Program*>>());
        assert (this->progsBySizeBool.size() == this->currentSize);
        this->progsBySizeBool.push_back(new std::vector<std::tuple<int, Program*>>());
        assert (this->progsBySizeStr.size() == this->currentSize);
        this->progsBySizeStr.push_back(new std::vector<std::tuple<int, Program*>>());
    }

    ComponentBase* comp = this->applyingComponents[this->currentComponentIndex];
    /**
     * @brief the size budget should minus the program it self, 
     * which is 1, so is currentSize - 1.
     */
    std::cerr << "[SearcherSizeEnumeration] " << comp->getComponentId() << " currentSize: " << this->currentSize << std::endl;
    stepRec(comp, 0, this->currentSize - 1);
    this->currentComponentIndex++;
}

void SearcherSizeEnumeration::stepRec(ComponentBase* comp, int paramIndex, int sizeBudget) {
    std::vector<ValType> vts = comp->getInputTypes();
    if (paramIndex >= vts.size()) {
        assert(paramIndex == vts.size());
        /**
         * @brief take current program params to create new program.
         */
        Program* newProg = (*pCreateAndCheckProgram)(this->environment, comp, this->currentProgramParams);
        /**
         * @brief If newProg is not nullptr, Double-check if newly created program have currentSize.
         */
        if (newProg != nullptr) {
            assert(newProg->syntaxSize == this->currentSize);
            this->putProgramByCurrentSize(newProg);
        }
        return;
    }

    /**
     * @brief Get max possible size for this param, and loop through all possible size.
     * Note: minimal start size is 1, not 0.
     * Note: if this is the last parameter, it's minimal start size = sizeBudget = maxSizeForThisParam.
     */
    ValType vt = vts[paramIndex];
    int parameterLeft = vts.size() - (paramIndex + 1);
    int maxSizeForThisParam = sizeBudget - parameterLeft;
    int startSize = parameterLeft > 0 ? 1 : maxSizeForThisParam;
    
    for (int trySize = startSize; trySize <= maxSizeForThisParam; trySize++) {
        std::vector<std::tuple<int, Program*>>* progVec = this->getProgramsBySizeAndValType(vt, trySize);
        /**
         * @brief For each size, loop through all programs with specific type and size.
         */
        for (auto pr : (*progVec)) {
            int preScore;
            Program* prog;
            std::tie(preScore, prog) = pr;
            assert(this->currentProgramParams.size() == paramIndex);
            this->currentProgramParams.push_back(prog);
            /**
             * @brief push parameter and call stepRec recursively.
             */
            stepRec(comp, paramIndex + 1, sizeBudget - trySize);
            /**
             * @brief Pop back the trying program, and try next one.
             */
            this->currentProgramParams.pop_back();
        }
    }
}

void SearcherSizeEnumeration::putProgramByCurrentSize(Program* prog) {
    ValType vtype = prog->outputType;
    std::vector<std::vector<std::tuple<int, Program*>>*>* operateVec;
    if (vtype == ValType::BOOL) {
        operateVec = &this->progsBySizeBool;
    } else if (vtype == ValType::INT) {
        operateVec = &this->progsBySizeInt;
    } else if (vtype == ValType::STRING) {
        operateVec = &this->progsBySizeStr;
    } else {
        assert(false);
    }

    assert (this->currentSize == operateVec->size() - 1);
    std::vector<std::tuple<int, Program*>>* sizeVec = operateVec->at(this->currentSize);
    int preScore = (*this->pGetPreScore)(prog);
    sizeVec->push_back(std::make_tuple(preScore, prog));
}

std::vector<std::tuple<int, Program*>>* SearcherSizeEnumeration::getProgramsBySizeAndValType(
    ValType valType, int size) {
    std::vector<std::vector<std::tuple<int, Program*>>*>* operateVec;
    if (valType == ValType::BOOL) {
        operateVec = &this->progsBySizeBool;
    } else if (valType == ValType::INT) {
        operateVec = &this->progsBySizeInt;
    } else if (valType == ValType::STRING) {
        operateVec = &this->progsBySizeStr;
    } else {
        assert(false);
    }
    assert(operateVec->size() > size);
    return operateVec->at(size);
}