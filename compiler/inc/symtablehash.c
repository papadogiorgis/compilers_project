#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "symtable.h"

#define HASH_MULTIPLIER 65599

ScopeLists *slists;

int addToScopeList(node *n){
	int index = n->scope;
	slists->bindings++;
	// insert at head
	if (slists->bindings >= slists->size){
		int old_size = slists->size;
		int new_size = 2 * slists->size;
		node** tmp = realloc(slists->lists, sizeof(node) * new_size);
		if (tmp == NULL){
			fprintf(stderr, "Realloc failed\n");
		} 
		for (int i = old_size; i < new_size; i++){
			slists->lists[i] = NULL;
		}
		slists->lists = tmp;
		slists->size = new_size;
	}
	n->nextScope = slists->lists[index];
	slists->lists[index] = n;

	return 1;
}

char *getType(enum SymbolType type){
	switch(type){
		case GLOBAL:
			return "global variable";
		case LOCALV:
			return "local variable";
		case FORMAL:
			return "formal argument";
		case LIBFUNC:
			return "library function";
		case USERFUNC:
			return "user function";
		default:
			return "bad type"; 
	}
}

void printNodeInfo(node *curr) {
	if (curr == NULL) return;
	printNodeInfo(curr->nextScope);

	printf("\"%s\" [%s] (line %d) (scope %d)\n", curr->key, getType(curr->type), curr->line, curr->scope);
	return;
}

void printScopeList(){
	node *curr;
	for (int i = 0; i < slists->size; i++){
		curr = slists->lists[i];
		if (curr != NULL) {
			printf("\n\n-----------   Scope #%d   -----------\n", curr->scope);
		}
		printNodeInfo(curr);
	}
}

int putLibFunctions(SymTable_T symtable){
	SymTable_put(symtable, "print", "print", LIBFUNC, 0, 0, 0); 
	SymTable_put(symtable, "input", "input", LIBFUNC, 0, 0,0 );
	SymTable_put(symtable, "objectmemberkeys" , "objectmemberkeys", LIBFUNC, 0, 0, 0); 
	SymTable_put(symtable, "objecttotalmembers", "objecttotalmembers", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "objectcopy", "objectcopy", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "totalarguments", "totalarguments", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "argument", "argument", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "typeof", "typeof", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "strtonum", "strtonum", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "sqrt", "sqrt", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "cos", "cos", LIBFUNC, 0, 0, 0);
	SymTable_put(symtable, "sin", "sin", LIBFUNC, 0, 0, 0);

	return 1;
}

void hideScope(unsigned int scope){
	assert(scope < slists->size);
	if (scope == 0){ return; }
	node *curr = slists->lists[scope];
	while (curr) {
		curr->isActive = 0;
		curr = curr->nextScope;
	}
}

void hideScopeRange(unsigned int scope) {
	for (int i = 1; i < scope; i++){
		hideScope(i);
	}
}

static unsigned int SymTable_hash(const char *pcKey){
	size_t ui;
	unsigned int uiHash = 0U;
	for(ui = 0U; pcKey[ui] != '\0'; ui++){
		uiHash = uiHash + HASH_MULTIPLIER * pcKey[ui];
	}
	return uiHash;
}


int findNextSize(int curr){ /*parameter : current size*/
	if (curr == 510)
		return 1021;
	else if (curr == 1022)
		return 2053;
	else if (curr == 2054)
		return 4093;
	else if(curr == 4094)
		return 8191;
	else if(curr == 8192)
		return 16381;
	else if(curr == 16384)
		return 32771;
	else
		return 65521;
}


/*creates new hashtable and inserts all of the keys of the previous hashtable in it
 * then it frees the previous hashtable and makes the hashtable of the symtable the new hashtable*/
void rehash(SymTable_T oSymTable, int curr_size){
	int new_size = findNextSize(curr_size);
	node **newtable;
	node *curr;
	node *next;
	int i, index;

	newtable = malloc(new_size * sizeof(node));
	oSymTable->size = new_size;

	for(i = 0; i < curr_size; i++){
		curr = oSymTable -> hashtable[i];
		while(curr != NULL){
			next = curr -> next;
			index = SymTable_hash(curr -> key) % new_size;
			curr -> next = newtable[index];
			newtable[index] = curr;
			
			curr = next;
		}
	}
	free(oSymTable -> hashtable);

	oSymTable -> hashtable = newtable;
}


/*creates a new symtable with size 509*/
SymTable_T SymTable_new(void){
	int i;
	SymTable_T new = malloc(sizeof(struct symtable));

	slists = malloc(sizeof(ScopeLists));
	slists->lists = malloc(sizeof(node) * 128);
	for (int i = 0; i < 128; i++){
		slists->lists[i] = NULL;
	}
	slists->size = 128;
	slists->bindings = 0;

	if (new == NULL) {
		printf("Memory allocation failed. Exit");
		exit(-1);
	}
	new -> bindings = 0;
	new -> size = 509;
	new -> hashtable = malloc(sizeof(node) * 509);
	if (new -> hashtable == NULL){
		printf("Calloc failed.");
		exit(-1);
	}

	for(i = 0; i< 509; i++){
		new -> hashtable[i] = NULL;
	}

	
	return new;
}

/*frees all memory allcoated by oSymTable*/
void SymTable_free(SymTable_T oSymTable){
	int i = 0;
	node *curr;
	node *next;

	if(oSymTable == NULL) return;

	for(i = 0; i < oSymTable -> size; i++){
		curr = oSymTable -> hashtable[i];
		while(curr != NULL){
			next = curr -> next;
			free(curr -> key);
			free(curr);
			curr = next;
		}
	
	}

	free(oSymTable -> hashtable);
	free(oSymTable);
}

/*returns number of bindings of symtable*/
unsigned int SymTable_getLength(SymTable_T oSymTable){
	assert(oSymTable != NULL);

	return oSymTable -> bindings;
}

/*returns 0 if symtable doesnt contain pcKey and 1 if it does*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
	int index = SymTable_hash(pcKey) % (oSymTable -> size);
	
	node *curr = oSymTable -> hashtable[index];

	assert(pcKey);
	assert(oSymTable);

	while(curr != NULL){
		if (strcmp(curr -> key, pcKey) == 0) 
			return 1;
		curr = curr -> next;
	}

	return 0;
}


int findScope(const char *pcKey, SymTable_T oSymTable) {
	node *curr;
	int max = -1;
	int index = SymTable_hash(pcKey) % (oSymTable->size);
	// int index = SymTable_hash(pcKey) % (oSymTable->size);
	curr = oSymTable->hashtable[index];
	while (curr != NULL) {
		if (strcmp(curr->key, pcKey) == 0){
			if ((int)(curr->scope) > (int) max && curr->isActive == 1){
				max = curr->scope;
			}
		}
		curr = curr->next;
	}
	return max;
}

node *getSymbolScope(const char *pcKey, SymTable_T oSymTable, unsigned int scope){
	node *curr;
	int index = SymTable_hash(pcKey) % (oSymTable->size);
	curr = oSymTable->hashtable[index];
	while (curr) {
		if (strcmp(curr->key, pcKey) == 0 && curr->isActive == 1 && curr->scope == scope){
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

node *getSymbol(const char *pcKey, SymTable_T oSymTable) {
	node *curr;
	int index = SymTable_hash(pcKey) % (oSymTable->size);
	curr = oSymTable->hashtable[index];
	while (curr) {
		if (strcmp(curr->key, pcKey) == 0 && curr->isActive == 1){
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

int checkFunc(const char *pcKey, SymTable_T oSymTable, unsigned int scope, int lineno){
	node *tmp = getSymbolScope(pcKey, oSymTable, scope);
	if (tmp != NULL) {
		if (tmp->type == USERFUNC) {
			printf("\nError: user func already defined, symbol: %s line: %d\n\n", pcKey, lineno);
		}
		else if (tmp->type == LOCALV || tmp->type == GLOBAL) {
			printf("\nError: func of already defined var cant be used, symbol: %s line: %d\n\n", pcKey, lineno);
		}
	}
	tmp = getSymbolScope(pcKey, oSymTable, 0);
	if (tmp != NULL && tmp->type == LIBFUNC){
		printf("\nError: func shadows lib func, symbol: %s line: %d\n\n", pcKey, lineno);
	}

	return 1;
}

/*if pcKey doesnt already exist inside the symtable it is inserted*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue
				, enum SymbolType type, unsigned int scope, unsigned int line, int localKwd){
	int index = SymTable_hash(pcKey) % (oSymTable -> size); /*index of hashtable*/
	node *new;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);
	
	int find_scope = findScope(pcKey, oSymTable);

	// no need to check getSymbol != NULL, because scope would be == -1
	if (find_scope == scope && getSymbolScope(pcKey, oSymTable, scope)->type == FORMAL && type == FORMAL){
		printf("\nError: formal redeclaration, symbol: %s line: %d\n\n", pcKey, line);
	} 
	else if (find_scope == 0 && getSymbolScope(pcKey, oSymTable, 0)->type == LIBFUNC && type == FORMAL){
		printf("\nError: formal shadows lib function, symbol: %s line: %d\n\n", pcKey, line);
	}

	if ((find_scope != -1 && localKwd == 0 && type!=FORMAL && type != USERFUNC) || (localKwd == 1 && find_scope == scope)){
		return 1;
	}


	new = malloc(sizeof(node));
	new -> key = malloc(strlen(pcKey) + 1);
	strcpy(new -> key, pcKey);
	new->line = line;
	new->isActive = 1;
	new->scope = scope;
	new->type = type;
	new -> next = oSymTable -> hashtable[index];
	oSymTable -> hashtable[index] = new;
	addToScopeList(new);

	(oSymTable -> bindings)++;
	if (oSymTable -> bindings > oSymTable -> size){ /*if hashtable gets too big we rehash*/
		if(oSymTable -> size < 65521){ /*until size == 65521*/
			// rehash(oSymTable , oSymTable->bindings);
			printf("buckets: %d\n" , oSymTable -> bindings);
		}
	}
	
	return 1;
	

}


/*if binding with key == pcKey exists inside symtable it gets removes and we return 1
 * otherwise return 0*/
int SymTable_remove(SymTable_T oSymTable, const char *pcKey){
	int index = SymTable_hash(pcKey) % (oSymTable -> size);
	node *curr = oSymTable -> hashtable[index];
	node *prev = NULL;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);

	if(SymTable_contains(oSymTable, pcKey) == 0) return 0; /*key doesnt exit in table*/

	while(curr != NULL){
		if (strcmp(curr -> key, pcKey) == 0)
			break;
		prev = curr;
		curr = curr -> next;
	}

	if (prev == NULL) /*1st element of the list*/
		oSymTable -> hashtable[index] = curr -> next;
	else{
		prev -> next = curr -> next;
	}
	
	free(curr -> key);
	free(curr);
	(oSymTable -> bindings--);
	return 1;
	
}


node *SymTable_get(SymTable_T oSymTable, const char *pcKey){
	node *curr;
	int index = SymTable_hash(pcKey) % (oSymTable -> size);
	
	assert(oSymTable != NULL);
	assert(pcKey != NULL);

	curr = oSymTable -> hashtable[index];
	while(curr != NULL){
		if (strcmp(pcKey, curr -> key) == 0)
			break;
		curr = curr -> next;
	}

	if (curr != NULL && strcmp(pcKey, curr -> key) == 0){
		// if (curr->type == USERFUNC || curr->type == LIBFUNC){
		// 	return curr -> value.funcVal;
		// }
		// else {
		// 	return curr->value.varVal;
		// }
		return curr;
	}
	else 
		return NULL;
}



/*applies pfApply function to all the bindings of the symtable*/
// void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra){
// 	node *curr;	
// 	int i;

// 	assert(pfApply != NULL);
// 	assert(oSymTable != NULL);

// 	for (i = 0; i< oSymTable -> size; i++){
// 		curr = oSymTable -> hashtable[i];
// 		while (curr != NULL){
// 			pfApply(curr -> key, curr -> value, (void*)pvExtra); /*cast to avoid compiler warnings, CAREFUL with use of pvExtra in apply*/
// 			curr = curr -> next;
// 		}
// 	}
// }
