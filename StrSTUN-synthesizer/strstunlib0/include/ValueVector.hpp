#ifndef VALUE_VECTOR
#define VALUE_VECTOR

#include "strstunbase.hpp"
#include "strstunvalues.hpp"
#include <vector>
class Visitor;

class ValueVector : ObjBase {
public:
    std::vector<ValBase*> values;
    ValueVector();
    ValueVector(std::vector<ValBase*> vals);
    ~ValueVector();
    ValType getValueType();
    bool equalTo(ValueVector& valvec);
    void accept(Visitor* visitor) override;
    
    static std::vector<int> getConcreteMatchingVector(ValueVector& targetVector, ValueVector& compVector, int firstN);
    static std::vector<int> convertToMatchingVector(ValueVector& boolVector, int firstN);

    static ValueVector getCondVector(ValueVector& targetVector, ValueVector& compVector);
    static ValueVector applyCondThenMask(ValueVector& targetVector, ValueVector& condVector);
    static ValueVector applyCondElseMask(ValueVector& targetVector, ValueVector& condVector);
};

struct ValVecP {
public: 
    ValueVector* valvec;
    bool operator==(const ValVecP &p) const
    {
        return this->valvec->equalTo(*p.valvec);
    }
};

struct ValVecPHashFunc
{
    std::size_t operator() (const ValVecP &node) const
    {
        std::size_t h1 = 0;
        ValType vt = node.valvec->getValueType();
        if(vt == ValType::INT) {
            for(auto& v : node.valvec->values) {
                ValInt* vi = reinterpret_cast<ValInt*>(v);
                h1 = (h1 << 1) ^ std::hash<int>()(vi->value);
            }
        }
        else if(vt == ValType::BOOL) {
            for(auto& v : node.valvec->values) {
                ValBool* vi = reinterpret_cast<ValBool*>(v);
                h1 = (h1 << 1) ^ std::hash<bool>()(vi->value);
            }
        }
        else if(vt == ValType::STRING) {
            for(auto& v : node.valvec->values) {
                ValStr* vi = reinterpret_cast<ValStr*>(v);
                h1 = (h1 << 1) ^ std::hash<std::string>()(vi->value);
            }
        }
        else {
            assert(false);
        }
        return h1;
    }
};

#endif