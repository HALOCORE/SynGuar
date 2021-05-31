#ifndef PERF_COUNTER
#define PERF_COUNTER

#include <iostream>
#include <unordered_map>
#include <string>

class PerfCounter {
public:
    static void initialize();
    static void print(std::ostream& out);
    static std::unordered_map<std::string, int>* callCount;
    static std::unordered_map<std::string, int>* levelCurrent;
    static std::unordered_map<std::string, std::unordered_map<int, int>*>* leveledCallCount;
    static void count(std::string funcName);
    static void levelSet(std::string funcName, int level);
    static void leveledCount(std::string funcName);
    static void leveledSetValue(std::string funcName, int value);
};

#endif