%{
    #include <stdio.h>
    #include "inc/symtable.h"

    int yyerror (char* yaccProvideMessage);

    int scope = 0;
    int funcFlag = 0;
    int loopFlag = 0;
    int infunc=0;
    int anonymousCnt = 0;
    char buf[1024]; 
    SymTable_T symtable;
    char **errors;
    int funcScope[1024] = {0};
    int errorsCnt, errorsSize;
    extern int yylex(void);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
%}

%union {
    int intval;
    char *strval;
    float floatval;

}


%start program
%right ASSIGN
%left OR
%left AND
%nonassoc EQ NOT_EQ
%nonassoc GR_THAN GREQ_THAN LESS_THAN LEQ_THAN
%left PLUS MINUS
%left MULT DIV MOD
%right NOT INCR DECR UMINUS
%left DOT DOTDOT
%left SQ_BR_RIGHT SQ_BR_LEFT
%left LEFT_PAR RIGHT_PAR
%token SEMICOLON COMMA COLON DOUBLE_COLON IF ELSE WHILE FOR FUNC RETURN BREAK 
%token LEFT_CURL_BR RIGHT_CURL_BR CONTINUE LOCAL TRUE FALSE NIL /*ENDLINE*/

%token <strval> ID STRING
%token <intval> INT
%token <floatval> REAL
%type <strval> lvalue       
%%

program:        stmts{printf("line %d: program -> stmts\n", yylineno);};

stmts:          stmts statement {printf("line %d: stmts->stmts statement\n", yylineno);}
                | statement{printf("line %d: stmts->statement\n", yylineno);}
                ;

statement:      expr SEMICOLON {printf("line %d: statement->expr;\n", yylineno);}
                | ifstmt {printf("line %d: statement->ifstmt\n", yylineno);}
                | whilestmt {printf("line %d: statement->whilestmt\n", yylineno);}
                | forstmt {printf("line %d: statement->forstmt\n", yylineno);}
                | returnstmt {printf("line %d: statement->returnstmt\n", yylineno);}
                | BREAK SEMICOLON {if (loopFlag == 0){printf("\nError: use of break outside of loop line %d\n\n", yylineno);}printf("line %d: statement->break;\n", yylineno);}
                | CONTINUE SEMICOLON{if (loopFlag == 0){printf("\nError: use of return outside of loop, line %d\n\n", yylineno);}printf("line %d: statement->continue;\n", yylineno);}
                | block{printf("line %d: statement->block\n", yylineno);}
                | funcdef{printf("line %d: statement->funcdef\n", yylineno);}
                | SEMICOLON{printf("line %d: statement->;\n", yylineno);}
                ;


expr:           assignexpr{printf("line %d: expr->assignexpr\n", yylineno);}
                | expr PLUS expr{printf("line %d: expr->expr+expr\n", yylineno);}
                | expr MINUS expr{printf("line %d: expr->exor-expr\n", yylineno);}
                | expr MULT expr{printf("line %d: expr->expr*expr\n", yylineno);}
                | expr DIV expr{printf("line %d: expr->expr/expr\n", yylineno);}
                | expr MOD expr{printf("line %d: expr->expr MOD expr\n", yylineno);}
                | expr GR_THAN expr{printf("line %d: expr->expr>expr\n", yylineno);}
                | expr GREQ_THAN expr{printf("line %d: expr->expr >= expr\n", yylineno);}
                | expr LESS_THAN expr{printf("line %d: expr->expr < expr\n", yylineno);}
                | expr LEQ_THAN expr{printf("line %d: expr->expr <= expr\n", yylineno);}
                | expr EQ expr{printf("line %d: expr->expr == expr\n", yylineno);}
                | expr NOT_EQ expr{printf("line %d: expr->expr != expr\n", yylineno);}
                | expr AND expr{printf("line %d: expr->expr and expr\n", yylineno);}
                | expr OR expr{printf("line %d: expr->expr or expr\n", yylineno);}
                | term{printf("line %d: expr->term\n", yylineno);}
                ;

term:           LEFT_PAR expr RIGHT_PAR{printf("line %d: term-> (expr)\n", yylineno);}
                | UMINUS expr{printf("line %d: term-> -expr\n", yylineno);}
                | NOT expr{printf("line %d: term-> not expr\n", yylineno);}
                | INCR lvalue{ if($2 != NULL){ node *tmp = getSymbol($2, symtable);
                                if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)) 
                                    {printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $2, yylineno);};
                }
                                printf("line %d: term-> ++lvalue\n", yylineno);}
                | lvalue INCR{ if($1 != NULL){ node *tmp = getSymbol($1, symtable);
                                if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)) 
                                    {printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1, yylineno);}
                }
                                printf("line %d: term-> lvalue++\n", yylineno);}
                | DECR lvalue{ if($2 != NULL){ node *tmp = getSymbol($2, symtable);
                                if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)) 
                                    {printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $2, yylineno);};
                }
                                printf("line %d: term-> --lvalue\n", yylineno);}
                | lvalue DECR{ if($1 != NULL){ node *tmp = getSymbol($1, symtable);
                                if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)) 
                                    {printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1, yylineno);};
                }
                                printf("line %d: term-> lvalue--\n", yylineno);}
                | primary  {printf("line %d: term->primary\n", yylineno);}
                ;

assignexpr:     lvalue ASSIGN expr{
    if($1 != NULL){
        node *tmp = getSymbol($1, symtable);
        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)){
                printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1, yylineno);
        }
    }
    printf("line %d: assignexpr->lvalue = expr\n", yylineno);
    };

primary:        lvalue{printf("line %d: primary->lvalue\n", yylineno);}
                | call{printf("line %d: primary->call\n", yylineno);}
                | objectdef{printf("line %d: primary->objectdef\n", yylineno);}
                /* | funcdef {printf("line %d: primary->funcdef\n", yylineno);} // to avoid crashing on foo = function(...){...} case */
                | LEFT_PAR funcdef RIGHT_PAR{printf("line %d: primary->(funcdef)\n", yylineno);}
                | const{printf("line %d: primary->const\n", yylineno);}
                ;

lvalue:         ID{$$=$1;node* symbol = getSymbol($1, symtable);
                    if (symbol == NULL){
                        if (scope == 0) {
                            SymTable_put(symtable, $1,$1, GLOBAL, scope, yylineno,0);} 
                    else {SymTable_put(symtable, $1,$1, LOCALV, scope, yylineno,0);}; 
                    } else {
                        if (findScope($1,symtable) != 0 && findScope($1,symtable) != scope && funcScope[scope] != funcScope[findScope($1,symtable)]){
                            printf("\nError: var not accesible, symbol: %s line: %d\n\n", $1, yylineno);
                        }
                    };
                printf("line %d: lvalue->id\n", yylineno);}


                | LOCAL ID{$$=$2;node *tmp = getSymbol($2, symtable); if(tmp!=NULL && tmp->type == LIBFUNC){printf("\nError: func shadows lib func, symbol: %s line: %d\n\n", $2, yylineno);}
                                                                else {SymTable_put(symtable, $2,$2, LOCALV, scope, yylineno,1);} 
                                                                printf("line %d: lvalue->local id\n", yylineno);}
                | DOUBLE_COLON ID{$$=$2;if (getSymbolScope($2, symtable,0) == NULL){printf("\nError: global var doesnt exist, symbol: %s line: %d\n\n", $2, yylineno);} 
                                printf("line %d: lvalue-> ::id\n", yylineno);}
                | member{$$ = NULL; printf("line %d: lvalue-> member\n", yylineno);}
                ;

member:         lvalue DOT ID{printf("line %d: member-> lvalue.ID\n", yylineno);}
                | lvalue SQ_BR_LEFT expr SQ_BR_RIGHT{printf("line %d: member-> lvalue{expr}\n", yylineno);}
                | call DOT ID{printf("line %d: member-> call.ID\n", yylineno);}
                | call SQ_BR_LEFT expr SQ_BR_RIGHT{printf("line %d: member-> call{expr}\n", yylineno);}
                ;

call:           call LEFT_PAR elist RIGHT_PAR{printf("line %d: call-> call(elist)\n", yylineno);}
                | lvalue callsuffix{printf("line %d: call-> lvalue callsuffix\n", yylineno);}
                | LEFT_PAR funcdef RIGHT_PAR LEFT_PAR elist RIGHT_PAR{printf("line %d: call-> (funcdef)(elist)\n", yylineno);}
                ;

callsuffix:     normcall{printf("line %d: callsuffix-> normcall\n", yylineno);}
                | methodcall{printf("line %d: callsuffix-> methodcall\n", yylineno);}
                ;

normcall:       LEFT_PAR elist RIGHT_PAR{printf("line %d: normcall-> (elist)\n", yylineno);};
methodcall:     DOTDOT ID LEFT_PAR elist RIGHT_PAR {printf("line %d: methodcall-> ..ID(elist)\n", yylineno);};

elist:          expr{printf("line %d: elist-> expr\n", yylineno);}
                | expr COMMA elist{printf("line %d: elist-> expr,elist\n", yylineno);}
                | {printf("line %d: elist-> EMPTY RULE\n", yylineno);}
                ;

objectdef:      SQ_BR_LEFT elist SQ_BR_RIGHT{printf("line %d: objectdef-> [elist]\n", yylineno);}
                | SQ_BR_LEFT indexed SQ_BR_RIGHT{printf("line %d: objectdef-> [indexed]\n", yylineno);}
                ;
            
indexed:        indexedelem{printf("line %d: indexed-> indexedelem\n", yylineno);}
                | indexedelem COMMA indexed{printf("line %d: indexed-> indexedelem,indexed\n", yylineno);}
                ;

indexedelem:    LEFT_CURL_BR {scope++; funcScope[scope]=funcScope[scope-1];} expr COLON expr RIGHT_CURL_BR{hideScope(scope--); printf("line %d: indexedelem-> {expr:expr}\n", yylineno);}
                ;

block:          LEFT_CURL_BR {if (funcFlag == 0){scope++;funcScope[scope] = funcScope[scope-1];}
                              else {funcFlag = 0;funcScope[scope] = funcScope[scope-1] + 1;};} RIGHT_CURL_BR{hideScope(scope--); printf("line %d: block-> {}\n", yylineno);}
                | LEFT_CURL_BR {if (funcFlag == 0){scope++;funcScope[scope] = funcScope[scope-1];}
                                else {funcFlag = 0;funcScope[scope] = funcScope[scope-1] + 1;};} stmts RIGHT_CURL_BR{hideScope(scope--);
                                 printf("line %d: block-> {stmts}\n", yylineno);}
                ;

funcdef:        FUNC ID {checkFunc($2, symtable, scope, yylineno);SymTable_put(symtable, $2, $2, USERFUNC, scope, yylineno, 0);} LEFT_PAR {scope++; funcFlag++; infunc++; funcScope[scope]=funcScope[scope-1];} 
                                idlist RIGHT_PAR block{infunc--; printf("line %d: funcdef-> function ID(idlist) block\n", yylineno);}
                | FUNC {sprintf(buf, "$%d", anonymousCnt++); SymTable_put(symtable, buf, buf, USERFUNC, scope, yylineno, 0);} LEFT_PAR {scope++; funcFlag++; infunc++; funcScope[scope]=funcScope[scope-1];} 
                                idlist RIGHT_PAR block{infunc--; printf("line %d: funcdef-> function (idlist) block\n", yylineno);}
                ;

const:          INT{printf("line %d: const-> int\n", yylineno);}
                | REAL{printf("line %d: const-> real\n", yylineno);}
                | STRING {printf("line %d: const-> string\n", yylineno);}
                | NIL{printf("line %d: const-> nil\n", yylineno);}
                | TRUE{printf("line %d: const-> true\n", yylineno);}
                | FALSE{printf("line %d: const-> false\n", yylineno);}
                // | ENDLINE{;}
                ;

idlist:         ID {SymTable_put(symtable, $1, $1, FORMAL, scope, yylineno, 0); printf("line %d: idlist-> ID\n", yylineno);}
                | idlist COMMA ID{SymTable_put(symtable, $3, $3, FORMAL, scope, yylineno, 0); printf("line %d: idlist-> idlist, ID\n", yylineno);}
                | {printf("line %d: idlist-> EMPTY RULE\n", yylineno);}
                ;

ifstmt:         IF LEFT_PAR expr RIGHT_PAR statement{printf("line %d: ifstmt-> if(expr) statement\n", yylineno);}
                | IF LEFT_PAR expr RIGHT_PAR statement ELSE statement{printf("line %d: ifstmt-> if(expr) statement else statement\n", yylineno);}
                ;

whilestmt:      WHILE {loopFlag = 1;} LEFT_PAR expr RIGHT_PAR statement{loopFlag = 0;printf("line %d: whilestmt-> while(expr) statement\n", yylineno);};

forstmt:        FOR {loopFlag = 1;} LEFT_PAR elist SEMICOLON expr SEMICOLON elist RIGHT_PAR statement{loopFlag = 0;printf("line %d: forstmt-> for(elist;expr;elist) statement\n", yylineno);};

returnstmt:     RETURN SEMICOLON{if (infunc == 0){printf("\nError: use of return outside of function, line %d\n", yylineno);}printf("line %d: returnstmt-> return;\n", yylineno);}
                | RETURN expr SEMICOLON{if (infunc == 0){printf("\nError: use of return outside of function, line %d\n", yylineno);}printf("line %d: returnstmt-> return expr;\n", yylineno);}
                ;

%%

int yyerror (char* yaccProvideMessage){
    fprintf(stderr, "%s: at line %d before token: %s\n", yaccProvideMessage, yylineno, yytext);
    fprintf(stderr, "Input not valid!\n");
    return 0;
}
