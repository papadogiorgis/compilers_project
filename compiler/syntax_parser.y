%{
    #include <stdio.h>
    #include "inc/symtable.h"

    int yyerror (char* yaccProvideMessage);

    int scope = 0;
    int funcFlag = 0;
    int anonymousCnt = 0;
    char buf[1024]; 
    SymTable_T symtable;
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

// TODO Keep reading after error

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
%%

program:        stmts{printf("program -> stmts\n");};

stmts:          stmts statement {printf("stmts->stmts statement\n");}
                | statement{printf("stmts->statement\n");}
                ;

statement:      expr SEMICOLON {printf("statement->expr;\n");}
                | ifstmt {printf("statement->ifstmt\n");}
                | whilestmt {printf("statement->whilestmt\n");}
                | forstmt {printf("statement->forstmt\n");}
                | returnstmt {printf("statement->returnstmt\n");}
                | BREAK SEMICOLON {printf("statement->break;\n");}
                | CONTINUE SEMICOLON{printf("statement->continue;\n");}
                | block{printf("statement->block\n");}
                | funcdef{printf("statement->funcdef\n");}
                | SEMICOLON{printf("statement->;");}
                ;


expr:           assignexpr{printf("expr->assignexpr\n");}
                | expr PLUS expr{printf("expr->expr+expr\n");}
                | expr MINUS expr{printf("expr->exor-expr\n");}
                | expr MULT expr{printf("expr->expr*expr\n");}
                | expr DIV expr{printf("expr->expr/expr\n");}
                | expr MOD expr{printf("expr->expr%expr\n");}
                | expr GR_THAN expr{printf("expr->expr>expr\n");}
                | expr GREQ_THAN expr{printf("expr->expr >= expr\n");}
                | expr LESS_THAN expr{printf("expr->expr < expr\n");}
                | expr LEQ_THAN expr{printf("expr->expr <= expr\n");}
                | expr EQ expr{printf("expr->expr == expr\n");}
                | expr NOT_EQ expr{printf("expr->expr != expr\n");}
                | expr AND expr{printf("expr->expr and expr\n");}
                | expr OR expr{printf("expr->expr or expr\n");}
                | term{printf("expr->term\n");}
                ;

term:           LEFT_PAR expr RIGHT_PAR{printf("term-> (expr)\n");}
                | UMINUS expr{printf("term-> -expr\n");}
                | NOT expr{printf("term-> not expr\n");}
                | INCR lvalue{printf("term-> ++lvalue\n");}
                | lvalue INCR{printf("term-> lvalue++\n");}
                | DECR lvalue{printf("term-> --lvalue\n");}
                | lvalue DECR{printf("term-> lvalue--\n");}
                | primary  {printf("term->primary\n");}
                ;

assignexpr:     lvalue ASSIGN expr{printf("assignexpr->lvalue = expr\n");};

primary:        lvalue{printf("primary->lvalue\n");}
                | call{printf("primary->call\n");}
                | objectdef{printf("primary->objectdef\n");}
                | funcdef {printf("primary->funcdef\n");} // to avoid crashing on foo = function(...){...} case
                | LEFT_PAR funcdef RIGHT_PAR{printf("primary->(funcdef)\n");}
                | const{printf("primary->const\n");}
                ;

lvalue:         ID{if (scope == 0) {SymTable_put(symtable, $1,$1, GLOBAL, scope, yylineno,0);} else {SymTable_put(symtable, $1,$1, LOCALV, scope, yylineno,0);}; printf("lvalue->id\n");}
                | LOCAL ID{SymTable_put(symtable, $2,$2, LOCALV, scope, yylineno,1); printf("lvalue->local id\n");}
                | DOUBLE_COLON ID{printf("lvalue-> ::id\n");}
                | member{printf("lvalue-> member\n");}
                ;

member:         lvalue DOT ID{;}
                | lvalue SQ_BR_LEFT expr SQ_BR_RIGHT{;}
                | call DOT ID{;}
                | call SQ_BR_LEFT expr SQ_BR_RIGHT{;}
                ;

call:           call LEFT_PAR elist RIGHT_PAR{fprintf(stderr, "call (elist)\n");}
                | lvalue callsuffix{;}
                | LEFT_PAR funcdef RIGHT_PAR LEFT_PAR elist RIGHT_PAR{;}
                ;

callsuffix:     normcall{;}
                | methodcall{;}
                ;

normcall:       LEFT_PAR elist RIGHT_PAR{;};
methodcall:     DOTDOT ID LEFT_PAR elist RIGHT_PAR {;};

elist:          expr{;}
                | expr COMMA elist{;}
                |
                ;

objectdef:      SQ_BR_LEFT elist SQ_BR_RIGHT{;}
                | SQ_BR_LEFT indexed SQ_BR_RIGHT{;}
                ;
            
indexed:        indexedelem{;}
                | indexedelem COMMA indexed{;}
                ;

indexedelem:    LEFT_CURL_BR {scope++; fprintf(stderr, "scope = %d\n", scope);} expr COLON expr RIGHT_CURL_BR{scope--; fprintf(stderr, "scope = %d\n", scope);;}
                ;

block:          LEFT_CURL_BR {if (funcFlag == 0){scope++;}
                              else {funcFlag = 0;}; fprintf(stderr, "scope = %d\n", scope);} RIGHT_CURL_BR{scope--; fprintf(stderr, "scope = %d\n", scope);}
                | LEFT_CURL_BR {if (funcFlag == 0){scope++;}
                                else {funcFlag = 0;}; fprintf(stderr, "scope = %d\n", scope);} stmts RIGHT_CURL_BR{scope--; fprintf(stderr, "scope = %d\n", scope);}
                ;

funcdef:        FUNC ID {SymTable_put(symtable, $2, $2, USERFUNC, scope, yylineno, 0);} LEFT_PAR {scope++; funcFlag++; fprintf(stderr, "scope = %d\n", scope);} 
                                idlist RIGHT_PAR block{;}
                | FUNC {sprintf(buf, "$%d", anonymousCnt++); SymTable_put(symtable, buf, buf, USERFUNC, scope, yylineno, 0);} LEFT_PAR {scope++; funcFlag++; fprintf(stderr, "scope = %d\n", scope);} idlist RIGHT_PAR block{;}
                ;

const:          INT{fprintf(stderr, "int: %d\n", yylval.intval);}
                | REAL{fprintf(stderr, "real: %f\n", yylval.floatval);}
                | STRING {fprintf(stderr, "string: \"%s\"\n", yylval.strval);}
                | NIL{;}
                | TRUE{;}
                | FALSE{;}
                // | ENDLINE{;}
                ;

idlist:         ID {SymTable_put(symtable, $1, $1, FORMAL, scope, yylineno, 0);}
                | idlist COMMA ID{SymTable_put(symtable, $3, $3, FORMAL, scope, yylineno, 0);}
                |
                ;

ifstmt:         IF LEFT_PAR expr RIGHT_PAR statement{;}
                | IF LEFT_PAR expr RIGHT_PAR statement ELSE statement{;}
                ;

whilestmt:      WHILE LEFT_PAR expr RIGHT_PAR statement{;};

forstmt:        FOR LEFT_PAR elist SEMICOLON expr SEMICOLON elist RIGHT_PAR statement{;};

returnstmt:     RETURN SEMICOLON{;}
                | RETURN expr SEMICOLON{;}
                ;

%%

int yyerror (char* yaccProvideMessage){
    fprintf(stderr, "%s: at line %d before token: %s\n", yaccProvideMessage, yylineno, yytext);
    fprintf(stderr, "Input not valid!\n");
    return 0;
}
