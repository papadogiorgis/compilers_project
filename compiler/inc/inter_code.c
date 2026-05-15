#include "inter_code.h"
#include <string.h>

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
	lval = lval ? emit_if_tableitem(lval) : 0;
	rval = rval ? emit_if_tableitem(rval) : 0;
	
	expr *tmp = newtemp();
	emit(op, lval, rval, tmp, op == not_op ? 0 : currQuad+2, yylineno);
	return tmp;
}

// Emits quad for expression: ++<var>
expr *inter_code_incr_var(expr *val)
{
	expr *res = newtemp();

	if (val->type == tableitem_e){
		res = emit_if_tableitem(val);
		emit(add, res, newexpr_constnum(1), res, 0, yylineno);
		emit(tablesetelem, val->index, res, val, 0, yylineno);
		return res;
	}

	emit(add, val, newexpr_constnum(1), val, 0, yylineno);
	emit(assign, val, NULL, res, 0, yylineno);
	return res;
}

expr *inter_code_decr_var(expr *val)
{
	expr *res = newtemp();

	if (val->type == tableitem_e){
		res = emit_if_tableitem(val);
		emit(sub, res, newexpr_constnum(1), res, 0, yylineno);
		emit(tablesetelem,  val->index, res, val,0, yylineno);
		return res;
	}

	emit(sub, val, newexpr_constnum(1), val, 0, yylineno);
	emit(assign, val, NULL, res, 0, yylineno);
	return res;
}

// Emits quad for expression: <var>++
expr *inter_code_increment(expr *val)
{
	expr *res = newtemp();

	if (val->type == tableitem_e){
		// fetch table item first and add later
		expr *tmp = emit_if_tableitem(val);
		emit(assign, tmp, NULL, res, 0, yylineno);
		emit(add, tmp, newexpr_constnum(1), tmp, 0, yylineno);
		emit(tablesetelem, val->index, tmp, val, 0 , yylineno);
		return res;
	}

	emit(assign, val, NULL, res, 0, yylineno);
	emit(add, val, newexpr_constnum(1), val, 0, yylineno);
	return res;
}

expr *inter_code_decrement(expr *val)
{
	expr *res = newtemp();

	if (val->type == tableitem_e){
		// fetch table item first and add later
		expr *tmp = emit_if_tableitem(val);
		emit(assign, tmp, NULL, res, 0, yylineno);
		emit(sub, tmp, newexpr_constnum(1), tmp, 0, yylineno);
		emit(tablesetelem, val->index, tmp, val, 0 , yylineno);
		return res;
	}

	emit(assign, val, NULL, res, 0, yylineno);
	emit(sub, val, newexpr_constnum(1), val, 0, yylineno);
	return res;
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

expr* inter_code_call(expr* lval, expr* elist){
	expr* f = emit_if_tableitem(lval);
	if(lval->sym && lval->sym->type == USERFUNC){
		f->type = programfunc_e;
	}else if(lval->sym && lval->sym->type == LIBFUNC){
		f->type = libraryfunc_e;
	}else{
		f->type = var_e;
	}
	//reeverese elist
	expr* rev = NULL;
	expr* cur = elist;
	while(cur != NULL){
		expr* next = cur->next;
		cur->next = rev;
		rev = cur;
		cur = next;
	}
	//now the elist is reversed and stored in expr* rev
	while(rev != NULL){
		emit(param, rev, NULL, NULL, 0, yylineno);
		rev = rev->next;
	}
	//make call
	emit(call, f, NULL, NULL, 0, yylineno);
	//save return value in a temp var
	expr* res = newtemp();
	emit(getretval, NULL, NULL, res, 0, yylineno);
	return res;
}

expr* inter_code_callsuffix(expr* lval, call_struct* callsuffix){
	if(callsuffix->method){
		//bring table
		lval = emit_if_tableitem(lval);
		//take func from table
		expr* f = newtemp();
		expr* index = newexpr(conststring_e);
		index->strConst = strdup(callsuffix->name);
		emit(tablegetelem, lval, index, f, 0, yylineno);
		//add lval as the first argument in elist
		lval->next = callsuffix->elist;
		callsuffix->elist = lval;
		//call the function
		return inter_code_call(f, callsuffix->elist);
	}else{
		//its a normal func, so we call it
		return inter_code_call(lval, callsuffix->elist);
	}

}

expr *inter_code_uminus(expr *exp)
{
	expr *result = newtemp();
	if (!is_arith(exp)) {
		fprintf(stderr, "Error: illegal arithmetic operation at line %d\n", yylineno);
		return NULL;
	}
	result->type = arithexpr_e;
	emit(uminus, exp, NULL, result, 0, yylineno);
	return result;
}

