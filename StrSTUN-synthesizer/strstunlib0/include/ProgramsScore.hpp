#ifndef PROGRAMS_SCORE
#define PROGRAMS_SCORE

#include <unordered_map>
#include "strstunbase.hpp"
#include "Program.hpp"

class ProgramsScore : ObjBase{
public:
    ProgramsScore() noexcept;
    int getHighestScore();
    Program* getProgramWithHightestScore();
    void setScore(Program* program, int score);
    int getScore(Program* program);
    bool exists(Program* program);
    void accept(Visitor* visitor) override;
private:
    std::unordered_map<Program*, int> scoreMap;
    int highestScore;
    Program* programWithHighestScore;
};

#endif