#include "ProgramsScore.hpp"

ProgramsScore::ProgramsScore() noexcept {
    this->highestScore = INT32_MIN;
    this->programWithHighestScore = nullptr;
}

int ProgramsScore::getHighestScore() {
    assert(this->programWithHighestScore != nullptr);
    return this->highestScore;
}

Program* ProgramsScore::getProgramWithHightestScore() {
    return this->programWithHighestScore;
}

void ProgramsScore::setScore(Program* program, int score) {
    this->scoreMap[program] = score;
    if (this->programWithHighestScore == nullptr || score > this->highestScore) {
        this->highestScore = score;
        this->programWithHighestScore = program;
    }
}

int ProgramsScore::getScore(Program* program) {
    assert(this->exists(program));
    return this->scoreMap[program];
}

bool ProgramsScore::exists(Program* program) {
    return this->scoreMap.find(program) != this->scoreMap.end();
}

void ProgramsScore::accept(Visitor* visitor) {
    return;
}