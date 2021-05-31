#ifndef VISITORBASE
#define VISITORBASE

#include "strstunvalues.hpp"
#include "ValueVector.hpp"
#include "Program.hpp"
#include "ComponentBuiltin.hpp"
#include "ComponentInput.hpp"
#include "ProgramManager.hpp"
#include "ComponentManager.hpp"
#include "Goal.hpp"
#include "GoalGraphManager.hpp"

class Visitor {
public:
    virtual void visit(ValInt*) = 0;
    virtual void visit(ValBool*) = 0;
    virtual void visit(ValStr*) = 0;
    virtual void visit(ValueVector*) = 0;
    virtual void visit(Program*) = 0;
    virtual void visit(ProgramManager*) = 0;
    virtual void visit(ComponentBuiltin*) = 0;
    virtual void visit(ComponentInput*) = 0;
    virtual void visit(ComponentIfThenElse*) = 0;
    virtual void visit(ComponentSynthesized*) = 0;
    virtual void visit(ComponentManager*) = 0;
    virtual void visit(Goal*) = 0;
    virtual void visit(GoalGraphManager*) = 0;
};

#endif