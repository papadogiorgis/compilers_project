
// typedef struct Variable {
// 	const char *name;
// } Variable;

// typedef struct Function {
// 	const char *name;
// } Function;

enum SymbolType {
	GLOBAL, LOCALV, FORMAL, USERFUNC, LIBFUNC
};

typedef struct node {
	int isActive;
	// union {
	// 	Variable *varVal;
	// 	Function *funcVal;
	// } value;
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

typedef struct symtable* SymTable_T;

void hideScope(unsigned int scope);

SymTable_T SymTable_new(void);

void SymTable_free(SymTable_T oSymTable);

unsigned int SymTable_getLength(SymTable_T oSymTable);

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue
				, enum SymbolType type, unsigned int scope, unsigned int line, int localKwd);

int SymTable_remove(SymTable_T oSymTable, const char *pcKey);

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

node *SymTable_get(SymTable_T oSymTable, const char *pcKey);

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);

void printScopeList();

int putLibFunctions(SymTable_T symtable);
