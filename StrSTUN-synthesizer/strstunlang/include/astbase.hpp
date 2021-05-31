#ifndef ASTBASE
#define ASTBASE

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

enum ASTType : int {
    AST_TYPE_DECL,
    AST_TYPE_BASIC,
    AST_TYPE_BASIC_LIST,
    AST_FUNCTION,
    AST_FUNCTION_LIST,
    AST_PARAM_LIST,
    AST_EXPRESSION,
    AST_EXPRESSION_LIST,
    AST_IDENTIFIER,
};

enum ASTExpressionType : int {
    ASTEXP_FUNC_APPLY,
    ASTEXP_SCALAR,
    ASTEXP_IF_THEN_ELSE
};

class ASTNode {
public:
    ASTType type;
    virtual std::ostream &operator >>(std::ostream &os) = 0;
    int total_component_size = -1;
};

class ASTIdentifier : public ASTNode {
public:
    ASTType type;
    std::string id;
    inline ASTIdentifier(std::string id) {
        this->type = ASTType::AST_IDENTIFIER;
        this->id = id;
        this->total_component_size = 1;
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTTypeBasic : public ASTNode {
public:
    ASTType type;
    std::string typeBasic;
    inline ASTTypeBasic(std::string typeBasic) {
        this->type = ASTType::AST_TYPE_BASIC;
        this->typeBasic = typeBasic;
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTTypeBasicList : public ASTNode {
public:
    ASTType type;
    ASTTypeBasic* head;
    ASTTypeBasicList* tail;
    inline ASTTypeBasicList(ASTNode* head, ASTNode* tail) {
        //std::cerr << "Create ASTTypeBasicList: " << head << " " << tail << std::endl;
        this->type = ASTType::AST_TYPE_BASIC_LIST;
        this->head = dynamic_cast<ASTTypeBasic*>(head);
        assert(this->head != nullptr);
         if (tail != nullptr) {
            this->tail = dynamic_cast<ASTTypeBasicList*>(tail);
            assert(this->tail != nullptr);
        } else {
            this->tail = nullptr;
        }
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTTypeDecl : public ASTNode {
public:
    ASTType type;
    ASTIdentifier* funcId;
    ASTTypeBasicList* inputTypes;
    ASTTypeBasic* outputType;
    inline ASTTypeDecl(ASTNode* funcId, ASTNode* inputTypes, ASTNode* outputType) {
        //std::cerr << "Create ASTTypeDecl " << std::endl;
        this->type = ASTType::AST_TYPE_DECL;
        this->funcId = dynamic_cast<ASTIdentifier*>(funcId);   
        assert(this->funcId != nullptr);
        this->inputTypes = dynamic_cast<ASTTypeBasicList*>(inputTypes);
        assert(this->inputTypes != nullptr);
        this->outputType = dynamic_cast<ASTTypeBasic*>(outputType);
        assert(this->outputType != nullptr);
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTFunction;
class ASTFunctionList;
class ASTParamList;
class ASTExpression;
class ASTExpressionList;

class ASTParamList : public ASTNode {
public:
    ASTIdentifier* head;
    ASTParamList* tail;
    inline ASTParamList(ASTNode* head, ASTNode* tail) {
        //std::cerr << "Create ASTParamList: " << head << " " << tail << std::endl;
        this->type = ASTType::AST_PARAM_LIST;
        this->head = dynamic_cast<ASTIdentifier*>(head);
        assert(this->head != nullptr);
        if (tail != nullptr) {
            this->tail = dynamic_cast<ASTParamList*>(tail);
            assert(this->tail != nullptr);
        } else {
            this->tail = nullptr;
        }
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTExpression : public ASTNode {
public:
    ASTExpressionType expressionType;
    inline ASTExpression(ASTExpressionType expressionType) {
        //std::cerr << "Create ASTExpression: type=" << expressionType << std::endl;
        this->type = ASTType::AST_EXPRESSION;
        this->expressionType = expressionType;
    }
    int num_conds = -1;
    int  max_term_size = -1;
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTExpressionList : public ASTNode {
public:
    ASTExpression* head;
    ASTExpressionList* tail;
    inline ASTExpressionList(ASTNode* head, ASTNode* tail) {
        this->type = ASTType::AST_EXPRESSION_LIST;
        this->head = dynamic_cast<ASTExpression*>(head);
        assert(this->head != nullptr);
        assert(this->head->total_component_size > 0);
        this->total_component_size = this->head->total_component_size;
        if(tail != nullptr) {
            this->tail = dynamic_cast<ASTExpressionList*>(tail);
            assert(this->tail != nullptr);
            assert(this->tail->total_component_size > 0);
            this->total_component_size += this->tail->total_component_size;
        } else {
            this->tail = nullptr;
        }
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTExpressionFuncApply : public ASTExpression {
public:
    ASTIdentifier* funcId;
    ASTExpressionList* realParams;
    inline ASTExpressionFuncApply(ASTNode* funcId, ASTNode* realParams) : ASTExpression(ASTExpressionType::ASTEXP_FUNC_APPLY) {
        this->funcId = dynamic_cast<ASTIdentifier*>(funcId);   
        this->realParams = dynamic_cast<ASTExpressionList*>(realParams);
        assert(this->funcId != nullptr);
        assert(this->realParams != nullptr);
        assert(this->realParams->total_component_size > 0);
        this->total_component_size = this->realParams->total_component_size + 1;
        this->num_conds = 0;
        this->max_term_size = this->total_component_size;
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTExpressionScalar : public ASTExpression {
public:
    std::string scalarName;
    inline ASTExpressionScalar(std::string scalarName) : ASTExpression(ASTExpressionType::ASTEXP_SCALAR) {
        this->scalarName = scalarName;
        std::cerr << "[Debug] Scalar AST Created: " << scalarName << std::endl;
        this->total_component_size = 1;
        this->num_conds = 0;
        this->max_term_size = this->total_component_size;
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTExpressionIfThenElse : public ASTExpression {
public:
    ASTExpression* condExp;
    ASTExpression* thenExp;
    ASTExpression* elseExp;
    inline ASTExpressionIfThenElse(ASTNode* condExp, ASTNode* thenExp, ASTNode* elseExp) 
        : ASTExpression(ASTExpressionType::ASTEXP_IF_THEN_ELSE) {
        this->condExp = dynamic_cast<ASTExpression*>(condExp);
        this->thenExp = dynamic_cast<ASTExpression*>(thenExp);
        this->elseExp = dynamic_cast<ASTExpression*>(elseExp);
        assert(this->condExp != nullptr);
        assert(this->thenExp != nullptr);
        assert(this->elseExp != nullptr);
        assert(this->condExp->total_component_size > 0);
        assert(this->thenExp->total_component_size > 0);
        assert(this->elseExp->total_component_size > 0);
        this->total_component_size = 
            this->condExp->total_component_size 
            + this->thenExp->total_component_size
            + this->elseExp->total_component_size + 1;
        assert(this->condExp->max_term_size > 0);
        assert(this->thenExp->max_term_size > 0);
        assert(this->elseExp->max_term_size > 0);
        this->max_term_size = std::max(
                std::max(this->condExp->max_term_size, this->thenExp->max_term_size),
                this->elseExp->max_term_size);
        assert(this->thenExp->num_conds >= 0);
        assert(this->elseExp->num_conds >= 0);
        this->num_conds = this->thenExp->num_conds + this->elseExp->num_conds + 1;
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTFunction : public ASTNode {
public:
    ASTTypeDecl* typeDecl;
    ASTIdentifier* funcId;
    ASTParamList* paramList;
    ASTExpression* expression;
    inline ASTFunction(ASTNode* typeDecl, ASTNode* funcId, ASTNode* paramList, ASTNode* expression) {
        this->type = ASTType::AST_FUNCTION;
        this->typeDecl = dynamic_cast<ASTTypeDecl*>(typeDecl);
        this->funcId = dynamic_cast<ASTIdentifier*>(funcId);
        this->paramList = dynamic_cast<ASTParamList*>(paramList);
        this->expression = dynamic_cast<ASTExpression*>(expression);
        assert(this->typeDecl != nullptr);
        assert(this->funcId != nullptr);
        assert(this->paramList != nullptr);
        assert(this->expression != nullptr);
        assert(this->typeDecl->funcId->id == this->funcId->id);
    }
    std::ostream &operator >>(std::ostream &os) override;
};

class ASTFunctionList : public ASTNode {
public:
    ASTFunction* head;
    ASTFunctionList* tail;
    inline ASTFunctionList(ASTNode* head, ASTNode* tail) {
        this->type = ASTType::AST_FUNCTION_LIST;
        this->head = dynamic_cast<ASTFunction*>(head);
        assert(this->head != nullptr);
        if(tail != nullptr) {
            this->tail = dynamic_cast<ASTFunctionList*>(tail);
            assert(this->tail != nullptr);
        } else {
            this->tail = nullptr;
        }
    }
    std::ostream &operator >>(std::ostream &os) override;
};

#endif