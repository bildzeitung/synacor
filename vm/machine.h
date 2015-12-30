
typedef struct {
    char *op;
    int instrs;
} OP;

OP OPLIST[] = {
    {"halt", 0},
    {"set", 2},
    {"push", 1},
    {"pop", 1},
    {"eq", 3},
    {"gt", 3},
    {"jmp", 1},
    {"jt", 2},
    {"jf", 2},
    {"add", 3},
    {"mult", 3},
    {"mod", 3},
    {"and", 3},
    {"or", 3},
    {"not", 2},
    {"rmem", 2},
    {"wmem", 2},
    {"call", 1},
    {"ret", 0},
    {"out", 1},
    {"in", 1},
    {"noop", 0}
};
