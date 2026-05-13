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
	index = inter_code_bool_to_val(index);
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

expr* inter_code_uminus(expr* e){
	e = emit_if_tableitem(e);
	if(!is_arith(e)){
		return NULL;
	}
	expr* temp = newexpr(arithexpr_e);
	temp->sym = newtemp()->sym;
	emit(uminus, e, NULL, temp, 0, yylineno);
	return temp;
}

//-----for short-circuit eval-----
void inter_code_boolean_comparison(expr* lval, expr* arg1, expr* arg2, iopcode op){
	lval->sym = newtemp()->sym;

	//i make true jump and add it to the truelist
	emit(op, arg1, arg2, NULL, 0, yylineno);
	lval->richtig_list = newlist(nextquadlabel() - 1);

	//i make false jump and add it to the falselist
	emit(jump, NULL, NULL, NULL, 0, yylineno);
	lval->falsch_list = newlist(nextquadlabel() - 1);
}

void inter_make_bool_expr(expr* e){
	//if expr e is already boolexpr, do nothing
	if(e->type == boolexpr_e){
		return;
	}
	//if its not, i make the jumps
	//if its true goto...
	emit(if_eq, e, newexpr_constbool(1), NULL, 0, yylineno);
	e->richtig_list = newlist(nextquadlabel() - 1);
	//if its false goto...
	emit(jump, NULL, NULL, NULL, 0, yylineno);
	e->falsch_list = newlist(nextquadlabel() - 1);
	//change e type so we know it has true and false listsnow
	e->type = boolexpr_e;
}

expr* inter_code_bool_to_val(expr* e){
	if(e->type != boolexpr_e){
		return e;
	}
	expr* res = newtemp();
	res->type = var_e;
	//if its true assign 1
	patchlist(e->richtig_list, nextquadlabel());
	emit(assign, newexpr_constbool(1), NULL, res, 0, yylineno);
	emit(jump, NULL, NULL, NULL, nextquadlabel()+2, yylineno);
	//if its false assign 0
	patchlist(e->falsch_list, nextquadlabel());
	emit(assign, newexpr_constbool(0), NULL, res, 0, yylineno);

	return res;

}
//--------------------------------