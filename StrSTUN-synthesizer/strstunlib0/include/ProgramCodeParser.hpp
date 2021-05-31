#ifndef PROGRAM_CODEGEN
#define PROGRAM_CODEGEN

#include "strstunbase.hpp"
#include "Program.hpp"
#include "componentbase.hpp"
#include "ComponentIfThenElse.hpp"


class ProgramCodeParser : ObjBase {
public:
    ProgramCodeParser(std::string code);
    Program* getResult();
    
    void accept(Visitor*) override;    
private:
    std::vector<std::string> tokens;
    std::string code;
    void tokenize();
    void parse();
};

#endif