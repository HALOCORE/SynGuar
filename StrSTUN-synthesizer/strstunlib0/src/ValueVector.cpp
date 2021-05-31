#include "ValueVector.hpp"
#include "visitorbase.hpp"

ValueVector::ValueVector() : values() {}

ValueVector::ValueVector(std::vector<ValBase*> vals) : values(vals) {}

ValueVector::~ValueVector() {
    return;//TODO
    for (ValBase* pv : this->values) {
        if (pv->type == ValType::BOOL)
            delete dynamic_cast<ValBool*>(pv);
        else if (pv->type == ValType::INT)
            delete dynamic_cast<ValInt*>(pv);
        else if (pv->type == ValType::STRING)
            delete dynamic_cast<ValStr*>(pv);
        else assert(false);
    }
}

ValType ValueVector::getValueType() {
    ValType vt = ValType::UNKNOWN;
    for (ValBase* pv : this->values) {
        assert(pv->type != ValType::UNKNOWN);
        if (vt == ValType::UNKNOWN) vt = pv->type;
        else assert(vt == pv->type);
    }
    return vt;
}

bool ValueVector::equalTo(ValueVector& valvec) {
    assert(this->values.size() == valvec.values.size());
    for (int i = 0; i < this->values.size(); i++) {
        if (!this->values[i]->equalTo(valvec.values[i])) return false;
    }
    return true;
}

std::vector<int> ValueVector::getConcreteMatchingVector(ValueVector& targetVector, ValueVector& compVector, int firstN) {
    int targetSize = targetVector.values.size();
    if(firstN < 0) assert(targetSize == compVector.values.size());
    else assert(firstN == targetSize);
    std::vector<int> result;
    for (int i = 0; i < targetSize; i++) {
        assert(!targetVector.values[i]->isDontCare());
        if (targetVector.values[i]->equalTo(compVector.values[i])) {
            result.push_back(1);
        }else {
            result.push_back(0);
        }
    }
    assert(result.size() == targetSize);
    return result;
}

std::vector<int> ValueVector::convertToMatchingVector(ValueVector& boolVector, int firstN) {
    std::vector<int> result;
    int resultSize = firstN < 0 ? boolVector.values.size() : firstN;
    for (int i = 0; i < resultSize; i++) {
        assert(!boolVector.values[i]->isDontCare());
        assert(boolVector.getValueType() == ValType::BOOL);
        ValBool* vb = reinterpret_cast<ValBool*>(boolVector.values[i]);
        if (vb->isValid()) {
            if(vb->value == true) result.push_back(1);
            else result.push_back(0);
        }else {
            assert(vb->isErr());
            result.push_back(MATCHVEC_ERROR);
        }
    }
    assert(result.size() > 0 && resultSize == result.size());
    return result;
}


ValueVector ValueVector::getCondVector(ValueVector& targetVector, ValueVector& compVector) {
    assert(targetVector.values.size() == compVector.values.size());
    ValueVector conds;
    for (int i = 0; i < compVector.values.size(); i++) {
        if (targetVector.values[i]->isDontCare()) {
            conds.values.push_back(new ValBool(false, ValStatus::DONT_CARE));
        }else if (targetVector.values[i]->equalTo(compVector.values[i])) {
            conds.values.push_back(new ValBool(true));
        }else {
            conds.values.push_back(new ValBool(false));
        }
    }
    return conds;
}

ValueVector ValueVector::applyCondThenMask(ValueVector& targetVector, ValueVector& condVector) {
    assert(targetVector.values.size() == condVector.values.size());
    ValueVector thenVec;
    for (int i = 0; i < targetVector.values.size(); i++) {
        ValBool* condVal = dynamic_cast<ValBool*>(condVector.values[i]);
        if (condVal->isDontCare() || (condVal->status == ValStatus::VALID && condVal->value == false)) {
            if (targetVector.values[i]->type == ValType::BOOL)
                thenVec.values.push_back(new ValBool(false, ValStatus::DONT_CARE));
            else if (targetVector.values[i]->type == ValType::INT)
                thenVec.values.push_back(new ValInt(0, ValStatus::DONT_CARE));
            else if (targetVector.values[i]->type == ValType::STRING)
                thenVec.values.push_back(new ValStr("", ValStatus::DONT_CARE));
            else assert(false);
        }else if (condVal->status == ValStatus::VALID && condVal->value == true) {
            if (targetVector.values[i]->type == ValType::BOOL)
                thenVec.values.push_back(new ValBool(*dynamic_cast<ValBool*>(targetVector.values[i])));
            else if (targetVector.values[i]->type == ValType::INT)
                thenVec.values.push_back(new ValInt(*dynamic_cast<ValInt*>(targetVector.values[i])));
            else if (targetVector.values[i]->type == ValType::STRING)
                thenVec.values.push_back(new ValStr(*dynamic_cast<ValStr*>(targetVector.values[i])));
            else assert(false);
        }else {
            assert (false);
        }
    }
    return thenVec;
}

ValueVector ValueVector::applyCondElseMask(ValueVector& targetVector, ValueVector& condVector) {
    assert(targetVector.values.size() == condVector.values.size());
    ValueVector resultVec;
    for (int i = 0; i < targetVector.values.size(); i++) {
        ValBool* condVal = dynamic_cast<ValBool*>(condVector.values[i]);
        if (condVal->isDontCare() || (condVal->status == ValStatus::VALID && condVal->value == true)) {
            if (targetVector.values[i]->type == ValType::BOOL)
                resultVec.values.push_back(new ValBool(false, ValStatus::DONT_CARE));
            else if (targetVector.values[i]->type == ValType::INT)
                resultVec.values.push_back(new ValInt(0, ValStatus::DONT_CARE));
            else if (targetVector.values[i]->type == ValType::STRING)
                resultVec.values.push_back(new ValStr("", ValStatus::DONT_CARE));
            else assert(false);
        }else if (condVal->status == ValStatus::VALID && condVal->value == false) {
            if (targetVector.values[i]->type == ValType::BOOL)
                resultVec.values.push_back(new ValBool(*dynamic_cast<ValBool*>(targetVector.values[i])));
            else if (targetVector.values[i]->type == ValType::INT)
                resultVec.values.push_back(new ValInt(*dynamic_cast<ValInt*>(targetVector.values[i])));
            else if (targetVector.values[i]->type == ValType::STRING)
                resultVec.values.push_back(new ValStr(*dynamic_cast<ValStr*>(targetVector.values[i])));
            else assert(false);
        }else {
            assert (false);
        }
    }
    return resultVec;
}

void ValueVector::accept(Visitor* visitor) {
    for(auto v : this->values) {
        ValInt* vint = dynamic_cast<ValInt*>(v);
        if (vint != nullptr) { visitor->visit(vint); continue; }
        ValBool* vbool = dynamic_cast<ValBool*>(v);
        if (vbool != nullptr) { visitor->visit(vbool); continue; }
        ValStr* vstr = dynamic_cast<ValStr*>(v);
        if (vstr != nullptr) { visitor->visit(vstr); continue; }
    }
}

