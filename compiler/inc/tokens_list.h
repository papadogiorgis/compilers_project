#ifndef TOKENS
#define TOKENS

struct alpha_tok {
    int lineno;
    int tokenno;
    char* content;
    char* category;
    char *type;
    char* storage_type;
    struct alpha_tok *next;
};

typedef struct alpha_tok alpha_token_t;

struct token_list {
    alpha_token_t *head;
};

int add_token(alpha_token_t *tok, struct token_list *list, int lineno, int *tokenno, char *content, char *category, char *type, char *storage_type);
int remove_token(struct token_list *list, int tokenno);
// int find_token(struct token_list *list, int tokenno);
void print_list(struct token_list *list);

#endif