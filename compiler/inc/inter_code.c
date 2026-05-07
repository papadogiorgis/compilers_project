#include "inter_code.h"

extern unsigned int tcount;
extern SymTable_T symtable;
extern int yylineno, scope;
extern int currQuad;

int is_arith(expr* e)
{
	return (e->type == var_e || e->type == tableitem_e || e->type == arithexpr_e || e->type == assignexpr_e || e->type == constnum_e);
}

expr* inter_code_assign(expr* lval, expr* rval)
{
	if (lval->type == tableitem_e) {
		emit(tablesetelem, lval, lval->index, rval, 0, yylineno);
		expr* temp = emit_if_tableitem(lval);
		temp->type = assignexpr_e;
		return temp;
	} else {
		//if rval is a table, bring its value
		rval = emit_if_tableitem(rval);
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
	lval = emit_if_tableitem(lval);
	rval = emit_if_tableitem(rval);

	expr* temp_var = newtemp();
	emit(op, lval, rval, temp_var, 0, yylineno);
	return temp_var;
}

expr *inter_code_bool (expr *lval, expr* rval, iopcode op)
{
	lval = emit_if_tableitem(lval);
	rval = emit_if_tableitem(rval);
	
	expr *tmp = newtemp();
	emit(op, lval, rval, tmp, currQuad+2, yylineno);
	return tmp;
}

expr* inter_code_objectdef_elist(expr* e){
	expr* temp = newtemp();
	temp->type = newtable_e;
	emit(tablecreate, temp, NULL, NULL, 0, yylineno);

	int i = 0;
	expr* elist_element = e;
	while(elist_element != NULL){
		expr* arith = newexpr(constnum_e);
		arith->numConst = i;
		emit(tablesetelem, temp, arith, elist_element, 0, yylineno);
		elist_element = elist_element->next;
		i++;
	}
	return temp;
}

expr* inter_code_objectdef_indexed(expr* e){
	expr* temp = newtemp();
	temp->type = newtable_e;
	emit(tablecreate, temp, NULL, NULL, 0, yylineno);

	expr* elist_element = e;
	while(elist_element != NULL){
		emit(tablesetelem, temp, elist_element, elist_element->index, 0, yylineno);
		elist_element = elist_element->next;
	}
	return temp;
}

expr* inter_code_member_item(expr* val, expr* index){
	/*if val is already a tableitem save its value
	  in a temp val */
	val = emit_if_tableitem(val);
	expr* mem_item = newexpr(tableitem_e);
	mem_item->sym = val->sym;
	mem_item->index = index;
	return mem_item;
}