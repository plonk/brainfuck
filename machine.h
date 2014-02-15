#include "memory.h"

typedef struct {
    memory_t mem;
    int dp;
    char *code;
    int *jump_table;
    int code_size;
    int ip;
} _machine, *machine_t;

machine_t machine_new(const char *code);

void machine_free(machine_t mach);

void machine_run(machine_t mach);

