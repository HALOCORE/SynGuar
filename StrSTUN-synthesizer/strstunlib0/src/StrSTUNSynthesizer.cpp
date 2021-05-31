#include <iostream>
#include <chrono>
#include "StrSTUNSynthesizer.hpp"
#include "strstunvalues.hpp"

#include "ValueVector.hpp"
#include "VisitorPretty.hpp"
#include "Program.hpp"
#include "ProgramManager.hpp"
#include "ComponentManager.hpp"
#include "ProgramCodeGen.hpp"

StrSTUNSynthesizer::StrSTUNSynthesizer(
    std::vector<ValType> inputTypes,
    ValType outputType) {
    
    this->compMgr = new ComponentManager(false); //Don't do memorization 
    this->progMgr = new ProgramManager(this->compMgr, inputTypes, outputType);
    this->graphMgr = nullptr;
}

StrSTUNSynthesizer::StrSTUNSynthesizer(
        std::vector<ValType> inputTypes,
        ValType outputType,
        std::vector<std::string> activeCompIds,
        std::vector<int> intConsts,
        std::vector<std::string> strConsts) {
    //update compMgr with consts. TODO
    this->compMgr = new ComponentManager(activeCompIds, intConsts, strConsts); //Don't do memorization
    this->progMgr = new ProgramManager(this->compMgr, inputTypes, outputType);
    this->graphMgr = nullptr;
}

void StrSTUNSynthesizer::debugPrint() {
    std::cerr << "\n\n======================" << std::endl;
    VisitorPretty::common->visit(this->compMgr);
    std::cerr << "--------------------" << std::endl;
    VisitorPretty::common->visit(this->progMgr);
    std::cerr << "--------------------" << std::endl;
    VisitorPretty::common->visit(this->graphMgr);
}

void StrSTUNSynthesizer::addExample(
    std::vector<ValBase*> inputParamsValue,
    ValBase* outputValue) {
    bool isAdded = this->progMgr->addExample(inputParamsValue, outputValue);
    isNewEgInfo.push_back(isAdded);
}

void StrSTUNSynthesizer::synthesisEnumerationPhrase() {
    this->progMgr->finishAddingExamples();
    //enumeration
    std::chrono::high_resolution_clock::time_point enumt1 = std::chrono::high_resolution_clock::now();
    this->progMgr->explorePrograms(TERM_SIZE);
    std::chrono::high_resolution_clock::time_point enumt2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> enumspan = enumt2 - enumt1;
    double enum_time = enumspan.count();
    std::cerr << "# StrSTUNSynthesizer::synthesis ENUM TIME: " << enum_time << "\n\n";
}

void StrSTUNSynthesizer::synthesisGoalGraphPhrase(int firstN, int goalDepth) {
    std::cerr << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
    if(firstN < 0) {
        std::cerr << "# StrSTUNSynthesizer Build Graph for ALL examples.\n"; 
        assert(this->graphMgr == nullptr);
        this->graphMgr = new GoalGraphManager(true);
        this->graphMgr->setProgramCreator(this->progMgr, ProgramManager::addProgramStatic);
        ValueVector valvec(this->progMgr->outputExamples);
        this->graphMgr->clearAndResetGoal(valvec);
    } else {
        if(this->graphMgr != nullptr) {
            delete this->graphMgr;
            this->graphMgr = nullptr;
            this->progMgr->deleteAllIfThenElsePrograms();
        }
        assert(firstN > 0 && firstN <= this->isNewEgInfo.size());
        int totalTrueCount = 0;
        int prefixValvecCount = 0;
        int firstNCount = 0;
        for(int& isNewEg : this->isNewEgInfo) {
            firstNCount += 1;
            if(isNewEg == true) {
                totalTrueCount++;
                if(firstNCount <= firstN) prefixValvecCount++;
            }
        }
        assert(totalTrueCount == this->progMgr->exampleCount);
        assert(prefixValvecCount > 0 && prefixValvecCount <= this->progMgr->exampleCount);
        
        std::cerr << "# StrSTUNSynthesizer Build Graph for #firstN=" << firstN << " #prefixCount=" << prefixValvecCount << "\n"; 
        this->graphMgr = new GoalGraphManager(false);
        this->graphMgr->setProgramCreator(this->progMgr, ProgramManager::addProgramStatic);
        ValueVector valvec;
        
        for(int i = 0; i < prefixValvecCount; i++) valvec.values.push_back(this->progMgr->outputExamples[i]);
        this->graphMgr->clearAndResetGoal(valvec);
    }
    //construct goalgraph
    std::chrono::high_resolution_clock::time_point ggt1 = std::chrono::high_resolution_clock::now();
    if (this->progMgr->outputExampleType == ValType::INT) {
        this->graphMgr->buildBottomValueGoals(&this->progMgr->intPrograms);
    } else if (this->progMgr->outputExampleType == ValType::STRING) {
        this->graphMgr->buildBottomValueGoals(&this->progMgr->strPrograms);
    } else {
        std::cerr << "!!! StrSTUNSynthesizer Doesn't support BOOL output.\n";
        assert(false); //Don't support boolean output for now!
    }
    this->graphMgr->buildBottomCondGoals(&this->progMgr->boolPrograms);
    this->graphMgr->buildCondGraph(goalDepth);
    std::chrono::high_resolution_clock::time_point ggt2 = std::chrono::high_resolution_clock::now();
    //print timing
    std::chrono::duration<double> ggspan = ggt2 - ggt1;
    double gg_time = ggspan.count();
    std::cerr << "# StrSTUNSynthesizer::synthesis GG TIME: " << gg_time << "\n\n";
}

void StrSTUNSynthesizer::synthesis(int goalDepth) {
    std::cerr << "# SYNTHESIS for goalDepth: " << goalDepth << "\n";
    this->synthesisEnumerationPhrase();
    this->synthesisGoalGraphPhrase(-1, goalDepth);
}

void StrSTUNSynthesizer::synthesis_without_counting() {
    std::cerr << "# SYNTHESIS_WITHOUT_COUNTING enumeration is not leveled...\n";
    this->synthesisEnumerationPhrase();

    std::cerr << "# SYNTHESIS_WITHOUT_COUNTING try goal graph 1...\n";
    this->synthesisGoalGraphPhrase(-1, 1);
    if(this->graphMgr->getGoalRootResultProgram() == nullptr) {
        std::cerr << "# SYNTHESIS_WITHOUT_COUNTING try goal graph 2...\n";
        delete this->graphMgr; //release resources
        this->graphMgr = nullptr;
        this->progMgr->deleteAllIfThenElsePrograms(); //release resources
        this->synthesisGoalGraphPhrase(-1, 2);
    }
}

Program* StrSTUNSynthesizer::getResultProgram() {
    return this->graphMgr->getGoalRootResultProgram();
}

std::string StrSTUNSynthesizer::serializeProgram(Program* prog) {
    ComponentManager* cmgr = this->compMgr;
    ProgramCodeGen codeGen(
        "SYNFUNC",
        prog,
        this->progMgr->inputExampleTypes,
        this->progMgr->outputExampleType,
        cmgr->getComponentsByIds(cmgr->getInputComponentIds()));
    std::string code = codeGen.getOneStringResult();
    return code;
}