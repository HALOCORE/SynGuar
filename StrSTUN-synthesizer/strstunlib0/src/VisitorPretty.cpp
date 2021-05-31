#include "VisitorPretty.hpp"

VisitorPretty* VisitorPretty::common;

VisitorPretty::VisitorPretty() {
    this->indent = 0;
    VisitorPretty::common = this;
}

void VisitorPretty::indentOutput() {
    for (int i = 0; i < indent; i ++) {
        std::cerr << "  ";
    }
}

void VisitorPretty::indentInc() {
    this->indent += 2;
}

void VisitorPretty::indentDec() {
    this->indent -= 2;
}

void VisitorPretty::visit(ValueVector* valvec) {
    indentOutput();
    std::cerr << "[ValueVector ";
    valvec->accept(this);
    std::cerr << "]" << std::endl;
    return;
}

void VisitorPretty::visit(ValBool* val) {
    if (val->status == ValStatus::VALID) std::cerr << "(Bool " << val->value << ")";
    else std::cerr << "(Bool " << ValStatusRepr[val->status] << ")";
    return;
}

void VisitorPretty::visit(ValInt* val) {
    if (val->status == ValStatus::VALID) std::cerr << "(Int " << val->value << ")";
    else std::cerr << "(Int " << ValStatusRepr[val->status] << ")";
    return;
}

void VisitorPretty::visit(ValStr* val) {
    if (val->status == ValStatus::VALID) std::cerr << "(Str " << val->value << ")";
    else std::cerr << "(Str " << ValStatusRepr[val->status] << ")";
    return;
}

void VisitorPretty::visit(Program* prog) {
    indentOutput();
    std::cerr << "visiting Program: " 
        << "[ " << prog->componentId << " ] ";
    for(auto ty : prog->inputTypes) {
        std::cerr << ", " << ValTypeRepr[ty];
    } 
    std::cerr << " -> " << ValTypeRepr[prog->outputType] << std::endl;
    indentInc();
    prog->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ProgramManager* pmgr) {
    indentOutput();
    std::cerr << "visiting ProgramManager: " << std::endl;
    indentInc();
    pmgr->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ComponentBuiltin* compbin) {
    indentOutput();
    std::cerr << "visiting ComponentBuiltin: " 
        << compbin->getComponentId() << ", "
        << std::endl;
    indentInc();
    compbin->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ComponentInput* compin) {
    indentOutput();
    std::cerr << "visiting ComponentInput: " 
        << compin->getComponentId() << ", "
        << compin->getParamIndex()
        << std::endl;
    indentInc();
    compin->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ComponentIfThenElse* compin) {
    indentOutput();
    std::cerr << "visiting ComponentIfThenElse: " 
        << compin->getComponentId() << std::endl;
    indentInc();
    compin->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ComponentSynthesized* compin) {
    indentOutput();
    std::cerr << "visiting ComponentSynthesized: " 
        << compin->getComponentId() << std::endl;
    indentInc();
    compin->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(ComponentManager* cmgr) {
    indentOutput();
    std::cerr << "visiting ComponentManager: " << std::endl;
    indentInc();
    cmgr->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(Goal* goal) {
    indentOutput();
    std::cerr << "visiting Goal: IsConcrete=" << goal->isConcrete << std::endl;
    indentInc();
    goal->accept(this);
    indentDec();
    return;
}

void VisitorPretty::visit(GoalGraphManager* gmgr) {
    indentOutput();
    std::cerr << "visiting GoalGraphManager: "
        << "isGoalRootSolved: " << gmgr->getIsGoalRootSolved()
        << std::endl;
    indentInc();
    gmgr->accept(this);
    indentDec();
}

void VisitorPretty::printIntVec(std::vector<int>& intvec) {
    indentOutput();
    std::cerr << "[IntVec size:" << intvec.size() << " (";
    for(int i = 0; i < intvec.size(); i++) {
        std::cerr << "[" << i << "]:" << intvec[i] << ", ";
    }
    std::cerr << ")]\n";
}