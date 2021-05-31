#ifndef PROGRAM
#define PROGRAM

#include "strstunbase.hpp"
#include "strstunvalues.hpp"
#include "ValueVector.hpp"

class Program : public ObjBase {
public:
    std::string componentId;

    std::vector<ValType> inputTypes;
    std::vector<Program*> inputPrograms;

    ValType outputType;
    ValueVector outputValue;
    //std::unordered_map<std::string, int> outputFlags;

    int syntaxSize;
    int syntaxDepth;
    mpz_class obsEqCount;
    
public:
    Program(
        std::string mComponentId,
        std::vector<ValType> mInputTypes,
        std::vector<Program*> mInputPrograms,
        ValType mOutputType,
        ValueVector mOutputValue); //std::unordered_map<std::string, int> mOutputFlags
    void accept(Visitor* visitor) override;
    void printStructure(std::ostream& out);
};

#endif