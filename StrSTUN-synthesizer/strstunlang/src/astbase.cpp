#include "astbase.hpp"

std::ostream & ASTIdentifier::operator >>(std::ostream &os) {
    return os << this->id;
}

std::ostream & ASTTypeBasic::operator >>(std::ostream &os) {
    return os << this->typeBasic;
}

std::ostream & ASTTypeBasicList::operator >>(std::ostream &os) {
    *this->head >> os;
    if (this->tail != nullptr) {
        os << " -> ";
        *this->tail >> os;
    }
    return os;
}

std::ostream & ASTTypeDecl::operator >>(std::ostream &os) {
    *this->funcId >> os;
    os << " :: ";
    *this->inputTypes >> os;
    os << " -> (";
    *this->outputType >> os;
    os << ");";
    return os;
}

std::ostream & ASTFunction::operator >>(std::ostream &os) {
    *this->typeDecl >> os;
    os << std::endl;
    *this->funcId >> os;
    os << " ";
    *this->paramList >> os;
    os << " = ";
    *this->expression >> os;
    os << ";";
    return os;
}

std::ostream & ASTFunctionList::operator >>(std::ostream &os) {
    *this->head >> os;
    os << std::endl;
    if (this->tail != nullptr) {
        os << std::endl;
        *this->tail >> os;
    }
    return os;
}

std::ostream & ASTParamList::operator >>(std::ostream &os) {
    *this->head >> os;
    if (this->tail != nullptr) {
        os << " ";
        *this->tail >> os;
    }
    return os;
}

std::ostream & ASTExpression::operator >>(std::ostream &os) {
    if (this->expressionType == ASTExpressionType::ASTEXP_FUNC_APPLY) {
        ASTExpressionFuncApply* expr = dynamic_cast<ASTExpressionFuncApply*>(this);
        assert(expr != nullptr);
        return *expr >> os;
    } else if (this->expressionType == ASTExpressionType::ASTEXP_IF_THEN_ELSE) {
        ASTExpressionIfThenElse* expr = dynamic_cast<ASTExpressionIfThenElse*>(this);
        assert(expr != nullptr);
        return *expr >> os;
    } else if (this->expressionType == ASTExpressionType::ASTEXP_SCALAR) {
        ASTExpressionScalar* expr = dynamic_cast<ASTExpressionScalar*>(this);
        assert(expr != nullptr);
        return *expr >> os;
    } else assert(false);
}

std::ostream & ASTExpressionFuncApply::operator >>(std::ostream &os) {
    os << "(";
    *this->funcId >> os;
    os << " ";
    *this->realParams >> os;
    os << ")";
    return os;
}

std::ostream & ASTExpressionScalar::operator >>(std::ostream &os) {
    return os << this->scalarName;
}

std::ostream & ASTExpressionIfThenElse::operator >>(std::ostream &os) {
    os << "if ";
    *this->condExp >> os;
    os << " then ";
    *this->thenExp >> os; 
    os << " else ";
    *this->elseExp >> os;
    return os;
}

std::ostream & ASTExpressionList::operator >>(std::ostream &os) {
    *this->head >> os;
    if (this->tail != nullptr) {
        os << " ";
        *this->tail >> os;
    }
    return os;
}


