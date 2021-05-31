#include "PerfCounter.hpp"
#include <cassert>

std::unordered_map<std::string, int>* PerfCounter::callCount;
std::unordered_map<std::string, int>* PerfCounter::levelCurrent;
std::unordered_map<std::string, std::unordered_map<int, int>*>* PerfCounter::leveledCallCount;

void PerfCounter::initialize() {
    std::unordered_map<std::string, int> count;
    PerfCounter::callCount = new std::unordered_map<std::string, int>();
    PerfCounter::levelCurrent = new std::unordered_map<std::string, int>();
    PerfCounter::leveledCallCount = new std::unordered_map<std::string, std::unordered_map<int, int>*>();
}

void PerfCounter::count(std::string funcName) {
    if(PerfCounter::callCount->find(funcName) == PerfCounter::callCount->end()) {
        (*PerfCounter::callCount)[funcName] = 0;
    }
    (*PerfCounter::callCount)[funcName]++;
}

void PerfCounter::levelSet(std::string funcName, int level) {
    (*PerfCounter::levelCurrent)[funcName] = level;
}


void PerfCounter::leveledCount(std::string funcName) {
    if(PerfCounter::leveledCallCount->find(funcName) == PerfCounter::leveledCallCount->end()) {
        (*PerfCounter::leveledCallCount)[funcName] = new std::unordered_map<int, int>();
    }
    assert(PerfCounter::levelCurrent->find(funcName) != PerfCounter::levelCurrent->end());
    int level = (*PerfCounter::levelCurrent)[funcName];
    std::unordered_map<int, int>* countMap = (*PerfCounter::leveledCallCount)[funcName];
    if(countMap->find(level) == countMap->end()) {
        (*countMap)[level] = 0;
    }
    (*countMap)[level]++;
}

void PerfCounter::leveledSetValue(std::string funcName, int value) {
    if(PerfCounter::leveledCallCount->find(funcName) == PerfCounter::leveledCallCount->end()) {
        (*PerfCounter::leveledCallCount)[funcName] = new std::unordered_map<int, int>();
    }
    assert(PerfCounter::levelCurrent->find(funcName) != PerfCounter::levelCurrent->end());
    int level = (*PerfCounter::levelCurrent)[funcName];
    std::unordered_map<int, int>* countMap = (*PerfCounter::leveledCallCount)[funcName];
    (*countMap)[level] = value;
}

void PerfCounter::print(std::ostream& out) {
    out << "[synthesizer] PerfCount: ";
    for (auto pair : (*PerfCounter::callCount)) {
        out << "{" << pair.first << ": " << pair.second << "} ";
    }
    out << std::endl <<"[synthesizer] PerfCountLeveled: ";
    for (auto pair : (*PerfCounter::leveledCallCount)) {
        out << "{" << pair.first << ": ";
        for(auto p2 : (*pair.second)) {
            out << p2.first <<"=" << p2.second << " ";
        }
        out << "} ";
    }
    out << std::endl;
}