#include "GoalGraphManager.hpp"
#include "visitorbase.hpp"
#include "VisitorPretty.hpp"
#include "ValueVector.hpp"

GoalGraphManager::GoalGraphManager(bool isFullMatch) : goalRoot(), unionGoalsHash(131072) {
    //this->pAllGoals.push_back(&goalRoot);
    this->createProgram1stParam = nullptr;
    this->pCreateProgram = nullptr;
    if(isFullMatch) this->firstN = -1;
    else this->firstN = 0;
}

GoalGraphManager::~GoalGraphManager() {
    std::cerr << "# GoalGraphManager deleted.\n";
    for(GoalUnion* p : this->unionGoals) {
        delete p;
    }
    for(GoalConcrete* p : this->concreteGoals) {
        delete p;
    }
    for(GoalConcrete* p : this->concreteCondGoals) {
        delete p;
    }
}

void GoalGraphManager::setProgramCreator(
    ObjBase* createProgram1stParam,
    Program* (*pCreateProgram)(ObjBase* createProgram1stParam, std::string mComponentId, std::vector<Program*> mInputPrograms)) {
    this->createProgram1stParam = createProgram1stParam;
    this->pCreateProgram = pCreateProgram;
}

///////////// this is the static branchProgramCreate used by Goal
Program* GoalGraphManager::branchProgramCreate(ObjBase* callObj, ValType vt, Program* condProg, Program* thenProg, Program* elseProg) {
    GoalGraphManager* ggm = reinterpret_cast<GoalGraphManager*>(callObj);
    std::string compId = ComponentIfThenElse::componentIdByValType[vt];
    std::vector<Program*> inputPrograms;
    inputPrograms.push_back(condProg);
    inputPrograms.push_back(thenProg);
    inputPrograms.push_back(elseProg);
    Program* resultProg = ggm->pCreateProgram(ggm->createProgram1stParam, compId, inputPrograms);
    assert(resultProg != nullptr);
    return resultProg;
}


void GoalGraphManager::buildBottomValueGoals(std::vector<std::tuple<ValueVector*, std::vector<Program*>*>>* valueVecMapping) {
    std::cerr << "# GoalGraphManager::buildBottomValueGoals.\n";
    mpz_class total_value_term = 0;
    int assert_appended_p = 0;
    for(auto p : *valueVecMapping) {
        //check all value vectors against target values
        //maintain a vector of concrete goals
        auto valvec = std::get<0>(p);
        std::vector<int> matchingVec = ValueVector::getConcreteMatchingVector(this->targetValues, *valvec, this->firstN);
        //VisitorPretty::common->visit(valvec);
        bool anyMatchingGoal = false;
        for(auto & g : this->concreteGoals) {
            assert(g->isConcrete);
            if(g->isBooleanVecEqual(matchingVec)) {
                anyMatchingGoal = true;
                g->addMatchingValVecNoCheck(p);
                assert_appended_p++;
                break;
            }
        }
        if(!anyMatchingGoal) {
            //all concrete goals are solved
            GoalConcrete* newGoal = new GoalConcrete(matchingVec, p);
            assert_appended_p++;
            this->concreteGoals.push_back(newGoal);
            this->updateAnyMatchVector(matchingVec);
        }
    }
    assert(assert_appended_p == valueVecMapping->size());
    for(auto& g : this->concreteGoals) total_value_term += g->solutionCount;
    this->term_count_upper = total_value_term;
    std::cerr << "# GoalGraphManager::buildBottomValueGoals done: #concrete=" << this->concreteGoals.size() << " totalTerm=" << total_value_term << "\n";
    VisitorPretty::common->printIntVec(this->anyMatchVector);
}

void GoalGraphManager::buildBottomCondGoals(std::vector<std::tuple<ValueVector*, std::vector<Program*>*>>* condValueVecMapping) {
    std::cerr << "# GoalGraphManager::buildBottomCondGoals.\n";
    mpz_class total_cond = 0;
    for(auto p : *condValueVecMapping) {
        //check all cond vectors
        //maintain a vector of concrete cond goals
        auto condvec = ValueVector::convertToMatchingVector(*std::get<0>(p), this->firstN);
        bool anyMatchingGoal = false;
        for(auto & g : this->concreteCondGoals) {
            assert(g->isConcrete);
            if(g->isBooleanVecEqual(condvec)) {
                anyMatchingGoal = true;
                g->addMatchingValVecNoCheck(p);
                break;
            }
        }
        if(!anyMatchingGoal) {
            GoalConcrete* newGoal = new GoalConcrete(condvec, p);
            this->concreteCondGoals.push_back(newGoal);
        }
    }
    for(auto& g : this->concreteCondGoals) total_cond += g->solutionCount;
    this->cond_count_upper = total_cond;
    std::cerr << "# GoalGraphManager::buildBottomCondGoals done: #concrete=" << this->concreteCondGoals.size() << " totalCond=" << total_cond << "\n";
}

void GoalGraphManager::buildCondGraph(int goalDepth) {
    //TODO
    std::cerr << "# GoalGraphManager::buildCondGraph. Fill root concrete...\n";
    for(auto& cg : this->concreteGoals) {
        if(cg->isGoodWithExpectedVec(this->goalRoot->boolMathingVec)) {
            EVENT_COUNT("GRootDirect");
            this->goalRoot->addConcreteGoalNoCheck(cg);
        }
    }
    assert(goalDepth > 0);
    std::cerr << "# GoalGraphManager::buildCondGraph. Expanding root...\n";
    std::vector<GoalUnion*> toExpand;
    for(auto& cg : this->concreteCondGoals) {
        auto p = this->goalRoot->getTEGoalsForConcreteCond(cg);
        std::vector<int>& thenEx = std::get<0>(p);
        std::vector<int>& elseEx = std::get<1>(p);
        if(thenEx.size() == 0) {
            //this condition cannot use.
            assert(elseEx.size() == 0);
            continue;
        }
        GoalUnion* thenGoal = this->getOrCreateUnionGoals(thenEx);
        if(thenGoal->concreteGoals.size() == 0) {
            //then union goal don't have concrete implementation.
            EVENT_COUNT("ThSk");
            continue;
        }
        GoalUnion* elseGoal = this->getOrCreateUnionGoals(elseEx);
        this->goalRoot->addBranchGoalsNoCheck(cg, thenGoal, elseGoal);
        EVENT_COUNT("GRSpt");
        toExpand.push_back(elseGoal);
    }
    this->goalRoot->isExpanded = true;
    if(goalDepth > 1) {
        assert(goalDepth == 2);
        std::cerr << "# GoalGraphManager::buildCondGraph. Expanding second level...\n";
        for(auto& xd : toExpand) {
            if(!xd->isExpanded) {
                for(auto& cg : this->concreteCondGoals) {
                    auto p = xd->getTEGoalsForConcreteCond(cg);
                    std::vector<int>& thenEx = std::get<0>(p);
                    std::vector<int>& elseEx = std::get<1>(p);
                    if(thenEx.size() == 0) {
                        //this condition cannot use.
                        assert(elseEx.size() == 0);
                        continue;
                    }
                    GoalUnion* thenGoal = this->getOrCreateUnionGoals(thenEx);
                    if(thenGoal->concreteGoals.size() == 0) {
                        //then union goal don't have concrete implementation.
                        EVENT_COUNT("ThSk");
                        continue;
                    }
                    GoalUnion* elseGoal = this->getOrCreateUnionGoals(elseEx);
                    xd->addBranchGoalsNoCheck(cg, thenGoal, elseGoal);
                    EVENT_COUNT("GRSpt");
                }
                xd->isExpanded = true;
            }
        }
    }
    
    this->calcHSizeUpperBound(goalDepth);
    std::cerr << "# GoalGraphManager::buildCondGraph. Upper bound of H: " << this->prog_count_upper << "\n";
    std::cerr << "# GoalGraphManager::buildCondGraph. Expanding first expand [skipped]\n";
    std::cerr << "# GoalGraphManager::buildCondGraph. Count level 0...\n";
    for(auto & ug : this->unionGoals) {
        ug->checkAndCountByLevel(0);
    }
    std::cerr << "# GoalGraphManager::buildCondGraph. Count level 1...\n";
    for(auto & ug : this->unionGoals) {
        ug->checkAndCountByLevel(1);
    }
    if(goalDepth > 1) {
        assert(goalDepth == 2);
        std::cerr << "# GoalGraphManager::buildCondGraph. Count level 2...\n";
        for(auto & ug : this->unionGoals) {
            ug->checkAndCountByLevel(2);
        }
    }
    this->goalRoot->ensureResultProgram(this, GoalGraphManager::branchProgramCreate);
    std::cerr << "# GoalGraphManager::buildCondGraph. RootGoal Solution Count:\n";
    this->goalRoot->debugPrint();
    std::cerr << "\n";
    this->prog_count_aftersynth = this->goalRoot->getTotalSolutionCount();
    std::cerr << "# GoalGraphManager::buildCondGraph. Upper bound of H after synth: " << this->prog_count_aftersynth << "\n\n";
}

mpz_class GoalGraphManager::calcHSizeUpperBound(int maxCond) {
    mpz_class totalH = 1;
    for(int i = 0; i < maxCond; i++) {
        totalH *= this->cond_count_upper - i;
        totalH *= this->term_count_upper;
    }
    totalH *= this->term_count_upper;
    this->prog_count_upper = totalH;
    return totalH;
}


GoalUnion* GoalGraphManager::getOrCreateUnionGoals(std::vector<int>& expectedMatchVec){
    //#define GU_VECTOR_DEDUP
    #define GU_HASHMAP_DEDUP

    #ifdef GU_VECTOR_DEDUP
    for(auto ug : this->unionGoals) {
        if(ug->isBooleanVecEqual(expectedMatchVec)) {
            return ug;
        }
    }
    #else
    #ifdef GU_HASHMAP_DEDUP
    IntVecP vp; vp.intvecp = &expectedMatchVec;
    if(this->unionGoalsHash.find(vp) != this->unionGoalsHash.end()) {
        return this->unionGoalsHash.at(vp);
    }
    #else
    GoalUnion* gu1 = nullptr;
    GoalUnion* gu2 = nullptr;
    for(auto ug : this->unionGoals) {
        if(ug->isBooleanVecEqual(expectedMatchVec)) {
            gu1 = ug;
        }
    }
    if(this->unionGoalsHash.find(expectedMatchVec) != this->unionGoalsHash.end()) {
        gu2 = this->unionGoalsHash.at(expectedMatchVec);
    }
    assert(gu1 == gu2);
    if(gu1 != nullptr) return gu1;
    #endif 
    #endif

    //otherwize, create new union goal, from concrete.
    GoalUnion* newGU = new GoalUnion(expectedMatchVec);
    for(auto& cg : this->concreteGoals) {
        if(cg->isGoodWithExpectedVec(expectedMatchVec)) {
            newGU->addConcreteGoalNoCheck(cg);
        }
    }
    this->unionGoals.push_back(newGU);
    IntVecP vp2; vp2.intvecp = new std::vector<int>(expectedMatchVec);
    this->unionGoalsHash.insert(std::make_pair(vp2, newGU));
    //std::cerr << "N";
    return newGU;
}


void GoalGraphManager::clearAndResetGoal(ValueVector targetValues) {
    std::cerr << "GoalGraphManager::clearAndResetGoal\n"; 
    this->targetValues = targetValues;
    this->anyMatchVector.clear();
    this->anyMatchCount = 0;
    std::vector<int> vec;
    for(int i = 0; i < targetValues.values.size(); i++) {
        this->anyMatchVector.push_back(0);
        vec.push_back(1);
    }
    assert(vec.size() > 0);
    if(this->firstN == 0) this->firstN = vec.size();
    this->goalRoot = new GoalUnion(vec);
    this->concreteGoals.clear();
    this->concreteCondGoals.clear();
    this->unionGoals.clear();
    this->unionGoalsHash.clear();
    this->unionGoals.push_back(this->goalRoot); //root goal is union goal.
    IntVecP vp; vp.intvecp = new std::vector<int>(this->goalRoot->boolMathingVec);
    this->unionGoalsHash.insert(std::make_pair(vp, this->goalRoot));
    
    this->cond_count_upper = 0;
    this->term_count_upper = 0;
    this->prog_count_upper = 0;

    assert(!this->getIsGoalRootSolved());
}

bool GoalGraphManager::getIsGoalRootSolved() {
    Program* result = this->goalRoot->resultProgram;
    if(result == nullptr) return false;
    return true;
}

Program* GoalGraphManager::getGoalRootResultProgram() {
    Program* result = this->goalRoot->resultProgram;
    return result;
}

int GoalGraphManager::updateAnyMatchVector(std::vector<int>& boolMatchVec) {
    int updatedMatchCount = 0;
    for(int i = 0; i < boolMatchVec.size(); i++) {
        if(boolMatchVec[i] == 1) {
            anyMatchVector[i] += 1;
            updatedMatchCount += 1;
        } else {
            assert(boolMatchVec[i] == 0);
            if(anyMatchVector[i] > 0){
                updatedMatchCount += 1;
            }
        }
    }
    this->anyMatchCount = updatedMatchCount;
    return updatedMatchCount;
}

void GoalGraphManager::accept(Visitor* visitor) {
    //visitor->visit(&this->goalRoot);
    std::cerr << "GoalGraphManager::accept not implemented.\n"; 
}