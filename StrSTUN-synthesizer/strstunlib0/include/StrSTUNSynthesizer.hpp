#ifndef STREC_SYNTHESIZER
#define STREC_SYNTHESIZER

#include "strstunbase.hpp"
#include "strstunvalues.hpp"
#include "Program.hpp"
#include "ProgramManager.hpp"

#define TERM_SIZE 9
#define GOAL_DEPTH 2

class StrSTUNSynthesizer {
public:
    StrSTUNSynthesizer(
        std::vector<ValType> inputTypes,
        ValType outputType);

    StrSTUNSynthesizer(
        std::vector<ValType> inputTypes,
        ValType outputType,
        std::vector<std::string> activeCompIds,
        std::vector<int> intConsts,
        std::vector<std::string> strConsts);
    
    void addExample(
        std::vector<ValBase*> inputParamsValue,
        ValBase* outputValue);

    void synthesis(int goalDepth);
    void synthesis_without_counting();
    void synthesisEnumerationPhrase();
    void synthesisGoalGraphPhrase(int firstN, int goalDepth=GOAL_DEPTH);

    Program* getResultProgram();    
    std::string serializeProgram(Program* prog);

    void debugPrint();

private:
    ProgramManager* progMgr;
    ComponentManager* compMgr;
    GoalGraphManager* graphMgr;
    std::vector<int> isNewEgInfo;
};

#endif