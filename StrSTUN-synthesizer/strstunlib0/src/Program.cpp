#include "Program.hpp"
#include "visitorbase.hpp"

Program::Program(
    std::string mComponentId, 
    std::vector<ValType> mInputTypes,
    std::vector<Program*> mInputPrograms,
    ValType mOutputType,
    ValueVector mOutputValue) :  //std::unordered_map<std::string, int> mOutputFlags
        componentId(mComponentId), 
        inputTypes(mInputTypes),
        inputPrograms(mInputPrograms),
        outputType(mOutputType),
        outputValue(mOutputValue) //outputFlags(mOutputFlags)
    {
    this->syntaxSize = 1;
    this->syntaxDepth = 0;
    this->obsEqCount = 1;
    for (auto p : mInputPrograms) {
        this->syntaxSize += p->syntaxSize;
        this->obsEqCount *= p->obsEqCount;
        if (p->syntaxDepth > this->syntaxDepth) 
            this->syntaxDepth = p->syntaxDepth;
    }
    //assert(this->obsEqCount > 0);
    this->syntaxDepth++;
} 

void Program::accept(Visitor* visitor) {
    visitor->visit(&this->outputValue);
}

void Program::printStructure(std::ostream& out) {
    out << "(" << this->componentId;
    for (auto p : this->inputPrograms) {
        out << " ";
        p->printStructure(out);
    }
    out << ")";
}