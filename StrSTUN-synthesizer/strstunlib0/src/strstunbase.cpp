#include "strstunbase.hpp"

bool ObjBase::isVerbose = false;
bool ObjBase::isLeveledObsEq = true;

void DebugPrint::printIntVec(const char* name, std::vector<int>& intvec) {
    std::cerr << "[" << name << " size:" << intvec.size() << " (";
    for(int i = 0; i < intvec.size(); i++) {
        std::cerr << "[" << i << "]:" << intvec[i] << ", ";
    }
    std::cerr << ")]";
}

void DebugPrint::printBigIntVec(const char* name, std::vector<mpz_class>& bintvec) {
    std::cerr << "[" << name << " size:" << bintvec.size() << " (";
    for(int i = 0; i < bintvec.size(); i++) {
        std::cerr << "[" << i << "]:" << bintvec[i] << ", ";
    }
    std::cerr << ")]";
}
