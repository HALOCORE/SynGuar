#ifndef GOAL_GRAPH_MANAGER
#define GOAL_GRAPH_MANAGER

#include "strstunbase.hpp"
#include "Goal.hpp"


class GoalGraphManager : ObjBase {
public:
    GoalGraphManager(bool isFullMatch);
    ~GoalGraphManager();

    void clearAndResetGoal(ValueVector targetValues);
    void buildBottomValueGoals(std::vector<std::tuple<ValueVector*, std::vector<Program*>*>>* valueVecMapping);
    void buildBottomCondGoals(std::vector<std::tuple<ValueVector*, std::vector<Program*>*>>* condValueVecMapping);
    mpz_class calcHSizeUpperBound(int maxCond);
    void buildCondGraph(int goalDepth);

    bool getIsGoalRootSolved();
    Program* getGoalRootResultProgram();
    void accept(Visitor* visitor) override;

    //----- Create Branching Program
    void setProgramCreator(
        ObjBase* createProgram1stParam,
        Program* (*pCreateProgram)(ObjBase* createProgram1stParam, std::string mComponentId, std::vector<Program*> mInputPrograms));

    ObjBase* createProgram1stParam;
    Program* (*pCreateProgram)(ObjBase* createProgram1stParam, std::string mComponentId, std::vector<Program*> mInputPrograms);
    static Program* branchProgramCreate(ObjBase* creator, ValType vt, Program* condProg, Program* thenProg, Program* elseProg);

private:
    ValueVector targetValues;
    std::vector<int> anyMatchVector;
    int anyMatchCount;
    int firstN;
    int updateAnyMatchVector(std::vector<int>& boolMatchVec);
    mpz_class term_count_upper;
    mpz_class cond_count_upper;
    mpz_class prog_count_upper;
    mpz_class prog_count_aftersynth;

    GoalUnion* goalRoot;
    std::vector<GoalConcrete*> concreteGoals;
    std::vector<GoalConcrete*> concreteCondGoals;
    std::vector<GoalUnion*> unionGoals;
    std::unordered_map<IntVecP, GoalUnion*, IntVecKeyPHashFunc> unionGoalsHash;
    GoalUnion* getOrCreateUnionGoals(std::vector<int>& expectedMatchVec);
};

#endif