#include "strstunvalues.hpp"

void ValBase::accept(Visitor* visitor) {
    std::cerr << "[WARN] should not call accept on Value." << std::endl;
    return;
}

bool ValBase::isDontCare() {
    return this->status == ValStatus::DONT_CARE;
}

bool ValBase::isErr() {
    return this->status == ValStatus::ERR;
}

bool ValBase::isValid() {
    return this->status == ValStatus::VALID;
}

void ValBase::print(std::ostream& out) {
    if (this->type == ValType::INT) {
        ValInt* val = dynamic_cast<ValInt*>(this);
        assert(val != nullptr);
        out << val->value;
    } else if (this->type == ValType::BOOL) {
        ValBool* val = dynamic_cast<ValBool*>(this);
        assert(val != nullptr);
        out << val->value;
    } else if (this->type == ValType::STRING) {
        ValStr* val = dynamic_cast<ValStr*>(this);
        assert(val != nullptr);
        out << val->value;
    } else assert(false);
}

ValInt::ValInt(int val) {
    this->value = val;
    this->type = ValType::INT;
    this->status = ValStatus::VALID;
}

ValInt::ValInt(int val, ValStatus status) : ValInt(val) {
    this->status = status;
}

bool ValInt::equalTo(ValBase* compareVal) {
    ValType tp = compareVal->type;
    if (tp != ValType::INT) return false;
    ValInt* vi = dynamic_cast<ValInt*>(compareVal);
    if (this->status != vi->status) return false;
    if (this->status == ValStatus::VALID) return this->value == vi->value;
    return true;
}

ValBool::ValBool(bool val) {
    this->value = val;
    this->type = ValType::BOOL;
    this->status = ValStatus::VALID;
}

ValBool::ValBool(bool val, ValStatus status) : ValBool(val) {
    this->status = status;
}

bool ValBool::equalTo(ValBase* compareVal) {
    ValType tp = compareVal->type;
    if (tp != ValType::BOOL) return false;
    ValBool* vb = dynamic_cast<ValBool*>(compareVal);
    if (this->status != vb->status) return false;
    if (this->status == ValStatus::VALID) return this->value == vb->value;
    return true;
}

ValStr::ValStr(std::string val) {
    this->value = val;
    this->type = ValType::STRING;
    this->status = ValStatus::VALID;
}

ValStr::ValStr(std::string val, ValStatus status) : ValStr(val) {
    this->status = status;
}

bool ValStr::equalTo(ValBase* compareVal) {
    ValType tp = compareVal->type;
    if (tp != ValType::STRING) return false;
    ValStr* vs = dynamic_cast<ValStr*>(compareVal);
    if (this->status != vs->status) return false;
    if (this->status == ValStatus::VALID) return this->value == vs->value;
    return true;
}