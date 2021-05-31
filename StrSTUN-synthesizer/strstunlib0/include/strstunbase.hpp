#ifndef STRECBASE
#define STRECBASE
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <gmpxx.h>
#include "PerfCounter.hpp"

#define MATCHVEC_ERROR -9
#define MATCHVEC_TRUE 1
#define MATCHVEC_FALSE 0
#define MATCHVEC_DONTCARE -1

#define BEGIN_FUNC() PerfCounter::count(__func__)

#define EVENT_COUNT(x) PerfCounter::count(x)

#define LEVELED_COUNT(x) PerfCounter::leveledCount(x)

#define LEVELED_SETVAL(x, y) PerfCounter::leveledSetValue(x, y)

#define LEVEL_SET(x, y) PerfCounter::levelSet(x, y)

class Visitor;

class DebugPrint {
public:
    static void printIntVec(const char* name, std::vector<int>& intvec);
    static void printBigIntVec(const char* name, std::vector<mpz_class>& bintvec);
};

class ObjBase {
public:
    virtual void accept(Visitor *visitor) = 0;
    static bool isVerbose;
    static bool isLeveledObsEq;
};

#endif