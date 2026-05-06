#include "inter_code.h"

extern unsigned int tcount;
extern SymTable_T symtable;
extern int yylineno, scope;

int is_arith(expr* e)
{
	return (e->type == var_e || e->type == tableitem_e || e->type == arithexpr_e || e->type == assignexpr_e || e->type == constnum_e);
}

expr* inter_code_assign(expr* lval, expr* rval)
{
	if (lval->type == tableitem_e) {
		emit(tablesetelem, lval->index, rval, lval, 0, yylineno);
		expr* temp = emit_if_tableitem(lval);
		temp->type = assignexpr_e;
		return temp;
	} else {
		// 1st emit assigns the value to the variable
		emit(assign, rval, NULL, lval, 0, yylineno);
		// 2nd emit saves the result in a temp var
		// this allows multiple assignmments to happen
		// like x=y=z=1
		expr* temp = newtemp();
		temp->type = assignexpr_e;
		emit(assign, lval, NULL, temp, 0, yylineno);
		// return temp var
		return temp;
	}
}

expr* inter_code_arithmetic(expr* lval, expr* rval, iopcode op)
{
	if (!is_arith(lval) || !is_arith(rval)) {
		fprintf(stderr, "ERROR: ILLEGAL ARITHMETIC OPERATION AT LINE %d\n", yylineno);
		return NULL;
	}
	expr* temp_var = newtemp();
	emit(op, lval, rval, temp_var, 0, yylineno);
	return temp_var;
}