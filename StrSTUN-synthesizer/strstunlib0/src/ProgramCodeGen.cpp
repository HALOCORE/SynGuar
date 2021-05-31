#include "ProgramCodeGen.hpp"

ProgramCodeGen::ProgramCodeGen(std::string targetFuncName, Program* prog, std::vector<ValType> inputTypes, ValType outputType, std::vector<ComponentBase*> inputComponents) {
    this->currentIndent = 0;
    this->result.push_back(this->getSignature(targetFuncName, prog, inputTypes, outputType));
    std::string headline = targetFuncName + " ";
    for (auto comp : inputComponents)
        headline += comp->getComponentId() + " ";
    headline += "= ";
    this->result.push_back(headline);
    this->programGen(prog);
    this->result[this->result.size() - 1] = this->result.back() + ";";
}

std::vector<std::string> ProgramCodeGen::getResult() {
    return this->result;    
}

std::string ProgramCodeGen::getOneStringResult() {
    std::string oneResult = "";
    for (std::string s : this->result) {
        oneResult += s + "\n";
    }
    return oneResult;
}

std::string ProgramCodeGen::getSignature(std::string targetFuncName, Program* prog, std::vector<ValType> inputTypes, ValType outputType) {
    std::string sig = targetFuncName + " :: ";
    auto valTypeToStrType = [](ValType vt) {
        if (vt == ValType::BOOL) return "Bool";
        else if (vt == ValType::INT) return "Int";
        else if (vt == ValType::STRING) return "Str";
        assert(false);
    };
    for (auto& inType : inputTypes) {
        sig += std::string(valTypeToStrType(inType)) + " -> ";
    }
    sig += "(" + std::string(valTypeToStrType(outputType)) + ");";
    return sig;
}

void ProgramCodeGen::programGen(Program* prog) {
    std::string compId = prog->componentId;
    ValType vtype = prog->outputType;
    if (compId == ComponentIfThenElse::componentIdByValType[vtype]) {
        this->ifThenElseProgramGen(
            prog->inputPrograms[0],
            prog->inputPrograms[1],
            prog->inputPrograms[2]);
    } else {
        this->normalProgramGen(compId, prog->inputPrograms);
    }
}

void ProgramCodeGen::ifThenElseProgramGen(Program* condp, Program* thenp, Program* elsep) {
    this->indentInc();
    this->result.push_back(this->getIndentSpaces() + "if ");
    this->indentInc();
    this->programGen(condp);
    this->result.push_back(this->getIndentSpaces() + "then ");
    this->programGen(thenp);
    this->result.push_back(this->getIndentSpaces() + "else ");
    this->programGen(elsep);
}

void ProgramCodeGen::normalProgramGen(std::string compId, std::vector<Program*> mInputPrograms) {
    std::string lastString = this->result.back();
    if (mInputPrograms.size() == 0) {
        lastString += compId;
        this->result[this->result.size() - 1] = lastString;
    } else {
        lastString += "(" + compId;
        this->result[this->result.size() - 1] = lastString;
        for (Program* p : mInputPrograms) {
            lastString = this->result.back();
            lastString += " ";
            this->result[this->result.size() - 1] = lastString;
            this->programGen(p);
        } 
        lastString = this->result.back();
        lastString += ")";
        this->result[this->result.size() - 1] = lastString;
    }
}

void ProgramCodeGen::indentInc() {
    this->currentIndent += 2;
}

void ProgramCodeGen::indentDec() {
    this->currentIndent -= 2;
}

std::string ProgramCodeGen::getIndentSpaces() {
    return std::string(this->currentIndent, ' ');
}

void ProgramCodeGen::accept(Visitor* visitor) {
    return;
}