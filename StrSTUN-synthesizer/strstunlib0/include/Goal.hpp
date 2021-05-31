#ifndef GOAL
#define GOAL

#include "strstunbase.hpp"
#include "ValueVector.hpp"
#include "Program.hpp"


// enum GoalCheckResult : int {
//     NO_MATCH,
//     PARTIAL_MATCH_DONT_CARE,
//     PARTIAL_MATCH_VALUE,
//     ALL_MATCH
// };

#define GOAL_DEPTH_UNDEF -9
#define GOAL_DEPTH_BOTTOM -1

class Goal : public ObjBase {
public:
    Goal(std::vector<int>& boolMathingVec, bool isConcrete);
    std::vector<int> boolMathingVec;

    bool isConcrete;
    Program* resultProgram;

    bool isBooleanVecEqual(std::vector<int>& vec);
    void accept(Visitor* visitor) override;

public:
    static mpz_class valVecCount(std::tuple<ValueVector*, std::vector<Program*>*>& valvec);
};


// hashing Goal* begin ========

struct IntVecP {
public:
    std::vector<int>* intvecp;
    bool operator==(const IntVecP &p) const
    {
        return *this->intvecp == (*p.intvecp);
    }
};

struct IntVecKeyPHashFunc
{
    std::size_t operator() (const IntVecP &node) const
    {
        std::size_t h1 = 0;
        for(auto& v : *node.intvecp) {
            h1 = (h1 << 1) ^ std::hash<int>()(v);
        }
        return h1;
    }
};
// hashing Goal* end ========


class GoalConcrete : public Goal {
public:
    GoalConcrete(std::vector<int>& boolMathingVec, std::tuple<ValueVector *, std::vector<Program *> *> firstReal);

    bool isGoodWithExpectedVec(std::vector<int>& expectedVec);
    void addMatchingValVecNoCheck(std::tuple<ValueVector*, std::vector<Program*>*> vec);

    bool isSolved;
    mpz_class solutionCount;
    void debugPrint();
private:
    std::vector<std::tuple<ValueVector*, std::vector<Program*>*>> matchingValueVecs;
};



class GoalUnion : public Goal {
public:
    GoalUnion(std::vector<int>& boolMathingVec);
    bool ensureResultProgram(ObjBase* createObj, Program* (*branchProgramCreate)(ObjBase*, ValType, Program*, Program*, Program*));

    bool isGoalConcreteMatch(Goal* concreteGoal);
    std::tuple<std::vector<int>, std::vector<int>> getTEGoalsForConcreteCond(GoalConcrete* condGoal);

    void addConcreteGoalNoCheck(GoalConcrete* concreteGoal);
    void addBranchGoalsNoCheck(GoalConcrete* condGoal, GoalUnion* goalThen, GoalUnion* goalElse);
    void checkAndCountByLevel(int level);
    mpz_class getTotalSolutionCount();

    std::vector<int> isSolvedByLevel;
    std::vector<mpz_class> solutionCountByLevel;
    bool isExpanded;

    void debugPrint();

    //public it now
    std::vector<GoalConcrete*> concreteGoals;
private:
    std::vector<std::tuple<GoalConcrete*, GoalUnion*, GoalUnion*>> branchGoals;
    int branchImplIndex;
};

#endif