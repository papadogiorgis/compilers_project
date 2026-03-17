#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "symtable.h"

#define HASH_MULTIPLIER 65599


static unsigned int SymTable_hash(const char *pcKey){
	size_t ui;
	unsigned int uiHash = 0U;
	for(ui = 0U; pcKey[ui] != '\0'; ui++){
		uiHash = uiHash + HASH_MULTIPLIER * pcKey[ui];
	}
	return uiHash;
}

struct symtable{
	node **hashtable;
	unsigned int bindings;
	int size;
};

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


/*if pcKey doesnt already exist inside the symtable it is inserted*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
	int index = SymTable_hash(pcKey) % (oSymTable -> size); /*index of hashtable*/
	node *new;

	assert(oSymTable != NULL);
	assert(pcKey != NULL);
	
	if (SymTable_contains(oSymTable, pcKey) == 1) return 0; /*key already stored*/
	

	new = malloc(sizeof(node));
	new -> key = malloc(strlen(pcKey) + 1);
	strcpy(new -> key, pcKey);
	new -> value = (void *)pvValue;
	new -> next = oSymTable -> hashtable[index];
	oSymTable -> hashtable[index] = new;

	(oSymTable -> bindings)++;
	if (oSymTable -> bindings > oSymTable -> size){ /*if hashtable gets too big we rehash*/
		if(oSymTable -> size < 65521){ /*until size == 65521*/
			rehash(oSymTable , oSymTable->bindings);
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


/*returns the value of a specific key inside the hashtable*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
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

	if (curr != NULL && strcmp(pcKey, curr -> key) == 0)
		return curr -> value;
	else 
		return NULL;
}



/*applies pfApply function to all the bindings of the symtable*/
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra){
	node *curr;	
	int i;

	assert(pfApply != NULL);
	assert(oSymTable != NULL);

	for (i = 0; i< oSymTable -> size; i++){
		curr = oSymTable -> hashtable[i];
		while (curr != NULL){
			pfApply(curr -> key, curr -> value, (void*)pvExtra); /*cast to avoid compiler warnings, CAREFUL with use of pvExtra in apply*/
			curr = curr -> next;
		}
	}
}
