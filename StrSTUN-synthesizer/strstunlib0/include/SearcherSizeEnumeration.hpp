#ifndef SEARCHER_SIZE_ENUMERATION
#define SEARCHER_SIZE_ENUMERATION

#include "strstunbase.hpp"
#include "Program.hpp"
#include "componentbase.hpp"
#include<algorithm>

class SearcherSizeEnumeration : public ObjBase {
public: 
    SearcherSizeEnumeration(
        ObjBase* environment,
        std::vector<ComponentBase*> applyComponents,
        std::vector<Program*> size0Programs,
        Program* (*pCreateAndCheckProgram)(ObjBase* env, ComponentBase* mComponent, std::vector<Program*> mInputPrograms),
        int (*pGetPreScore)(Program* prog));

    void enumerateToSize(int size);
    void step();
    void accept(Visitor* visitor) override;

private:
    void stepRec(ComponentBase* comp, int paramIndex, int sizeBudget);
    void putProgramByCurrentSize(Program* prog);
    std::vector<std::tuple<int, Program*>>* getProgramsBySizeAndValType(
        ValType valType, int size);
    
    ObjBase* environment;
    std::vector<ComponentBase*> applyingComponents;
    Program* (*pCreateAndCheckProgram)(
        ObjBase* env, 
        ComponentBase* mComponent,
        std::vector<Program*> mInputPrograms);
    int (*pGetPreScore)(Program* prog);

    std::vector<std::vector<std::tuple<int, Program*>>*> progsBySizeBool;
    std::vector<std::vector<std::tuple<int, Program*>>*> progsBySizeInt;
    std::vector<std::vector<std::tuple<int, Program*>>*> progsBySizeStr;
    int currentSize;
    int currentComponentIndex;
    std::vector<Program*> currentProgramParams;
};

#endif