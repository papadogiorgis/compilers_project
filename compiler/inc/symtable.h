#ifndef HASHMAP
#define HASHMAP
enum SymbolType {
	GLOBAL, LOCALV, FORMAL, USERFUNC, LIBFUNC
};

typedef struct node {
	int isActive;
	unsigned int offset;
	// unsigned int line;
	int isVar;
	enum SymbolType type;
	char *key;
	unsigned int scope;
	unsigned int line;
	// void *value;
	struct node *next; // next for hashtable collisions list
	struct node *nextScope; // next ptr for scope linked list
}node;

typedef struct ScopeLists {
	node **lists;
	unsigned int bindings;
	unsigned int size;
} ScopeLists;

struct symtable{
	node **hashtable;
	unsigned int bindings;
	int size;
};

extern int funcScope[1024];

typedef struct symtable* SymTable_T;

int checkFunc(const char *pcKey, SymTable_T oSymTable, unsigned int scope, int lineno);

void hideScope(unsigned int scope);

node *getSymbol(const char *pcKey, SymTable_T oSymTable);

node *getSymbolScope(const char *pcKey, SymTable_T oSymTablem, unsigned int scope);


int findScope(const char *pcKey, SymTable_T oSymTable);

void hideScopeRange(unsigned int scope);

SymTable_T SymTable_new(void);

void SymTable_free(SymTable_T oSymTable);

unsigned int SymTable_getLength(SymTable_T oSymTable);

// changed from returning int !
node *SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue
				, enum SymbolType type, unsigned int scope, unsigned int line, int localKwd, unsigned int offset);

int SymTable_remove(SymTable_T oSymTable, const char *pcKey);

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

node *SymTable_get(SymTable_T oSymTable, const char *pcKey);

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);

void printScopeList();

int putLibFunctions(SymTable_T symtable);
#endif