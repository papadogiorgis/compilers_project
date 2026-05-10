#ifndef QUADS_H
#define QUADS_H

#include "symtable.h"
#include <stdio.h>

typedef enum expr_t {
	var_e,
	tableitem_e,
	programfunc_e,
	libraryfunc_e,
	arithexpr_e,
	boolexpr_e,
	assignexpr_e,
	newtable_e,
	constnum_e,
	constbool_e,
	conststring_e,

	nil_e,
} expr_t;

typedef struct stmt_t{
	int breaklist, contlist;
} stmt_t;

typedef struct expr {
	expr_t type;
	node* sym; // symbol
	struct expr* index;
	double numConst;
	char* strConst;
	unsigned char boolConst;
	struct expr* next;
} expr;

typedef enum iopcode {
	assign,
	add,
	sub,
	mul,
	division,
	mod,
	uminus,
	and_op,
	or_op,
	not_op,
	jump,
	if_eq,
	if_noteq,
	if_lesseq,
	if_greatereq,
	if_less,
	if_greater,
	call,
	param,
	ret,
	getretval,
	funcstart,
	funcend,
	tablecreate,
	tablegetelem,
	tablesetelem
} iopcode;

typedef struct quad {
	iopcode op;
	expr* result;
	expr* arg1;
	expr* arg2;
	unsigned int label;
	unsigned int line;
} quad;

typedef enum scopespace_t {
	programvar,
	functionlocal,
	formalarg
} scopespace_t;

enum symbol_t { var_s,
	programfunc_s,
	libraryfunc_s };

typedef struct forprefix {
	unsigned test;
	unsigned enter;
} forprefix;

typedef struct call_struct{
	expr* elist;
	unsigned char method;
	char* name;
}call_struct;

struct expr* newexpr(expr_t type);

void emit(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned label, unsigned line);

expr* newtemp(void);

void patchlist(int list, int label);

node* newtempsym();

expr* newexpr_constbool(int val);

void reset_temp_counter(void);

expr* emit_if_tableitem(expr* ex);

void resetFunctionLocalOffset(void);

void resetFormalArgOffset(void);

void restoreCurrScopeOffset(unsigned n);

unsigned nextquadlabel(void);

void patchLabel(unsigned quadNo, unsigned label);

expr* lvalue_expr(node* sym);

void enterscopespace(void);

void exitscopespace(void);

scopespace_t currscopespace(void);

unsigned int currscopeoffset(void);

void incurrscopeoffset(void);

void give_quads(FILE*);

int mergelist (int l1, int l2);

int newlist(int i);

void make_stmt (stmt_t* s);


#endif