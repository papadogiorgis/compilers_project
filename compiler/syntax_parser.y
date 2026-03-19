%{
    #include <stdio.h>
    
    int yyerror (char* yaccProvideMessage);

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
%%

program:        stmts{;};

stmts:          stmts statement {;}
                | statement{;}
                ;

statement:      expr SEMICOLON {;}
                | ifstmt {;}
                | whilestmt {;}
                | forstmt {;}
                | returnstmt {;}
                | BREAK SEMICOLON {;}
                | CONTINUE SEMICOLON{;}
                | block{;}
                | funcdef{;}
                | SEMICOLON{;}
                ;


expr:           assignexpr{;}
                | expr PLUS expr{;}
                | expr MINUS expr{;}
                | expr MULT expr{;}
                | expr DIV expr{;}
                | expr MOD expr{;}
                | expr GR_THAN expr{;}
                | expr GREQ_THAN expr{;}
                | expr LESS_THAN expr{;}
                | expr LEQ_THAN expr{;}
                | expr EQ expr{;}
                | expr NOT_EQ expr{;}
                | expr AND expr{;}
                | expr OR expr{;}
                | term{;}
                ;

term:           LEFT_PAR expr RIGHT_PAR{;}
                | UMINUS expr{;}
                | NOT expr{;}
                | INCR lvalue{;}
                | lvalue INCR{;}
                | DECR lvalue{;}
                | lvalue DECR{;}
                | primary  {;}
                ;

assignexpr:     lvalue ASSIGN expr{;};

primary:        lvalue{;}
                | call{;}
                | objectdef{;}
                | funcdef {;} // to avoid crashing on foo = function(...){...} case
                | LEFT_PAR funcdef RIGHT_PAR{;}
                | const{;}
                ;

lvalue:         ID{;}
                | LOCAL ID{;}
                | DOUBLE_COLON ID{;}
                | member{;}
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

indexedelem:    LEFT_CURL_BR expr COLON expr RIGHT_CURL_BR{;}
                ;

block:          LEFT_CURL_BR RIGHT_CURL_BR{;}
                | LEFT_CURL_BR stmts RIGHT_CURL_BR{;}
                ;

funcdef:        FUNC ID LEFT_PAR idlist RIGHT_PAR block{fprintf(stderr, "function def\n"); }
                | FUNC LEFT_PAR idlist RIGHT_PAR block{;}
                ;

const:          INT{fprintf(stderr, "int: %d\n", yylval.intval);}
                | REAL{fprintf(stderr, "real: %f\n", yylval.floatval);}
                | STRING {fprintf(stderr, "string: \"%s\"\n", yylval.strval);}
                | NIL{;}
                | TRUE{;}
                | FALSE{;}
                // | ENDLINE{;}
                ;

idlist:         ID {;}
                | idlist COMMA ID{;}
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
    return -1;
}
