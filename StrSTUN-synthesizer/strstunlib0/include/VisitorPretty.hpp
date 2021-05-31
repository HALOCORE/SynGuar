#ifndef VISITOR_PRETTY
#define VISITOR_PRETTY

#include "visitorbase.hpp"

class VisitorPretty : public Visitor {
public:
    VisitorPretty();
    void visit(ValueVector*) override;
    void visit(ValInt*) override;
    void visit(ValBool*) override;
    void visit(ValStr*) override;
    void visit(Program*) override;
    void visit(ProgramManager*) override;
    void visit(ComponentBuiltin*) override;
    void visit(ComponentInput*) override;
    void visit(ComponentIfThenElse*) override;
    void visit(ComponentSynthesized*) override;
    void visit(ComponentManager*) override;
    void visit(Goal*) override;
    void visit(GoalGraphManager*) override;
    static VisitorPretty* common;

public:
    void printIntVec(std::vector<int>& intvec);

private:
    int indent;
    void indentOutput();
    void indentInc();
    void indentDec();
};

#endif