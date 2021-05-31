#ifndef PROGRAM_MANAGER
#define PROGRAM_MANAGER

#include "strstunbase.hpp"
#include "Program.hpp"
#include "ComponentManager.hpp"
#include "ProgramsScore.hpp"
#include "SearcherSizeEnumeration.hpp"
#include "GoalGraphManager.hpp"

enum ProgramSearchStrategy : int {
    ASTAR,
    SIZE_ENUMERATION,
    DEPTH_ENUMERATION,
    NOT_SET,
};

class ProgramManager : public ObjBase {
public:
    /**
     * @brief Currently ifThenElsePrograms is not 
     * in any other program collections.
     */
    std::vector<Program*> ifThenElsePrograms;

    std::vector<Program*> allPrograms;

    //a vector of values. each value, a vector of programs with different size.
    std::vector<std::tuple<ValueVector*, std::vector<Program*>*>> intPrograms;
    std::unordered_map<ValVecP, std::tuple<ValueVector*, std::vector<Program*>*>, ValVecPHashFunc> intProgramsHashmap;
    //a vector of values. 
    std::vector<std::tuple<ValueVector*, std::vector<Program*>*>> boolPrograms;
    std::unordered_map<ValVecP, std::tuple<ValueVector*, std::vector<Program*>*>, ValVecPHashFunc> boolProgramsHashmap;
    //a vector of values. 
    std::vector<std::tuple<ValueVector*, std::vector<Program*>*>> strPrograms;
    std::unordered_map<ValVecP, std::tuple<ValueVector*, std::vector<Program*>*>, ValVecPHashFunc> strProgramsHashmap;

    ComponentManager* componentManager;
    GoalGraphManager* goalGraphManager;

    SearcherSizeEnumeration* searcherSizeEnumeration;

    int exampleCount;
    std::vector<ValType> inputExampleTypes;
    ValType outputExampleType;
    std::vector<std::vector<ValBase*>*> inputExamplesByParamIndex;
    std::vector<ValBase*> outputExamples;

public:
    ProgramManager(
        ComponentManager* compMgr, 
        std::vector<ValType> inputExampleTypes, 
        ValType outputExampleType) noexcept;

    void clearForAddingExamples();
    void finishAddingExamples();
    //do deduplication, but for same size.
    bool addExample(
        std::vector<ValBase*> inputParamsValue,
        ValBase* outputValue);
    bool isExampleDuplicated(
        std::vector<ValBase*>& inputParamsValue,
        ValBase* outputValue);
    void explorePrograms(int size);
    void accept(Visitor* visitor) override;
    void deleteAllIfThenElsePrograms();

    static Program* createAndCheckProgramStatic(ObjBase* progManager, ComponentBase* mComponent, std::vector<Program*> mInputPrograms);
    static std::vector<ValBase*> fetchExampleInputStatic(ObjBase* progManager, int exampleIndex);
    
    Program* addProgram(
        std::string mComponentId,
        std::vector<Program*> mInputPrograms);
    static Program* addProgramStatic(
        ObjBase* progManager,
        std::string mComponentId,
        std::vector<Program*> mInputPrograms);
    
    ProgramSearchStrategy searchStrategy;

private:
    /**
     * @brief if is cleared, means no program and goal graph is empty.
     * able to add examples.
     */
    bool isAcceptingExample;
    /**
     * @brief called when finish add example.
     */
    void initializeSearcher();

    /**
     * @brief called by saturateRecursion.
     * @param prog 
     * @param index 
     * @param isBooleanFiltered 
     * @param booleanFilter 
     */
    void saturateRecursionRec(Program* prog, int index, std::vector<std::vector<ValBase*>>& newExampleInputs, std::vector<ValBase*>& newExampleOutputs);
};

#endif