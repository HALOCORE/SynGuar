%scanner Scanner.h
%baseclass-preinclude astbase.hpp

%token T_DSEMICOLON T_TYPE T_ARROW
%token T_EQUAL T_IF T_ELSE T_THEN T_ZERO T_NIL T_LEFT T_RIGHT T_END
%token T_IDENTIFIER

// %stype std::string

%stype ASTNode*

// %union {
//     std::string str;
//     ASTNode* ast;
// }

// %type <str> identifier;
// %type <ast> function paramlist expression expressionlist;

%start module

%%

module: functionlist {root = $1;}
;

functionlist: function  {$$ = new ASTFunctionList($1, nullptr);}
            | function functionlist {$$ = new ASTFunctionList($1, $2);}
;

function: typedecl identifier paramlist T_EQUAL expression T_END {$$ = new ASTFunction($1, $2, $3, $5); }
;

typedecl: identifier T_DSEMICOLON typebasiclist T_LEFT typebasic T_RIGHT T_END {$$ = new ASTTypeDecl($1, $3, $5);}
;

typebasiclist: typebasic T_ARROW {$$ = new ASTTypeBasicList($1, nullptr);}
            | typebasic T_ARROW typebasiclist {$$ = new ASTTypeBasicList($1, $3);}
;

paramlist: identifier {$$ = new ASTParamList($1, nullptr);}
            | identifier paramlist {$$ = new ASTParamList($1, $2);}
;

expression: T_LEFT identifier expressionlist T_RIGHT {$$ = new ASTExpressionFuncApply($2, $3);}
            | T_NIL {$$ = new ASTExpressionScalar("nil");}
            | T_ZERO {$$ = new ASTExpressionScalar("zero");}
            | identifier {$$ = new ASTExpressionScalar(reinterpret_cast<ASTIdentifier*>($1)->id);}
            | T_IF expression T_THEN expression T_ELSE expression {$$ = new ASTExpressionIfThenElse($2, $4, $6);}
;

expressionlist: expression  {$$ = new ASTExpressionList($1, nullptr);}
            | expression expressionlist {$$ = new ASTExpressionList($1, $2);}
;

identifier: T_IDENTIFIER { $$ = new ASTIdentifier(std::string(d_scanner.matched()));}
;

typebasic: T_TYPE { $$ = new ASTTypeBasic(std::string(d_scanner.matched())); }
;