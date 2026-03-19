%{
    #include <string.h>
    #include "inc/tokens_list.h"
    #include <stdlib.h>
    #include "syntax_parser.h"

    extern struct token_list token_list;

    #define ADD_KEYWORD(content, type) add_token(NULL, &token_list, yylineno, &tokencnt, content, "KEYWORD", type, "enumerated");
    #define ADD_OP(content, type) add_token(NULL, &token_list, yylineno, &tokencnt, content, "OPERATOR", type, "enumerated");
    #define ADD_PUNC(content, type) add_token(NULL, &token_list, yylineno, &tokencnt, content, "PUNCTUATION", type, "enumerated");


    int tokencnt = 0;
    int startline=0;
%}

%option noyywrap
%option yylineno
%option header-file="./scanner.h"

if "if"
else "else"
while "while"
for "for"
function "function"
return "return"
break "break"
continue "continue"
and "and"
not "not"
or "or"
local "local"
true "true"
false "false"
nil "nil"
assign "="
plus "+"
minus "-"
mul "*"
div "/"
mod "%"
equal "=="
noteq "!="
incr "++"
decr "--"
grthan ">"
lessthan "<"
geq ">="
leq "<="
int [0-9]+
real [0-9]+[.][0-9]+
id [a-zA-Z][a-zA-Z_0-9]*
leftcurlbr "{"
rightcurlbr "}"
leftsqbrack "["
rightsqbrack "]"
leftpar "("
rightpar ")"
semicol ";"
comma ","
colon ":"
doublecolon "::"
dot "."
doubledot ".."
new_line \n

%%

{if}   {ADD_KEYWORD("if", "IF"); return IF;}
{else}   {ADD_KEYWORD("else", "ELSE"); return ELSE;}
{while}   {ADD_KEYWORD("while", "WHILE"); return WHILE;}
{for}   {ADD_KEYWORD("for", "FOR"); return FOR;}
{function}   {ADD_KEYWORD("function", "FUNCTION"); return FUNC;}
{return}   {ADD_KEYWORD("return", "RETURN"); return RETURN;}
{break}   {ADD_KEYWORD("break", "BREAK"); return BREAK;}
{continue}   {ADD_KEYWORD("continue", "CONTINUE"); return CONTINUE;}
{and}   {ADD_KEYWORD("and", "AND"); return AND;}
{not}   {ADD_KEYWORD("not", "NOT"); return NOT;}
{or}   {ADD_KEYWORD("or", "OR"); return OR;}
{local}   {ADD_KEYWORD("local", "LOCAL"); return LOCAL;}
{true}   {ADD_KEYWORD("true", "TRUE"); return TRUE;}
{false}   {ADD_KEYWORD("false", "FALSE"); return FALSE;}
{nil}   {ADD_KEYWORD("nil", "NIL"); return NIL;} 


{equal}   {ADD_OP("==", "EQUAL"); return EQ;}
{plus}   {ADD_OP("+", "PLUS"); return PLUS;}
{assign}   {ADD_OP("=", "ASSIGNMENT"); return ASSIGN;}
{minus}   {ADD_OP("-", "MINUS"); return MINUS;}
{mul}   {ADD_OP("*", "MULTIPLICATION"); return MULT;}
{div}   {ADD_OP("/", "DIVISION");return DIV;}
{mod}   {ADD_OP("%", "MOD");return MOD;}
{noteq}   {ADD_OP("!=", "NOT_EQUAL"); return NOT_EQ;}
{incr}   {ADD_OP("++", "INCREMENT"); return INCR;}
{decr}   {ADD_OP("--", "DECREMENT");return DECR;}
{grthan}   {ADD_OP(">", "GREATER_THAN"); return GR_THAN;}
{lessthan}   {ADD_OP("<", "LESS_THAN"); return LESS_THAN;}
{geq}   {ADD_OP(">=", "GREATER_EQUAL"); return GREQ_THAN;}
{leq}   {ADD_OP("<=", "LESS_EQUAL"); return LEQ_THAN;}


{semicol}   {ADD_PUNC(";", "SEMICOLON"); return SEMICOLON;}
{comma}   {ADD_PUNC(",", "COMMA"); return COMMA;}
{colon}   {ADD_PUNC(":", "COLON"); return COLON;}
{doublecolon}   {ADD_PUNC("::", "DOUBLE_COLON"); return DOUBLE_COLON;}
{dot}   {ADD_PUNC(".", "DOT"); return DOT;}
{doubledot}   {ADD_PUNC("..", "DOUBLE_DOT"); return DOTDOT;}
{leftsqbrack}   {ADD_PUNC("[", "LEFT_SQ_BAR"); return SQ_BR_LEFT;}
{rightsqbrack}   {ADD_PUNC("]", "RIGHT_SQ_BAR"); return SQ_BR_RIGHT;}
{leftcurlbr}   {ADD_PUNC("{", "LEFT_CURL_BR"); return LEFT_CURL_BR;}
{rightcurlbr}   {ADD_PUNC("}", "RIGHT_CURL_BR"); return RIGHT_CURL_BR;}
{new_line} {/*return ENDLINE;*/}
[ \t\n]+ {/*ignores them*/}
{leftpar}   {ADD_PUNC("(", "LEFT_PAR"); return LEFT_PAR;}
{rightpar}   {ADD_PUNC(")", "RIGHT_PAR"); return RIGHT_PAR;}

{int} {
    yylval.intval = atoi(yytext);
    add_token(NULL, &token_list, yylineno, &tokencnt, yytext, "CONST_INT", yytext, "integer");
    return INT;
}
{real} {
    yylval.floatval = atof(yytext);
    add_token(NULL, &token_list, yylineno, &tokencnt, yytext, "CONST_REAL", yytext, "real");
    return REAL;
}
{id} {
    yylval.strval = strdup(yytext);
    add_token(NULL, &token_list, yylineno, &tokencnt, yytext, "IDENTIFIER", yytext, "char*");
    return ID;
}


"//" {
    int c;
    add_token(NULL,&token_list, yylineno, &tokencnt, "", "COMMENT", "LINE_COMMENT", "enumerated");
    while ((c = input()) != EOF){
        if (c == '\n' || c == '\0'){
            break;
        }
    }
}

"/*" {
    int *com_stack = malloc(sizeof(int) * 256);
    int stack_size = 256;
    int top = 1;
    int c;
    char buf[1024];

    com_stack[0] = yylineno;

    while(top > 0){
        c = input();

        if(c == EOF || c == '\0'){
            fprintf(stderr, "Block comment error. (line: %d)\n", yylineno);
            break;
        }

        if(c == '/'){
            if((c = input()) == '*'){
                if(top < stack_size){
                    com_stack[top++] = yylineno;
                }else{
                    // fprintf(stderr, "Warning: Comment nesting limit reached!!\n");
                    int *tmp = realloc(com_stack, (2 * stack_size)*sizeof(int));
                    if (tmp == NULL){
                      free(com_stack);
                      fprintf(stderr, "Memory error on comment stack.\n");
                      exit(-1);
                    }
                    com_stack = tmp;
                    stack_size = stack_size * 2;
                    com_stack[top++] = yylineno;
                }
            }else{
                unput(c);
            }
        }else if(c == '*'){
            if((c = input()) == '/'){
                startline = com_stack[--top];
                sprintf(buf, "%d - %d", startline, yylineno);

                if(top > 0){
                    add_token(NULL, &token_list, startline, &tokencnt, buf, "COMMENT", "NESTED_COMMENT", "enumerated");
                }else{
                    add_token(NULL, &token_list, startline, &tokencnt, buf, "COMMENT", "BLOCK_COMMENT", "enumerated");
                }
            }else{
                unput(c);
            }
        }
    }
}

"\"" {
    int c, lookAhead;
    unsigned int bufsize = 1024;
    char *buf = malloc(sizeof(char) * bufsize);
    int stringEnded = 0;
    int currBufSize = -1;  
    int breakFlag = 0; // will use it to continue from while if i have already written in buf a special char

    while (!stringEnded){
        c = input();
        currBufSize++;
        breakFlag = 0;
        if (c == EOF){
            fprintf(stderr, "String error.\n");
            exit(-1);
        }
        if (currBufSize == bufsize){
            char *tmp = realloc(buf, 2*bufsize);
            if (tmp == NULL) {
                fprintf(stderr, "Realloc failed\n");
                free(buf);
                exit(-1);
            }
            buf = tmp;
            bufsize = bufsize * 2;
        }

        if (c == '\"'){
            buf[currBufSize] = '\0'; //terminate string
            stringEnded = 1;
            continue;
        }
        if (c == '\\'){
            lookAhead = input();
            switch (lookAhead) {
                case 'n':
                    breakFlag = 1;
                    buf[currBufSize] = '\n';
                    break;
                case 't':
                    breakFlag = 1;
                    buf[currBufSize] = '\t';
                    break;
                case '\\':
                    breakFlag = 1;
                    buf[currBufSize] = '\\';
                    break;
                case '\"':
                    breakFlag = 1;
                    buf[currBufSize] = '\"';
                    break;
                case '0':
                    breakFlag = 1;
                    buf[currBufSize] = '\0';
                    break;
                default:
                    unput(lookAhead);
            }
        }
        if (breakFlag){
            continue;
        }
        else {
            buf[currBufSize] = c;
        }
    }

    add_token(NULL, &token_list, yylineno, &tokencnt, buf, "STRING", buf, "char*");
    yylval.strval = strdup(buf);
    return STRING;
}
%%