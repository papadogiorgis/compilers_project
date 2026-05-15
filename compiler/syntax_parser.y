%{
    #include <string.h>
    #include <stdio.h>

    #include "inc/symtable.h"
    #include "inc/quads.h"
    #include "inc/inter_code.h"
    #include "inc/stack.h"

    #define loopcounter (lcs_top->cnt)

    int yyerror (char* yaccProvideMessage);

    int scope = 0;
    int funcFlag = 0;
    int loopFlag = 0;
    // int loopcounter = 0;
    int infunc=0;
    int anonymousCnt = 0;
    char buf[1024]; 
    SymTable_T symtable;
    stack_t *stack;
    char **errors;
    int funcScope[1024] = {0};
    int errorsSize;
    lc_stack_t *lcs_top = NULL;
    lc_stack_t* lcs_bot = NULL;
    int err_count = 0;
    typedef struct ret_stack{
        int retlist;
        struct ret_stack* next;
    }ret_stack;
    ret_stack* ret_top = NULL;
    
    extern int yylex(void);

    extern int yylineno;
    extern char* yytext;
    extern FILE* yyin;
    extern unsigned currQuad;
%}

%union {
    int intval;
    char *strval;
    float floatval;
    unsigned uintval;
    struct expr *expression;
    struct node *node;
    struct stmt_t *stmt;
    struct forprefix *forprefix;
    struct call_struct* call_s;
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
%type <expression> lvalue expr term assignexpr primary call member const elist objectdef indexed indexedelem
%type <node> funcprefix funcdef
%type <uintval> funcbody ifprefix elseprefix  whilestart whilecond N M
%type <strval> funcname
%type <stmt> stmts statement ifstmt whilestmt forstmt loopstmt block
%type <forprefix> forprefix
%type <call_s> callsuffix normcall methodcall

%%

program:        stmts{printf("line %d: program -> stmts\n", yylineno);};

stmts:          stmts statement 
                    {
                        printf("line %d: stmts->stmts statement\n", yylineno);
                        $$ = malloc(sizeof(stmt_t));
                        make_stmt($$);
                        // printf("DEBUG: $1 is %p, $2 is %p\n", (void *)$1, (void*)$2);
                        int b1 = $1 ? $1->breaklist : 0;
                        int b2 = $2 ? $2->breaklist : 0;
                        int c1 = $1 ? $1->contlist : 0;
                        int c2 = $2 ? $2->contlist : 0;
                        $$->breaklist = mergelist(b1 , b2);
                        $$->contlist = mergelist(c1, c2);
                    }
                | statement
                    {
                        printf("line %d: stmts->statement\n", yylineno);\
                        $$ = $1;
                    }
                ;

statement:      expr SEMICOLON {
                    emit_if_tableitem($1);
                    reset_temp_counter();
                    $$ = malloc(sizeof(stmt_t));
                    $$->breaklist = 0;
                    $$->contlist = 0;
                    printf("line %d: statement->expr;\n", yylineno);}
                | ifstmt {
                    reset_temp_counter();
                    $$ = $1;
                    printf("line %d: statement->ifstmt\n", yylineno);}
                | whilestmt {
                    reset_temp_counter();
                    // $$ = $1;
                    $$ = 0;
                    printf("line %d: statement->whilestmt\n", yylineno);}
                | forstmt {
                    reset_temp_counter();
                    $$ = (void *)0;
                    printf("line %d: statement->forstmt\n", yylineno);}
                | returnstmt {
                    reset_temp_counter();
                    $$ = (void *)0;
                    printf("line %d: statement->returnstmt\n", yylineno);}
                | BREAK SEMICOLON {
                    reset_temp_counter();
                    // if (!$$){ $$ = malloc(sizeof(stmt_t));}
                    if (lcs_top && loopcounter == 0){
                        printf("\nError: use of break outside of loop line %d\n\n", yylineno);
                        err_count++;
                    }
                    printf("line %d: statement->break;\n", yylineno);
                    $$ = malloc(sizeof(stmt_t));
                    make_stmt($$);
                    $$->breaklist = newlist(nextquadlabel());
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                    }
                | CONTINUE SEMICOLON{
                    reset_temp_counter();
                    // if (!$$) {$$ = malloc(sizeof(stmt_t));}
                    if (lcs_top && loopcounter == 0){
                        printf("\nError: use of continue outside of loop, line %d\n\n", yylineno);
                        err_count++;
                    }
                    printf("line %d: statement->continue;\n", yylineno);
                    $$ = malloc(sizeof(stmt_t));
                    make_stmt($$);
                    $$->contlist = newlist(nextquadlabel());
                    emit(jump, NULL, NULL, NULL, 0 , yylineno);
                    }
                | block{printf("line %d: statement->block\n", yylineno);
                    // $$ = malloc(sizeof(stmt_t));
                    // make_stmt($$);
                    $$ = $1;
                }
                | funcdef{printf("line %d: statement->funcdef\n", yylineno);
                    // $$ = (void *)0;
                    // $$ = malloc(sizeof(stmt_t));
                    // make_stmt($$);
                    $$ = NULL;
                }
                | SEMICOLON{printf("line %d: statement->;\n", yylineno);
                    // $$ = (void *)0;
                    $$ = malloc(sizeof(stmt_t));
                    make_stmt($$);
                    $$ = 0;
                }
                ;


expr:           assignexpr{ 
                            $$=$1;
                            printf("line %d: expr->assignexpr\n", yylineno);
                        }
                | expr PLUS expr{   
                            $$ = inter_code_arithmetic($1, $3, add);
                            printf("line %d: expr->expr+expr\n", yylineno);
                        }
                | expr MINUS expr{  $$ = inter_code_arithmetic($1, $3, sub);
                                    printf("line %d: expr->exor-expr\n", yylineno);}
                | expr MULT expr{   $$ = inter_code_arithmetic($1, $3, mul);
                                    printf("line %d: expr->expr*expr\n", yylineno);}
                | expr DIV expr{    $$ = inter_code_arithmetic($1, $3, division);
                                    printf("line %d: expr->expr/expr\n", yylineno);}
                | expr MOD expr{    $$ = inter_code_arithmetic($1, $3, mod);
                                    printf("line %d: expr->expr MOD expr\n", yylineno);}
                | expr GR_THAN expr{
                        printf("line %d: expr->expr>expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_greater);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_greater, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr GREQ_THAN expr{
                        printf("line %d: expr->expr >= expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_greatereq);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_greatereq, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr LESS_THAN expr{
                        printf("line %d: expr->expr < expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_less);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_less, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr LEQ_THAN expr{
                        printf("line %d: expr->expr <= expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_lesseq);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_lesseq, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr EQ expr{
                        printf("line %d: expr->expr == expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_eq);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_eq, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr NOT_EQ expr{
                        printf("line %d: expr->expr != expr\n", yylineno);
                        $1 = emit_if_tableitem($1);
                        $3 = emit_if_tableitem($3);

                        $$ = newexpr(boolexpr_e);
                        inter_code_boolean_comparison($$, $1, $3, if_noteq);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(if_noteq, $1, $3, $$, nextquadlabel()+3, yylineno);
                        emit(assign, newexpr_constbool(0), NULL, $$, 0, yylineno);
                        emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
                        emit(assign, newexpr_constbool(1), NULL, $$, 0, yylineno);*/
                    }
                | expr AND {inter_make_bool_expr($1);} M expr{
                        printf("line %d: expr->expr and expr\n", yylineno);
                        // $$ = inter_code_bool($1, $3, and_op);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(and_op, $1, $3, $$, 0, yylineno);*/

                        inter_make_bool_expr($5);
                        patchlist($1->richtig_list, $4);
                        $$ = newexpr(boolexpr_e);
                        $$->sym = newtemp()->sym;
                        $$->richtig_list = $5->richtig_list;
                        $$->falsch_list = mergelist($1->falsch_list, $5->falsch_list);
                    }
                | expr OR {inter_make_bool_expr($1);} M expr{
                        printf("line %d: expr->expr or expr\n", yylineno);
                        /*$$ = newtemp();
                        $$->type = boolexpr_e;
                        emit(or_op, $1, $3, $$, 0, yylineno);*/

                        inter_make_bool_expr($5);
                        patchlist($1->falsch_list, $4);
                        $$ = newexpr(boolexpr_e);
                        $$->sym = newtemp()->sym;
                        $$->richtig_list = mergelist($1->richtig_list, $5->richtig_list);
                        $$->falsch_list = $5->falsch_list;
                    }
                | term{printf("line %d: expr->term\n", yylineno);}
                ;

term:           LEFT_PAR expr RIGHT_PAR{$$=$2; printf("line %d: term-> (expr)\n", yylineno);}
                | MINUS expr %prec UMINUS{
                    printf("line %d: term-> -expr\n", yylineno);
                    $$ = inter_code_uminus($2);}
                | NOT expr{
                    inter_make_bool_expr($2);
                    $$ = newexpr(boolexpr_e);
                    $$->sym = newtemp()->sym;
                    $$->richtig_list = $2->falsch_list;
                    $$->falsch_list = $2->richtig_list;
                    printf("line %d: term-> not expr\n", yylineno);}
                | INCR lvalue{
                    if($2->sym != NULL){ 
                        node *tmp = getSymbol($2->sym->key, symtable);
                        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)) {
                            printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $2->sym->key, yylineno);
                            err_count++;
                        }
                    }
                    expr* lval = emit_if_tableitem($2);
                    expr* tempnum = newexpr(constnum_e);
                    tempnum->numConst = 1;
                    emit(add, lval, tempnum, lval, 0, yylineno);
                    if($2->type == tableitem_e){
                        emit(tablesetelem, $2->index, lval, $2, 0, yylineno);
                    }
                    expr* term = newtemp();
                    emit(assign, lval, NULL, term, 0, yylineno);
                    $$ = term;
                    printf("line %d: term-> ++lvalue\n", yylineno);}
                | lvalue INCR{
                    if($1->sym != NULL){
                        node *tmp = getSymbol($1->sym->key, symtable);
                        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)){
                            printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1->sym->key, yylineno);
                            err_count++;
                        }
                    }
                    expr* old = newtemp();
                    expr* lval = emit_if_tableitem($1);
                    emit(assign, lval, NULL, old, 0, yylineno);
                    expr* tempnum = newexpr(constnum_e);
                    tempnum->numConst = 1;
                    emit(add, lval, tempnum, lval, 0, yylineno);
                    if($1->type == tableitem_e){
                        emit(tablesetelem, $1->index, lval, $1, 0, yylineno);
                    }
                    $$ = old;
                    printf("line %d: term-> lvalue++\n", yylineno);}
                | DECR lvalue{
                    if($2->sym != NULL){
                        node *tmp = getSymbol($2->sym->key, symtable);
                        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)){
                            printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $2->sym->key, yylineno);
                            err_count++;
                        }
                    }
                    expr* lval = emit_if_tableitem($2);
                    expr* tempnum = newexpr(constnum_e);
                    tempnum->numConst = 1;
                    emit(sub, lval, tempnum, lval, 0, yylineno);
                    if($2->type == tableitem_e){
                        emit(tablesetelem, $2->index, lval, $2, 0, yylineno);
                    }
                    expr* term = newtemp();
                    emit(assign, lval, NULL, term, 0, yylineno);
                    $$ = term;
                    printf("line %d: term-> --lvalue\n", yylineno);}
                | lvalue DECR{
                    if($1->sym != NULL){
                        node *tmp = getSymbol($1->sym->key, symtable);
                        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)){
                            printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1->sym->key, yylineno);
                            err_count++;
                        }
                    }
                    expr* old = newtemp();
                    expr* lval = emit_if_tableitem($1);
                    emit(assign, lval, NULL, old, 0, yylineno);
                    expr* tempnum = newexpr(constnum_e);
                    tempnum->numConst = 1;
                    emit(sub, lval, tempnum, lval, 0, yylineno);
                    if($1->type == tableitem_e){
                        emit(tablesetelem, $1->index, lval, $1, 0, yylineno);
                    }
                    $$ = old;
                    printf("line %d: term-> lvalue--\n", yylineno);}
                | primary  {$$=$1; printf("line %d: term->primary\n", yylineno);}
                ;

assignexpr:     lvalue ASSIGN expr{
                    if($1->sym != NULL){
                        node *tmp = getSymbol($1->sym->key, symtable);
                        if (tmp != NULL && (tmp->type == USERFUNC || tmp->type == LIBFUNC)){
                                printf("\nError: using func as lvalue, symbol:%s line:%d\n\n", $1->sym->key, yylineno);
                                err_count++;
                        }
                    }
                    $$ = inter_code_assign($1, $3);
                    printf("line %d: assignexpr->lvalue = expr\n", yylineno);
                }
                | call ASSIGN expr {
                    printf("\nError: function call is not lvalue, line:%d\n\n", yylineno);
                    err_count++;
                    $$ = newexpr(nil_e);
                }
                ;

primary:        lvalue{$$=$1; printf("line %d: primary->lvalue\n", yylineno);}
                | call{$$=$1; printf("line %d: primary->call\n", yylineno);}
                | objectdef{
                    $$ = $1;
                    printf("line %d: primary->objectdef\n", yylineno);}
                /* | funcdef {printf("line %d: primary->funcdef\n", yylineno);} // to avoid crashing on foo = function(...){...} case */
                | LEFT_PAR funcdef RIGHT_PAR{
                    $$ = lvalue_expr($2);
                    $$->type = programfunc_e;
                    printf("line %d: primary->(funcdef)\n", yylineno);}
                | const{$$=$1; printf("line %d: primary->const\n", yylineno);}
                ;

lvalue:         ID{
                    node* symbol = getSymbol($1, symtable);
                    if (symbol == NULL){
                        if (scope == 0) {
                            symbol = SymTable_put(symtable, $1,$1, GLOBAL, scope, yylineno,0, currscopeoffset());incurrscopeoffset();} 
                        else {symbol = SymTable_put(symtable, $1,$1, LOCALV, scope, yylineno,0,currscopeoffset());incurrscopeoffset();}; 
                    } else {
                        if (findScope($1,symtable) != 0 && findScope($1,symtable) != scope && funcScope[scope] != funcScope[findScope($1,symtable)]){
                            printf("\nError: var not accesible, symbol: %s line: %d\n\n", $1, yylineno);
                            err_count++;
                        }
                    };
                    $$ = newexpr(var_e);
                    $$->sym = symbol;
                printf("line %d: lvalue->id\n", yylineno);}


                | LOCAL ID{
                    node *tmp = getSymbol($2, symtable);
                    if(tmp!=NULL && tmp->type == LIBFUNC){
                        printf("\nError: func shadows lib func, symbol: %s line: %d\n\n", $2, yylineno);
                        err_count++;
                    }else{
                        tmp = SymTable_put(symtable, $2,$2, LOCALV, scope, yylineno,1,currscopeoffset());
                        incurrscopeoffset();
                    }
                    $$ = newexpr(var_e);
                    $$->sym = tmp;
                    printf("line %d: lvalue->local id\n", yylineno);}
                | DOUBLE_COLON ID{
                    node* tmp = getSymbolScope($2, symtable, 0);
                    if (tmp == NULL){
                        printf("\nError: global var doesnt exist, symbol: %s line: %d\n\n", $2, yylineno);
                        err_count++;
                    }
                    $$ = newexpr(var_e);
                    $$->sym = tmp;
                    printf("line %d: lvalue-> ::id\n", yylineno);}
                | member{$$ = $1; printf("line %d: lvalue-> member\n", yylineno);}
                ;

member:         lvalue DOT ID{
                    expr* index = newexpr(conststring_e);
                    index->strConst = strdup($3);
                    $$ = inter_code_member_item($1, index);
                    printf("line %d: member-> lvalue.ID\n", yylineno);}
                | lvalue SQ_BR_LEFT expr SQ_BR_RIGHT{
                    $$ = inter_code_member_item($1, $3);
                    printf("line %d: member-> lvalue{expr}\n", yylineno);}
                | call DOT ID{
                    expr* index = newexpr(conststring_e);
                    index->strConst = strdup($3);
                    $$ = inter_code_member_item($1, index);
                    printf("line %d: member-> call.ID\n", yylineno);}
                | call SQ_BR_LEFT expr SQ_BR_RIGHT{
                    $$ = inter_code_member_item($1, $3);
                    printf("line %d: member-> call{expr}\n", yylineno);}
                ;

call:           call LEFT_PAR elist RIGHT_PAR{
                    $$ = inter_code_call($1, $3);
                    printf("line %d: call-> call(elist)\n", yylineno);}
                | lvalue callsuffix{
                    $$ = inter_code_callsuffix($1, $2);
                    printf("line %d: call-> lvalue callsuffix\n", yylineno);}
                | LEFT_PAR funcdef RIGHT_PAR LEFT_PAR elist RIGHT_PAR{
                    expr* f = newexpr(programfunc_e);
                    f->sym = $2;
                    $$ = inter_code_call(f, $5);
                    printf("line %d: call-> (funcdef)(elist)\n", yylineno);}
                ;

callsuffix:     normcall{
                    $$ = $1;
                    printf("line %d: callsuffix-> normcall\n", yylineno);}
                | methodcall{
                    $$ = $1;
                    printf("line %d: callsuffix-> methodcall\n", yylineno);}
                ;

normcall:       LEFT_PAR elist RIGHT_PAR{
                    $$ = malloc(sizeof(call_struct));
                    $$->elist = $2;
                    $$->method = 0;
                    $$->name = NULL;
                    printf("line %d: normcall-> (elist)\n", yylineno);};

methodcall:     DOTDOT ID LEFT_PAR elist RIGHT_PAR {
                    $$ = malloc(sizeof(call_struct));
                    $$->elist = $4;
                    $$->method = 1;
                    $$->name = strdup($2);
                    printf("line %d: methodcall-> ..ID(elist)\n", yylineno);};

elist:          expr{
                    $1->next = NULL;
                    $$=$1;
                    printf("line %d: elist-> expr\n", yylineno);}
                | expr COMMA elist{
                    $1->next = $3;
                    $$ = $1;
                    printf("line %d: elist-> expr,elist\n", yylineno);}
                | { $$ = NULL;
                    printf("line %d: elist-> EMPTY RULE\n", yylineno);}
                ;

objectdef:      SQ_BR_LEFT elist SQ_BR_RIGHT{
                    $$ = inter_code_objectdef_elist($2);
                    printf("line %d: objectdef-> [elist]\n", yylineno);}
                | SQ_BR_LEFT indexed SQ_BR_RIGHT{
                    $$ = inter_code_objectdef_indexed($2);
                    printf("line %d: objectdef-> [indexed]\n", yylineno);}
                ;
            
indexed:        indexedelem{
                    $1->next = NULL;
                    $$=$1;
                    printf("line %d: indexed-> indexedelem\n", yylineno);}
                | indexedelem COMMA indexed{
                    $1->next = $3;
                    $$ = $1;
                    printf("line %d: indexed-> indexedelem,indexed\n", yylineno);}
                ;

indexedelem:    LEFT_CURL_BR expr COLON expr RIGHT_CURL_BR{
                    $$ = newexpr(nil_e);
                    $$->index = $2;
                    $$->value = $4;
                    $$->next = NULL;
                    printf("line %d: indexedelem-> {expr:expr}\n", yylineno);
                    }
                ;


block:          LEFT_CURL_BR {scope++;} RIGHT_CURL_BR {hideScope(scope--); printf("line %d: block-> {}\n", yylineno);} 
                | LEFT_CURL_BR {scope++;} stmts RIGHT_CURL_BR {$$ = $3; hideScope(scope--); printf("line %d: block-> {stmts}\n", yylineno);}

funcname:       ID {$$ = $1;} | 
                {sprintf(buf, "$%d", anonymousCnt++);
                $$ = buf;};

funcprefix:     FUNC funcname 
                {
                    $$ = SymTable_put(symtable, $2, $2, USERFUNC, scope, yylineno, 0, 0);
                    $$->iaddress = nextquadlabel() + 1;
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                    emit(funcstart,  NULL, NULL,lvalue_expr($$), 0, yylineno);
                    stack_push(stack, currscopeoffset());
                    // scope++;
                    //funcFlag = 1;
                    // enterscopespace();
                    resetFormalArgOffset();
                    infunc++;

                    ret_stack* new_ret = malloc(sizeof(ret_stack));
                    new_ret->retlist = 0;
                    new_ret->next = ret_top;
                    ret_top = new_ret;
                };

funcargs:       LEFT_PAR {scope++;} idlist {scope--;} RIGHT_PAR
                {
                    // enterscopespace();
                    resetFunctionLocalOffset();
                };

funcbody:       funcblockstart block funcblockend
                {
                    $$ = currscopeoffset();
                    // exitscopespace();
                };

funcblockstart: {push_loopcounter();};

funcblockend:   {pop_loopcounter();};

funcdef:        funcprefix funcargs funcbody
                {
                    infunc--;
                    int offset = pop_and_top(stack);
                    restoreCurrScopeOffset(offset);
                    patchLabel($1->iaddress - 1, nextquadlabel());
                    $1->totalLocals = $3;
                    // exitscopespace();
                    if(ret_top){
                        patchlist(ret_top->retlist, nextquadlabel());
                        ret_stack* temp = ret_top;
                        ret_top = ret_top->next;
                        free(temp);
                    }
                    emit(funcend, NULL, NULL, lvalue_expr($1), 0, yylineno);
                    $$ = $1;
                    //emit(funcend, NULL, NULL, lvalue_expr($1), 0, yylineno);

                };

const:          INT
                    {
                        $$ = newexpr(constnum_e);
                        $$->numConst = (float)$1;
                        printf("line %d: const-> int\n", yylineno);
                    }
                | REAL
                    {
                        $$ = newexpr(constnum_e);
                        $$->numConst = $1;
                        printf("line %d: const-> real\n", yylineno);
                    }
                | STRING 
                    {
                        $$ = newexpr(conststring_e);
                        $$->strConst = strdup($1);
                        printf("line %d: const-> string\n", yylineno);
                    }
                | NIL
                    {
                        $$ = newexpr(nil_e);
                        printf("line %d: const-> nil\n", yylineno);
                    }
                | TRUE
                    {
                        $$ = newexpr(constbool_e);
                        $$->boolConst = 1;
                        printf("line %d: const-> true\n", yylineno);
                    }
                | FALSE
                    {
                        $$ = newexpr(constbool_e);
                        $$->boolConst = 0;
                        printf("line %d: const-> false\n", yylineno);
                    }
                // | ENDLINE{;}
                ;

idlist:         ID {SymTable_put(symtable, $1, $1, FORMAL, scope, yylineno, 0, currscopeoffset());incurrscopeoffset(); printf("line %d: idlist-> ID\n", yylineno);}
                | idlist COMMA ID{SymTable_put(symtable, $3, $3, FORMAL, scope, yylineno, 0,currscopeoffset()); incurrscopeoffset(); printf("line %d: idlist-> idlist, ID\n", yylineno);}
                | {printf("line %d: idlist-> EMPTY RULE\n", yylineno);}
                ;

ifprefix:       IF LEFT_PAR expr RIGHT_PAR
                {
                    //emit(if_eq, $3, NULL,  newexpr_constbool(1), nextquadlabel() + 2,yylineno);
                    expr* cond = inter_code_bool_to_val($3);
                    emit(if_eq, cond, newexpr_constbool(1), NULL, nextquadlabel() + 2, yylineno);
                    $$ = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                };

elseprefix:     ELSE
                {
                    $$ = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                };

ifstmt:         ifprefix statement
                {
                    printf("line %d: ifstmt-> if(expr) statement\n", yylineno);
                    patchLabel($1, nextquadlabel());
                    $$ = $2;
                } 
                | ifprefix statement elseprefix statement
                {
                    printf("line %d: ifstmt-> if(expr) statement else statement\n", yylineno);
                    patchLabel($1, $3 + 1);
                    patchLabel($3, nextquadlabel());

                    /* For the ifstmt, create a new statement that will contain the merged
                        break and continue lists of the statements that are inside the if/else,
                        so that if there are conditional breaks/continues the jump quad that 
                        is emitted can be back patched. 

                        for example in the cases of: if (b) break; / if (c) continue;
                     */
                    stmt_t *new_stmt = malloc(sizeof(stmt_t));
                    make_stmt(new_stmt);

                    int b1 = $2 ? $2->breaklist : 0;
                    int b2 = $4 ? $4->breaklist : 0;
                    int c1 = $2 ? $2->contlist : 0;
                    int c2 = $4 ? $4->contlist : 0;


                    new_stmt->breaklist = mergelist(b1, b2);
                    new_stmt->contlist = mergelist(c1, c2);
                    $$ = new_stmt;
                };

loopstart:      {if(lcs_top){++loopcounter;}};

loopend:        {if(lcs_top){--loopcounter;}};

loopstmt:       loopstart statement loopend {$$ = $2;};

whilestart:     WHILE 
                {
                    $$ = nextquadlabel();
                };

whilecond:      LEFT_PAR expr RIGHT_PAR
                {
                    //emit(if_eq, $2, newexpr_constbool(1), NULL, nextquadlabel() + 2, yylineno);
                    expr* cond = inter_code_bool_to_val($2);
                    emit(if_eq, cond, newexpr_constbool(1), NULL, nextquadlabel() + 2, yylineno);
                    $$ = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0 ,yylineno);
                };

/* whilestmt:      whilestart whilecond stmts */
whilestmt:      whilestart whilecond loopstmt
                {
                    emit(jump, NULL, NULL, NULL, $1, yylineno);
                    patchLabel($2, nextquadlabel());
                    if ($3){
                        patchlist($3->breaklist, nextquadlabel());
                        patchlist($3->contlist, $1);
                    }
                };

N:              {$$ = nextquadlabel(); emit(jump,NULL, NULL, NULL, 0, yylineno);};

M:              {$$ = nextquadlabel();}

forprefix:      FOR LEFT_PAR elist SEMICOLON M expr SEMICOLON
                {
                    $$ = malloc(sizeof(forprefix));
                    $$->test = $5;
                    $$->enter = nextquadlabel();
                    expr* cond = inter_code_bool_to_val($6);
                    emit(if_eq, cond, newexpr_constbool(1), NULL, 0, yylineno);
                    //emit(if_eq, newexpr_constbool(1), $6, NULL, 0, yylineno);
                    $$->falselist = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                };

/* forstmt:        FOR {loopFlag = 1;} LEFT_PAR elist SEMICOLON expr SEMICOLON elist RIGHT_PAR statement{loopFlag = 0;printf("line %d: forstmt-> for(elist;expr;elist) statement\n", yylineno);}; */

forstmt:        forprefix elist RIGHT_PAR N loopstmt N
                {
                    int stepquad = ($1 ? $1->falselist : 0) + 1;
                    patchLabel($1 ? $1->enter : 0, $4 + 1);
                    patchLabel($4, $1 ? $1->test : 0);
                    patchLabel($6, stepquad);

                    patchLabel($1 ? $1->falselist : 0, nextquadlabel());

                    patchlist($5 ? $5->breaklist: 0, nextquadlabel());
                    patchlist($5 ? $5->contlist: 0, stepquad);
                };

returnstmt:     RETURN SEMICOLON{
                    if (infunc == 0){
                        printf("\nError: use of return outside of function, line %d\n", yylineno);
                        err_count++;
                    }
                    emit(ret, NULL, NULL, NULL, 0, yylineno);
                    int j = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                    if(ret_top){
                        ret_top->retlist = mergelist(ret_top->retlist, newlist(j));
                    }
                    printf("line %d: returnstmt-> return;\n", yylineno);}
                | RETURN expr SEMICOLON{
                    if (infunc == 0){
                        printf("\nError: use of return outside of function, line %d\n", yylineno);
                        err_count++;
                    }
                    expr* retval = emit_if_tableitem($2);
                    emit(ret, retval, NULL, NULL, 0, yylineno);
                    int j = nextquadlabel();
                    emit(jump, NULL, NULL, NULL, 0, yylineno);
                    if(ret_top){
                        ret_top->retlist = mergelist(ret_top->retlist, newlist(j));
                    }
                    printf("line %d: returnstmt-> return expr;\n", yylineno);}
                ;

%%

int yyerror (char* yaccProvideMessage){
    fprintf(stderr, "%s: at line %d before token: %s\n", yaccProvideMessage, yylineno, yytext);
    fprintf(stderr, "Input not valid!\n");
    err_count++;
    return 0;
}