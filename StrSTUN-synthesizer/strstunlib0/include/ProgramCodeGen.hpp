#ifndef PROGRAM_CODEGEN
#define PROGRAM_CODEGEN

#include "strstunbase.hpp"
#include "Program.hpp"
#include "componentbase.hpp"
#include "ComponentIfThenElse.hpp"


class ProgramCodeGen : ObjBase {
public:
    ProgramCodeGen(std::string targetFuncName, Program* prog, std::vector<ValType> inputTypes, ValType outputType, std::vector<ComponentBase*> inputComponents);
    std::vector<std::string> getResult();
    std::string getOneStringResult();
    
    void accept(Visitor*) override;

private:
    int currentIndent;
    std::vector<std::string> result;
    void indentInc();
    void indentDec();
    std::string getIndentSpaces();
    std::string getSignature(std::string targetFuncName, Program* prog, std::vector<ValType> inputTypes, ValType outputType);
    void programGen(Program* prog);
    void ifThenElseProgramGen(Program* condp, Program* thenp, Program* elsep);
    void normalProgramGen(std::string compId, std::vector<Program*> mInputPrograms);
};

#endif