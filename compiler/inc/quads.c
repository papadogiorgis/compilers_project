/**
 * Code for quad generation
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quads.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE (total * sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE * sizeof(quad) + CURR_SIZE)

unsigned currQuad = 1, total = 1;
unsigned int programVarOffset = 0;
unsigned int functionLocalOffset = 0;
unsigned int formalArgOffset = 0;
unsigned int scopeSpaceCounter = 1;
unsigned int tcount = 0;

extern SymTable_T symtable;
extern int yylineno, scope;
quad* quads = NULL;

void expand(void)
{
	assert(total == currQuad);
	quad* p = (quad*)malloc(NEW_SIZE);
	if (quads) {
		memcpy(p, quads, CURR_SIZE);
		free(quads);
	}
	quads = p;
	total += EXPAND_SIZE;
}

void emit(iopcode op, expr* arg1, expr* arg2, expr* result, unsigned label, unsigned line)
{
	if (currQuad == total)
		expand();
	quad* p = quads + currQuad++;
	p->op = op;
	p->arg1 = arg1;
	p->arg2 = arg2;
	p->result = result;
	p->label = label;
	p->line = line;
}

expr* newexpr(expr_t type)
{
	expr* tmp = malloc(sizeof(expr));
	tmp->type = type;
	return tmp;
}

scopespace_t currscopespace(void)
{
	if (scopeSpaceCounter == 1) {
		return programvar;
	} else if (scopeSpaceCounter % 2 == 0) {
		return formalarg;
	} else {
		return functionlocal;
	}
}

unsigned int currscopeoffset(void)
{
	switch (currscopespace()) {
	case programvar:
		return programVarOffset;
	case functionlocal:
		return functionLocalOffset;
	case formalarg:
		return formalArgOffset;
	default:
		assert(0);
	}
}

void incurrscopeoffset(void)
{
	switch (currscopespace()) {
	case programvar:
		++programVarOffset;
		break;
	case functionlocal:
		++functionLocalOffset;
		break;
	case formalarg:
		++formalArgOffset;
		break;
	default:
		assert(0);
	}
}

void enterscopespace(void)
{
	++scopeSpaceCounter;
}

void exitscopespace(void)
{
	assert(scopeSpaceCounter > 1);
	--scopeSpaceCounter;
}

void resetFunctionLocalOffset(void) { functionLocalOffset = 0; }

void resetFormalArgOffset(void) { formalArgOffset = 0; }

void restoreCurrScopeOffset(unsigned n)
{
	switch (currscopespace()) {
	case programvar:
		programVarOffset = n;
		break;
	case functionlocal:
		functionLocalOffset = n;
		break;
	case formalarg:
		formalArgOffset = n;
		break;
	default:
		assert(0);
	}
}

unsigned nextquadlabel(void)
{
	return currQuad;
}

void patchLabel(unsigned quadNo, unsigned label)
{
	printf("quadNo: %d, currQuad: %d\n", quadNo, currQuad);
	if (label != 0){
		assert(quadNo < currQuad && !quads[quadNo].label); 
	}
	quads[quadNo].label = label;
}

void patchlist(int list, int label)
{
	while (list){
		int next = quads[list].label;
		quads[list].label = label;
		list = next;
	}
}

void make_stmt (stmt_t* s)
{
	s->breaklist = s->contlist = 0;
}

int newlist(int i)
{
	quads[i].label = 0; return i;
}

int mergelist (int l1, int l2)
{
	if (!l1){
		return l2;
	}
	else if(!l2){
		return l1;
	}
	else {
		int i = l1;
		while (quads[i].label){
			i = quads[i].label;
		}
		quads[i].label = l2;
		return l1;
	}
}

/**
 * Make lvalue epxression out of symbol entry
 */
expr* lvalue_expr(node* sym)
{
	assert(sym);
	expr* e = (expr*)malloc(sizeof(expr));
	memset(e, 0, sizeof(expr));

	e->next = (expr*)0;
	e->sym = sym;

	switch (sym->type) {
	case var_s:
		e->type = var_e;
		break;
	case USERFUNC:
	case programfunc_s:
		e->type = programfunc_e;
		break;
	case LIBFUNC:
	case libraryfunc_s:
		e->type = libraryfunc_e;
		break;
	default:
		assert(0);
	}

	return e;
}

/*node* newtempsym()
{
	node* tmp;
	char* name = malloc(24);
	sprintf(name, "_t%d", tcount++);
	tmp = getSymbol(name, symtable);
	if (!tmp) {
		tmp = SymTable_put(symtable, name, name, LOCALV, scope, yylineno, 0, currscopeoffset());
		incurrscopeoffset();
		return tmp;
	}
	return NULL;
}*/

expr* newtemp()
{
	char* name = malloc(24);
	sprintf(name, "_t%d", tcount++);

	/* First, check if there is an existent temp var with this name
	  in the current scope, so we dont mess the temp vars of
	  different functions, like in this case:
	  z = 10 + 10 + sqrt(49);
	  the function sqrt() will change the _t0 data if we use
	  the function getSymbol and not getSymbolScope */
	node* sym = getSymbolScope(name, symtable, scope);

	if (sym == NULL) { /* If there is no temp var
						with this name in the current scope,
					we add one in the symtable */
		sym = SymTable_put(symtable, name, name, LOCALV, scope, yylineno, 0, currscopeoffset());
		incurrscopeoffset(); /*the reason we use this is to
		 allocate a unique memory slot for the new temp var*/
	}

	/* Then we assign the sym we found with getsymbolscope
	  (or we created inside the if)
	  in a new expr, and return it. */
	expr* e = newexpr(var_e);
	e->sym = sym;
	return e;
}

void reset_temp_counter()
{
	tcount = 0;
}

expr* emit_if_tableitem(expr* ex)
{
	if (ex->type != tableitem_e) {
		return ex;
	}
	expr* result = newtemp();
	result->type = var_e;
	emit(tablegetelem, ex, ex->index, result, 0, yylineno);
	return result;
}

expr* newexpr_constbool(int val)
{
	expr* e = newexpr(constbool_e);
	if (val == 1) {
		e->boolConst = 1;
	} else if (val == 0) {
		e->boolConst = 0;
	} else {
		assert(0);
	}
	return e;
}

const char* opcode_to_str(iopcode op)
{
	switch (op) {
	case assign:
		return "ASSIGN";
	case add:
		return "ADD";
	case sub:
		return "SUB";
	case mul:
		return "MUL";
	case division:
		return "DIV";
	case mod:
		return "MOD";
	case uminus:
		return "UMINUS";
	case and_op:
		return "AND";
	case or_op:
		return "OR";
	case not_op:
		return "NOT";
	case jump:
		return "JUMP";
	case if_eq:
		return "JE"; // jump if equal
	case if_noteq:
		return "JNE"; // jump if not equal
	case if_lesseq:
		return "JLE"; // jump if less equal
	case if_greatereq:
		return "JGE"; // jump if greater equal
	case if_less:
		return "JL"; // jump if less
	case if_greater:
		return "JG"; // jump if greater
	case call:
		return "CALL";
	case param:
		return "PARAM";
	case ret:
		return "RET";
	case getretval:
		return "RETVAL";
	case funcstart:
		return "FUNCSTART";
	case funcend:
		return "FUNCEND";
	case tablecreate:
		return "TABCREATE";
	case tablegetelem:
		return "TABGETEL";
	case tablesetelem:
		return "TABSETEL";
	default:
		assert(0);
	}
}

/**
 * Use type of expression to return string for printing quads.
 */
const char* expr_to_str(expr* e)
{
	if (!e)
		return "";
	static char buf[2048];

	switch (e->type) {
	case var_e:
		if (e->sym && e->sym->key)
			return e->sym->key;
		return "anonymous_var";
	case arithexpr_e:
	case assignexpr_e:
		if (e->sym && e->sym->key)
			return e->sym->key;
		return "anonymous_var";
    case tableitem_e:
        if(e->sym && e->sym->key){
            return e->sym->key;
        }
        return "anonymous_var";
    case newtable_e:
        if(e->sym && e->sym->key){
            return e->sym->key;
        }
        return "anonymous_var";
	case constnum_e:
		sprintf(buf, "%g", e->numConst);
		return buf;
	case constbool_e:
		return e->boolConst ? "true" : "false";
	case conststring_e:
		sprintf(buf, "\"%s\"", e->strConst);
		return buf;
	case nil_e:
		return "NIL";
	case programfunc_e:
		// return "UFUNC";
		return e->sym->key;
	case libraryfunc_e:
		// return "LIBFUNC";
		return e->sym->key;
	case boolexpr_e:
		if (e->sym) {
			return e->sym->key;
		} else {
			return 0;
		}
	default:
		assert(0);
	}
}

void give_quads(FILE* quads_txt)
{
	fprintf(quads_txt, "quad#   opcode          result          arg1            arg2            label \n");
	fprintf(quads_txt, "------------------------------------------------------------------------------\n");
	for (int i = 1; i < currQuad; i++) {
		quad* q = &quads[i];

		fprintf(quads_txt, "%-7d ", i);
		fprintf(quads_txt, "%-15s ", opcode_to_str(q->op));
		fprintf(quads_txt, "%-15s ", expr_to_str(q->result));
		fprintf(quads_txt, "%-15s ", expr_to_str(q->arg1));
		fprintf(quads_txt, "%-15s ", expr_to_str(q->arg2));

		if (q->label > 0) {
			fprintf(quads_txt, "%-10d\n", q->label);
		} else {
			fprintf(quads_txt, "\n");
		}
	}
}