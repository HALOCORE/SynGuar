#include "ProgramManager.hpp"
#include "visitorbase.hpp"

//size-level obsEq

//#define PROGMGR_REJECT_ALL_ERROR

ProgramManager::ProgramManager(
    ComponentManager* compMgr, 
    std::vector<ValType> inputExampleTypes,
    ValType outputExampleType) noexcept : strProgramsHashmap(131072), intProgramsHashmap(131072), boolProgramsHashmap(131072) {
    this->componentManager = compMgr;
    this->exampleCount = 0;
    this->searchStrategy = ProgramSearchStrategy::SIZE_ENUMERATION;
    /**
     * @brief Set I/O example signatures.
     */
    this->inputExampleTypes = inputExampleTypes;
    this->outputExampleType = outputExampleType;
    for (int i = 0; i < this->inputExampleTypes.size(); i++) {
        std::vector<ValBase*>* inputExamplesForOneParam = new std::vector<ValBase*>();
        this->inputExamplesByParamIndex.push_back(inputExamplesForOneParam);
    }
    /**
     * @brief create input components.
     */
    for(int i = 0; i < inputExampleTypes.size(); i++) {
        ComponentBase* comp = compMgr->addInputComponent(
            i, inputExampleTypes[i], this->inputExamplesByParamIndex[i]);
    }
    /**
     * @brief set isCleared, so able to add examples.
     */
    this->clearForAddingExamples();
}

void ProgramManager::clearForAddingExamples() {
    std::cerr << "[synthesizer] clearForAddingExamples.\n";
    this->allPrograms.clear();
    this->boolPrograms.clear();
    this->intPrograms.clear();
    this->strPrograms.clear();
    this->ifThenElsePrograms.clear();
    this->isAcceptingExample = true;
    /**
     * goal graph manager is reset when finish adding examples.
     */
}

void ProgramManager::finishAddingExamples() {
    std::cerr << "[synthesizer] finishAddingExamples. #example = " << this->exampleCount << "\n";
    this->isAcceptingExample = false;
    /**
     * @brief Create input programs.
     */
    std::vector<std::string> inputCompIds = this->componentManager->getInputComponentIds();
    for(int i = 0; i < inputExampleTypes.size(); i++) {
        Program* prog = this->addProgram(inputCompIds[i], std::vector<Program*>());
    }
    /**
     * @brief Create const programs.
     */
    for (auto constCompId : this->componentManager->getConstComponentIds()) {
        Program* prog = this->addProgram(constCompId, std::vector<Program*>());
    }
    /**
     * @brief Initialize/Recreate Searcher.
     */
    this->initializeSearcher();
}

void ProgramManager::initializeSearcher() {
    /**
     * @brief Initialize SizeEnumeration Searcher.
     */
    if (this->searchStrategy == ProgramSearchStrategy::SIZE_ENUMERATION) {
        auto preScore = [](Program* p) {return 1; };
        std::vector<std::string> applyCompIds = this->componentManager->getApplyComponentIds();
        std::vector<ComponentBase*> applyComps = this->componentManager->getComponentsByIds(applyCompIds);
        this->searcherSizeEnumeration = new SearcherSizeEnumeration(
            (ObjBase*)this,
            applyComps,
            this->allPrograms,
            this->createAndCheckProgramStatic,
            preScore);
    } else {
        assert(false);
    }
    
}

std::tuple<ValueVector*, std::vector<Program*>*, Program*> __checkProgEquivalent(
    std::vector<std::tuple<ValueVector*, std::vector<Program*>*>>* progSet,
    Program* prog) {
    ValueVector* resultValvec = nullptr;
    std::vector<Program*>* resultProgset = nullptr;
    Program* resultProg = nullptr;
    
    ValueVector* valvec = &prog->outputValue;
    for(auto p : *progSet) {
        ValueVector* existVec;
        std::vector<Program*>* existSet;
        std::tie(existVec, existSet) = p;
        if(valvec->equalTo(*existVec)) {
            resultValvec = existVec;
            resultProgset = existSet;
            for(Program* existProg : *existSet) {
                if(prog->syntaxSize == existProg->syntaxSize) {
                    resultProg = existProg;
                    break;
                }
            }
            break;
        }
    }
    return std::make_tuple(resultValvec, resultProgset, resultProg);
}

std::tuple<ValueVector*, std::vector<Program*>*, Program*> __checkProgEquivalentHashVer(
    std::unordered_map<ValVecP, std::tuple<ValueVector*, std::vector<Program*>*>, ValVecPHashFunc>* valMap,
    Program* prog) {
    ValueVector* resultValvec = nullptr;
    std::vector<Program*>* resultProgset = nullptr;
    Program* resultProg = nullptr;
    
    ValueVector* valvec = &prog->outputValue;
    ValVecP vp; vp.valvec = valvec; 
    if(valMap->find(vp) != valMap->end()) {
        ValueVector* existVec;
        std::vector<Program*>* existSet;
        std::tie(existVec, existSet) = valMap->at(vp);
        assert(valvec->equalTo(*existVec));
        resultValvec = existVec;
        resultProgset = existSet;
        for(Program* existProg : *existSet) {
            if(prog->syntaxSize == existProg->syntaxSize) {
                resultProg = existProg;
                break;
            }
        }
    }
    return std::make_tuple(resultValvec, resultProgset, resultProg);
}

void ProgramManager::deleteAllIfThenElsePrograms() {
    // for(Program* p : this->ifThenElsePrograms) {
    //     delete p;
    // }
    std::cerr << "# ProgramManager clear #ifThenElsePrograms=" << this->ifThenElsePrograms.size() << "\n";
    this->ifThenElsePrograms.clear();
}

Program* ProgramManager::addProgram(
    std::string mComponentId,
    std::vector<Program*> mInputPrograms) {
    BEGIN_FUNC();
    assert(!this->isAcceptingExample);
    
    if(ValBase::isVerbose) std::cerr << "[synthesizer] addProgram called: " << mComponentId << " allProgram.size=" << this->allPrograms.size() << std::endl;
    ComponentBase* comp = this->componentManager->getComponentById(mComponentId);
    std::vector<ValType> inTypes = comp->getInputTypes();
    ValType outType = comp->getOutputType();
    /**
     * @brief First, check component matches input programs or not.
     */
    assert(inTypes.size() == mInputPrograms.size());
    auto it = inTypes.begin();
    auto it2 = mInputPrograms.begin();
    for (; it != inTypes.end() && it2 != mInputPrograms.end(); it++, it2++) {
        assert((*it) == (*it2)->outputType);
    }

    // std::unordered_map<std::string, int> outputFlags; //NOTICE: Always empty flag.
    // if(false) {
    //     /**
    //      * @brief Second, calculate output Flags based on input components, 
    //      * and return null if "STOP" flag shows up. 
    //      */
    //     std::vector<std::unordered_map<std::string, int>*> inputsFlags;
    //     for(Program* p : mInputPrograms) {
    //         inputsFlags.push_back(&(p->outputFlags));
    //     }
    //     std::unordered_map<std::string, int> outputFlags = comp->calculateFlags(inputsFlags);
    //     if (outputFlags.find(COMPONENT_FLAG_STOP) != outputFlags.end()) {
    //         if(ObjBase::isVerbose) std::cerr << "addProgram calculate flag get STOP, ignore this program." << std::endl;
    //         EVENT_COUNT("ProgDdpFlg");
    //         return nullptr;
    //     }
    // }
    
    
    /**
     * @brief Third, generate groups of inputs and produce outputs for new Program.
     */
    ValueVector outputVec;
    bool anyValid = false;
    for(int i = 0; i < this->exampleCount; i++) {
        std::vector<ValBase*> inputParamsValue;
        for (auto p : mInputPrograms) {
            inputParamsValue.push_back(p->outputValue.values[i]);
        }
        ValBase* outputValue = comp->evaluate(inputParamsValue, i);
        outputVec.values.push_back(outputValue);
        if (outputValue->isValid()) anyValid = true;
    }

    /**
     * @brief      
     * Note: if all output is ERR, reject this program.
     */
    #ifdef PROGMGR_REJECT_ALL_ERROR
    if(this->exampleCount > 0 && !anyValid) {
        if(ObjBase::isVerbose) std::cerr << "addProgram ignore all ERR value vector." << std::endl;
        EVENT_COUNT("ProgDdpErr");
        return nullptr;
    }
    #endif
    

    /**
     * @brief Construct Probram object and add it to proper collections.
     */
    Program* prog = new Program(
        mComponentId, 
        inTypes, 
        mInputPrograms, 
        outType,
        outputVec); //outputFlags
    

    if (comp->getCommponentType() == ComponentType::IF_THEN_ELSE) {
        this->ifThenElsePrograms.push_back(prog);
        return prog;
    } else {
        /**
         * @brief Check this program against all existing program, but requires same size. 
         * if have value vector in same size, return nullptr.
         * The update of #programs is in here.
         */
        #define PM_HASHMAP_ONLY
        //#define PM_VECTOR_ONLY
        
        std::vector<std::tuple<ValueVector *, std::vector<Program *> *>>* checkSet = nullptr;
        std::unordered_map<ValVecP, std::tuple<ValueVector*, std::vector<Program*>*>, ValVecPHashFunc>* checkSetHash = nullptr;
        if(prog->outputType == ValType::BOOL) {
            checkSet = &this->boolPrograms;
            checkSetHash = &this->boolProgramsHashmap;
        } else if(prog->outputType == ValType::INT) {
            checkSet = &this->intPrograms;
            checkSetHash = &this->intProgramsHashmap;
        } else if(prog->outputType == ValType::STRING) {
            checkSet = &this->strPrograms;
            checkSetHash = &this->strProgramsHashmap;
        } else {
            assert(false);
        }
       
        #ifdef PM_HASHMAP_ONLY
        auto obsEqVecAndProg = __checkProgEquivalentHashVer(checkSetHash, prog);
        #else
        #ifdef PM_VECTOR_ONLY
        auto obsEqVecAndProg = __checkProgEquivalent(checkSet, prog);
        #else
        auto obsEqVecAndProg = __checkProgEquivalentHashVer(checkSetHash, prog);
        auto obsEqVecAndProg2 = __checkProgEquivalent(checkSet, prog);
        assert(std::get<0>(obsEqVecAndProg) == std::get<0>(obsEqVecAndProg2));
        assert(std::get<1>(obsEqVecAndProg) == std::get<1>(obsEqVecAndProg2));
        assert(std::get<2>(obsEqVecAndProg) == std::get<2>(obsEqVecAndProg2));
        #endif
        #endif
        
        ValueVector* existVec = std::get<0>(obsEqVecAndProg);
        std::vector<Program*>* existProgset = std::get<1>(obsEqVecAndProg);
        Program* existProg = std::get<2>(obsEqVecAndProg);
        if(existVec != nullptr) {
            if(existProg != nullptr) {
                //Add obsEqCount to existProg
                //assert(prog->obsEqCount > 0);
                assert(existProg->syntaxSize == prog->syntaxSize);
                existProg->obsEqCount += prog->obsEqCount;
                EVENT_COUNT("ProgDdpObsPROG");
                delete prog; //try delete prog.
                return nullptr;
            } else {
                //this prog should be enumerated, but add to same valuevec
                //assert(prog->obsEqCount > 0);
                if(ObjBase::isLeveledObsEq) {
                    existProgset->push_back(prog);
                    this->allPrograms.push_back(prog);
                    EVENT_COUNT("ProgDdpObsVALVEC");
                    EVENT_COUNT("ProgAdded");
                    LEVEL_SET("#progBySize", prog->syntaxSize);
                    LEVELED_COUNT("#progBySize");
                    return prog;
                } else {
                    EVENT_COUNT("ProgDdpObsPROG");
                    delete prog;
                    return nullptr;
                }
            }
        } else {
            std::vector<Program*>* newProgSet = new std::vector<Program*>();
            newProgSet->push_back(prog);
            ValVecP vp; vp.valvec = &prog->outputValue;
            checkSet->push_back(std::make_tuple(&prog->outputValue, newProgSet));
            checkSetHash->insert(std::make_pair(vp, std::make_tuple(&prog->outputValue, newProgSet)));
            this->allPrograms.push_back(prog);
            EVENT_COUNT("ProgVecAdded");
            EVENT_COUNT("ProgAdded");
            LEVEL_SET("#progBySize", prog->syntaxSize);
            LEVELED_COUNT("#progBySize");
            return prog;
        }
    }
    assert(false);
}

Program* ProgramManager::addProgramStatic(
    ObjBase* progManager,
    std::string mComponentId,
    std::vector<Program*> mInputPrograms) {
    ProgramManager* pmgr = reinterpret_cast<ProgramManager*>(progManager);
    return pmgr->addProgram(mComponentId, mInputPrograms);   
}

void ProgramManager::explorePrograms(int size) {
    if (this->searchStrategy == ProgramSearchStrategy::SIZE_ENUMERATION) {
        this->searcherSizeEnumeration->enumerateToSize(size);
    } else assert(false);
}

/**
 * @brief Used by SearcherSizeEnumeration.
 * Note: possibly returning nullptr.
 * 
 * @param progManager       a ProgramManager pointer casted to ObjBase.
 * @param mComponent        pointer to the component (to create program)
 * @param mInputPrograms    vector of input programs for creating program
 * @return Program*         created program, or a nullptr
 */
Program* ProgramManager::createAndCheckProgramStatic(ObjBase* progManager, ComponentBase* mComponent, std::vector<Program*> mInputPrograms) {
    ProgramManager* pmgr = reinterpret_cast<ProgramManager*>(progManager);
    std::string compId = mComponent->getComponentId();
    Program* prog = pmgr->addProgram(compId, mInputPrograms);
    return prog;
}


std::vector<ValBase*> ProgramManager::fetchExampleInputStatic(ObjBase* progManager, int exampleIndex) {
    ProgramManager* pmgr = reinterpret_cast<ProgramManager*>(progManager);
    std::vector<ValBase*> inputExample;
    for (auto vec : pmgr->inputExamplesByParamIndex) {
        inputExample.push_back(vec->at(exampleIndex));
    }
    return inputExample;
}

bool ProgramManager::isExampleDuplicated(
    std::vector<ValBase*>& inputParamsValue,
    ValBase* outputValue) {
    /**
     * @brief Check duplication. If duplicated, return true.
     */
    assert(this->inputExamplesByParamIndex.size() > 0);
    assert(this->inputExamplesByParamIndex.size() == inputParamsValue.size());
    for(int i = 0; i < this->inputExamplesByParamIndex[0]->size(); i++) {
        bool anyDifferent = false;
        for(int pIndex = 0; pIndex < inputParamsValue.size(); pIndex++) {
            ValBase* pNewParam = inputParamsValue[pIndex];
            ValBase* pExistingParam = this->inputExamplesByParamIndex[pIndex]->at(i);
            if (!(pNewParam->equalTo(pExistingParam))) anyDifferent = true;
        }
        if (anyDifferent == false) {
            return true;
        }
    }
    return false;
}

bool ProgramManager::addExample(
    std::vector<ValBase*> inputParamsValue,
    ValBase* outputValue) {
    BEGIN_FUNC();
    /**
     * @brief Check duplication. If duplicated, return.
     */
    bool isDuplicated = this->isExampleDuplicated(inputParamsValue, outputValue);
    if (isDuplicated) std::cerr << "[synthesizer] addExample deduplicated!";
    else std::cerr << "[synthesizer] addExample new. ";
    for(auto& vp : inputParamsValue) {
        std::cerr << "(";
        vp->print(std::cerr);
        std::cerr << ")";
    }
    std::cerr << " -> (";
    outputValue->print(std::cerr);
    std::cerr << ")\n";
    if(isDuplicated) return false;

    assert(this->isAcceptingExample);
    /**
     * @brief First, increment exampleCount, update input examples and output examples.
     */
    this->exampleCount++;
    LEVELED_SETVAL("#egBySatuRound", this->exampleCount);
    assert(this->inputExamplesByParamIndex.size() == inputParamsValue.size());
    for(int i = 0; i < inputParamsValue.size(); i++) {
        this->inputExamplesByParamIndex[i]->push_back(inputParamsValue[i]);
    }
    this->outputExamples.push_back(outputValue);
    return true;
}

void ProgramManager::accept(Visitor* visitor) {
    // for (auto p : this->allPrograms) {
    //     visitor->visit(p);
    // }
    std::cerr << "ProgramManager::accept not implemented.\n";
}