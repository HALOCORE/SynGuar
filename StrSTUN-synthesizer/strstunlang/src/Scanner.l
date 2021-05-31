%%
#.*         ; // ignore comments
[ \t]       ; // ignore all whitespace
\n          ; // ignore new line
"Int"           {return Parser::T_TYPE;}
"Bool"           {return Parser::T_TYPE;}
"Str"           {return Parser::T_TYPE;}
"::"            {return Parser::T_DSEMICOLON;}
"->"            {return Parser::T_ARROW;}
";"             {return Parser::T_END;}
"="             {return Parser::T_EQUAL;}
"if"            {return Parser::T_IF;}
"else"          {return Parser::T_ELSE;}
"then"          {return Parser::T_THEN;}

"zero"          {return Parser::T_ZERO;} //obselete
"nil"           {return Parser::T_NIL;} //obselete

"("             {return Parser::T_LEFT;}
")"             {return Parser::T_RIGHT;}
[a-zA-Z\+\-\=][a-zA-Z0-9\.\+\-\=]*  {return Parser::T_IDENTIFIER;}
S\.\"[^\n"]*\" {return Parser::T_IDENTIFIER;}
