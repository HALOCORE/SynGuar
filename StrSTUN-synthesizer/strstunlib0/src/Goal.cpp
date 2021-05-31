#include "Goal.hpp"
#include "visitorbase.hpp"

/////////////// Goal

Goal::Goal(std::vector<int>& boolMathingVec, bool isConcrete) : boolMathingVec(boolMathingVec) {
    this->resultProgram = nullptr;
    this->isConcrete = isConcrete;
    // LEVEL_SET("#GoalByDepth", this->depth);
    // LEVELED_COUNT("#GoalByDepth");
}

void Goal::accept(Visitor* visitor) {
    std::cerr << "Goal::accept not implemented.\n";
}

bool Goal::isBooleanVecEqual(std::vector<int>& vec) {
    if(this->boolMathingVec == vec) return true;
    return false;
}

mpz_class Goal::valVecCount(std::tuple<ValueVector*, std::vector<Program*>*>& valvec) {
    auto progvec = std::get<1>(valvec);
    mpz_class result = 0;
    for(auto p : *progvec) {
        result += p->obsEqCount;
    }
    return result;
}



/////////////// GoalConcrete

GoalConcrete::GoalConcrete(std::vector<int>& boolMathingVec, std::tuple<ValueVector *, std::vector<Program *> *> firstReal) : Goal(boolMathingVec, true) {
    BEGIN_FUNC();
    this->isSolved = true;
    auto& progVec = std::get<1>(firstReal);
    this->resultProgram = progVec->at(0);
    this->matchingValueVecs.push_back(firstReal);
    this->solutionCount = Goal::valVecCount(firstReal);
}

void GoalConcrete::addMatchingValVecNoCheck(std::tuple<ValueVector*, std::vector<Program*>*> matchingVecProg) {
    this->matchingValueVecs.push_back(matchingVecProg);
    this->solutionCount += Goal::valVecCount(matchingVecProg);
}

bool GoalConcrete::isGoodWithExpectedVec(std::vector<int>& expectedVec) {
    assert(expectedVec.size() == this->boolMathingVec.size());
    bool anyMismatch = false;
    for(int i = 0; i < expectedVec.size(); i++) {
        if(expectedVec[i] == MATCHVEC_DONTCARE) continue;
        if(expectedVec[i] == MATCHVEC_TRUE) {
            if(this->boolMathingVec[i] != MATCHVEC_TRUE) {
                anyMismatch = true;
                break;
            }
        }
        else assert(false); //assume only for valvec, not condvec
    }
    return !anyMismatch;
}

void GoalConcrete::debugPrint() {
    std::cerr << " [GoalConcrete isSolved=" << this->isSolved << " solutionCount=" << this->solutionCount << " ";
    DebugPrint::printIntVec("MatchingVec", this->boolMathingVec);
    std::cerr << "]";
}


/////////////// GoalUnion

GoalUnion::GoalUnion(std::vector<int>& boolMathingVec) : Goal(boolMathingVec, false) {
    BEGIN_FUNC();
    this->isSolvedByLevel.clear();
    this->solutionCountByLevel.clear();
    this->resultProgram = nullptr;
    this->isExpanded = false;
    this->branchImplIndex = -1;
}

bool GoalUnion::isGoalConcreteMatch(Goal* concreteGoal) {
    //assume matched goals are all for value, not for cond.
    std::vector<int>& concreteSubVec = concreteGoal->boolMathingVec;
    assert(concreteSubVec.size() == this->boolMathingVec.size());
    bool anyMismatch = false;
    for(int i = 0; i < concreteSubVec.size(); i++) {
        if(this->boolMathingVec[i] == MATCHVEC_DONTCARE) continue;
        assert(this->boolMathingVec[i] == MATCHVEC_TRUE);
        if(concreteSubVec[i] == MATCHVEC_FALSE) {
            anyMismatch = true; 
            break;
        }
    }
    if(anyMismatch) return false;
    return true;
}

std::tuple<std::vector<int>, std::vector<int>> GoalUnion::getTEGoalsForConcreteCond(GoalConcrete* condGoal) {
    std::vector<int> thenExpected;
    std::vector<int> elseExpected;
    assert(this->boolMathingVec.size() == condGoal->boolMathingVec.size());
    for(int i = 0; i < this->boolMathingVec.size(); i++) {
        if(this->boolMathingVec[i] == MATCHVEC_TRUE) {
            if(condGoal->boolMathingVec[i] == MATCHVEC_TRUE) {
                thenExpected.push_back(MATCHVEC_TRUE);
                elseExpected.push_back(MATCHVEC_DONTCARE);
            } else if(condGoal->boolMathingVec[i] == MATCHVEC_FALSE) {
                thenExpected.push_back(MATCHVEC_DONTCARE);
                elseExpected.push_back(MATCHVEC_TRUE);
            } else if(condGoal->boolMathingVec[i] == MATCHVEC_ERROR) {
                //if the goal have error on required position, cannot use.
                thenExpected.clear();
                elseExpected.clear();
                auto p = std::make_tuple(thenExpected, elseExpected);
                return p;
            } else {
                assert(false);
            }
        } else {
            assert(this->boolMathingVec[i] == MATCHVEC_DONTCARE);
            thenExpected.push_back(MATCHVEC_DONTCARE);
            elseExpected.push_back(MATCHVEC_DONTCARE);
        }
    }
    auto p = std::make_tuple(thenExpected, elseExpected);
    return p;
}

void GoalUnion::addConcreteGoalNoCheck(GoalConcrete* concreteGoal) {
    assert(concreteGoal->isSolved);
    this->concreteGoals.push_back(concreteGoal);
}

void GoalUnion::addBranchGoalsNoCheck(GoalConcrete* condGoal, GoalUnion* goalThen, GoalUnion* goalElse) {
    assert(this->isExpanded == false);
    assert(condGoal->isSolved);
    this->branchGoals.push_back(std::make_tuple(condGoal, goalThen, goalElse));
}

bool GoalUnion::ensureResultProgram(ObjBase* createObj, Program* (*branchProgramCreate)(ObjBase*, ValType, Program*, Program*, Program*)) {
    if(this->resultProgram != nullptr) return this->resultProgram;
    if(this->getTotalSolutionCount() == 0) return false;
    assert(this->isSolvedByLevel[0] == false);
    if (this->branchImplIndex < 0) {
        std::cerr << "# CRITICAL ERROR: ensure result program have negative index: " << this->branchImplIndex << std::endl;
        return false;
    }
    
    auto& p = this->branchGoals[this->branchImplIndex];
    GoalConcrete* gcond = std::get<0>(p);
    GoalUnion* gthen = std::get<1>(p);
    GoalUnion* gelse = std::get<2>(p);
    assert(gthen != this || gelse != this);
    bool isThenEnsured = gthen->ensureResultProgram(createObj, branchProgramCreate);
    bool isElseEnsured = gelse->ensureResultProgram(createObj, branchProgramCreate);
    if(!(isThenEnsured && isElseEnsured)) {
        std::cerr << "FATAL ERROR: ensureResultProgram failed.\n";
        assert(false); 
        return false;
    }
    Program* condProg = gcond->resultProgram;
    Program* thenProg = gthen->resultProgram;
    Program* elseProg = gelse->resultProgram;
    assert(condProg != nullptr && thenProg != nullptr && elseProg != nullptr);
    ValType vt = thenProg->outputType;
    assert(thenProg->outputType == elseProg->outputType);
    Program* bprog = branchProgramCreate(createObj, vt, condProg, thenProg, elseProg);
    this->resultProgram = bprog;
    return true;
}

void GoalUnion::checkAndCountByLevel(int level) {
    //Only level 0 direct prog assign is done here.
    assert(this->isSolvedByLevel.size() == level);
    assert(this->solutionCountByLevel.size() == level);
    if(level == 0) {
        //count concrete goals
        assert(this->resultProgram == nullptr);
        mpz_class direct_count = 0;
        for(auto g : this->concreteGoals) {
            assert(g->solutionCount > 0);
            if(this->resultProgram == nullptr) {
                //direct program is assigned directly
                this->resultProgram = g->resultProgram;
            }
            direct_count += g->solutionCount;
        }
        this->solutionCountByLevel.push_back(direct_count);
        if(direct_count > 0) {
            this->isSolvedByLevel.push_back(true);
        } else {
            this->isSolvedByLevel.push_back(false);
        }
    }
    else {
        //count branching
        mpz_class branch_count = 0;
        int branchGoalsIdx = -1;
        for(auto & p : this->branchGoals) {
            branchGoalsIdx++;
            GoalConcrete* gcond = std::get<0>(p);
            GoalUnion* gthen = std::get<1>(p);
            GoalUnion* gelse = std::get<2>(p);
            mpz_class& condCount = gcond->solutionCount;
            mpz_class& thenCount = gthen->solutionCountByLevel[0];
            mpz_class& elseCount = gelse->solutionCountByLevel[level - 1];
            //for level 1, count level 0 for else branch.
            mpz_class case_count = condCount * thenCount * elseCount;
            //note: the original program creation is changed into keep realizing pointer
            if(this->resultProgram == nullptr && this->branchImplIndex < 0 && case_count > 0) {
                this->branchImplIndex = branchGoalsIdx;
            }
            branch_count += case_count;
        }
        this->solutionCountByLevel.push_back(branch_count);
        if(branch_count > 0) {
            this->isSolvedByLevel.push_back(true);
        } else{
            this->isSolvedByLevel.push_back(false);
        }
    }
}

mpz_class GoalUnion::getTotalSolutionCount() {
    mpz_class totalCount = 0;
    assert(this->isSolvedByLevel.size() > 0 && this->solutionCountByLevel.size() == this->isSolvedByLevel.size());
    for(mpz_class& c : this->solutionCountByLevel) {
        totalCount += c;
    }
    return totalCount;
}

void GoalUnion::debugPrint() {
    std::cerr << " [GoalUnion ";
    DebugPrint::printIntVec("MatchingVec", this->boolMathingVec);
    std::cerr << " ";
    DebugPrint::printIntVec("isSolvedByLevel", this->isSolvedByLevel);
    std::cerr << " ";
    DebugPrint::printBigIntVec("solutionCountByLevel", this->solutionCountByLevel);
    std::cerr << "]";
}
