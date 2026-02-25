%{
    #include <string.h>
    #include "inc/tokens_list.h"
    #include <stdlib.h>

    #define YY_DECL int alpha_yylex(alpha_token_t *token)
    #define KEYWORD "KEYWORD"
    #define OP "OPERATOR"
    #define PUNC "PUNCTUATION"

    int tokencnt = 0;
    struct token_list token_list = {NULL};
    int commentcount=0, startline=0, endline=0;
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


{if}   {add_token(&token_list, yylineno, &tokencnt, "if", "KEYWORD", "IF");}
{else}   {add_token(&token_list, yylineno, &tokencnt, "else", "KEYWORD", "ELSE");}
{while}   {add_token(&token_list, yylineno, &tokencnt, "while", "KEYWORD", "WHILE");}
{for}   {add_token(&token_list, yylineno, &tokencnt, "for", "KEYWORD", "FOR");}
{function}   {add_token(&token_list, yylineno, &tokencnt, "function", "KEYWORD", "FUNCTION");}
{return}   {add_token(&token_list, yylineno, &tokencnt, "return", "KEYWORD", "RETURN");}
{break}   {add_token(&token_list, yylineno, &tokencnt, "break", "KEYWORD", "BREAK");}
{continue}   {add_token(&token_list, yylineno, &tokencnt, "continue", "KEYWORD", "CONTINUE");}
{and}   {add_token(&token_list, yylineno, &tokencnt, "and", "KEYWORD", "AND");}
{not}   {add_token(&token_list, yylineno, &tokencnt, "not", "KEYWORD", "NOT");}
{or}   {add_token(&token_list, yylineno, &tokencnt, "or", "KEYWORD", "OR");}
{local}   {add_token(&token_list, yylineno, &tokencnt, "local", "KEYWORD", "LOCAL");}
{true}   {add_token(&token_list, yylineno, &tokencnt, "true", "KEYWORD", "TRUE");}
{false}   {add_token(&token_list, yylineno, &tokencnt, "false", "KEYWORD", "FALSE");}
{nil}   {add_token(&token_list, yylineno, &tokencnt, "nil", "KEYWORD", "NIL");}


{equal}   {add_token(&token_list, yylineno, &tokencnt, "==", "OPERATOR", "EQUAL");}
{plus}   {add_token(&token_list, yylineno, &tokencnt, "+", "OPERATOR", "PLUS");}
{assign}   {add_token(&token_list, yylineno, &tokencnt, "=", "OPERATOR", "ASSIGNMENT");}
{minus}   {add_token(&token_list, yylineno, &tokencnt, "-", "OPERATOR", "MINUS");}
{mul}   {add_token(&token_list, yylineno, &tokencnt, "*", "OPERATOR", "MULTIPLICATION");}
{div}   {add_token(&token_list, yylineno, &tokencnt, "/", "OPERATOR", "DIVISION");}
{mod}   {add_token(&token_list, yylineno, &tokencnt, "%", "OPERATOR", "MOD");}
{noteq}   {add_token(&token_list, yylineno, &tokencnt, "!=", "OPERATOR", "NOT_EQUAL");}
{incr}   {add_token(&token_list, yylineno, &tokencnt, "++", "OPERATOR", "INCREMENT");}
{decr}   {add_token(&token_list, yylineno, &tokencnt, "--", "OPERATOR", "DECREMENT");}
{grthan}   {add_token(&token_list, yylineno, &tokencnt, ">", "OPERATOR", "GREATER_THAN");}
{lessthan}   {add_token(&token_list, yylineno, &tokencnt, "<", "OPERATOR", "LESS_THAN");}
{geq}   {add_token(&token_list, yylineno, &tokencnt, ">=", "OPERATOR", "GREATER_EQUAL");}
{leq}   {add_token(&token_list, yylineno, &tokencnt, "<=", "OPERATOR", "LESS_EQUAL");}


{semicol}   {add_token(&token_list, yylineno, &tokencnt, ";", "PUNCTUATION", "SEMICOLON");}
{comma}   {add_token(&token_list, yylineno, &tokencnt, ",", "PUNCTUATION", "COMMA");}
{colon}   {add_token(&token_list, yylineno, &tokencnt, ":", "PUNCTUATION", "COLON");}
{doublecolon}   {add_token(&token_list, yylineno, &tokencnt, "::", "PUNCTUATION", "DOUBLE_COLON");}
{dot}   {add_token(&token_list, yylineno, &tokencnt, ".", "PUNCTUATION", "DOT");}
{doubledot}   {add_token(&token_list, yylineno, &tokencnt, "..", "PUNCTUATION", "DOUBLE_DOT");}
{leftsqbrack}   {add_token(&token_list, yylineno, &tokencnt, "[", "PUNCTUATION", "LEFT_SQ_BAR");}
{rightsqbrack}   {add_token(&token_list, yylineno, &tokencnt, "]", "PUNCTUATION", "RIGHT_SQ_BAR");}
{leftcurlbr}   {add_token(&token_list, yylineno, &tokencnt, "{", "PUNCTUATION", "LEFT_CURL_BAR");}
{rightcurlbr}   {add_token(&token_list, yylineno, &tokencnt, "}", "PUNCTUATION", "RIGHT_CURL_BAR");}
{new_line} {yylineno;}
{leftpar}   {add_token(&token_list, yylineno, &tokencnt, "(", "PUNCTUATION", "LEFT_PAR");}
{rightpar}   {add_token(&token_list, yylineno, &tokencnt, ")", "PUNCTUATION", "RIGHT_PAR");}

{int} {add_token(&token_list, yylineno, &tokencnt, yytext, "CONST_INT", yytext);}
{real} {add_token(&token_list, yylineno, &tokencnt, yytext, "CONST_REAL", yytext);}
{id} {add_token(&token_list, yylineno, &tokencnt, yytext, "IDENTIFIER", yytext);}

"//" {
    char c;
    while ((c = input()) != EOF){
        if (c == '\n' || c == '\0'){
            break;
        }
    }
}

"/*" {
    commentcount++;
    startline = yylineno;
    char c;
    char buf[1024];

    while(commentcount > 0){
        c = input();
        if(c == EOF || c == '\0'){
            commentcount = -1;
            fprintf(stderr, "Block comment error!!\n");
            exit(-1);
            break;
        }

        if(c == '/'){
            if((c = input()) == '*'){
                commentcount++;
            }else{
                unput(c);
            }
        }else if(c == '*'){
            if((c = input()) == '/'){
                commentcount--;
                endline = yylineno;
            }else{
                unput(c);
            }
        }
    }
    sprintf(buf, "%d - %d", startline, endline);
    add_token(&token_list, yylineno, &tokencnt, buf, "COMMENT", "BLOCK_COMMENT");
}

"\"" {
    char c, lookAhead;
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


    add_token(&token_list, yylineno, &tokencnt, buf, "STRING", buf);
}



%%

int main (int argc, char **argv){
    if (argc > 1){
        if (!(yyin = fopen(argv[1], "r"))){
            fprintf(stderr, "cant open file\n");
            return 1;
        }
    }
    else {
        /* yyin = stdin; */
    }
    /* yylex();*/
    alpha_token_t t;
    alpha_yylex(&t);
    fprintf(stderr, "--------------- Lexical Analysis ---------------\n");
    print_list(&token_list);
    return 0;   
}

