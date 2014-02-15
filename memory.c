#include "memory.h"
#include <stdlib.h>
#include <stdio.h>

memory_t memory_new(void)
{
    memory_t inst = calloc(sizeof(_memory), 1);

    inst->data = calloc(sizeof(char), INITIAL_CAPACITY);
    inst->capacity = INITIAL_CAPACITY;

    return inst;
}

void memory_free(memory_t inst)
{
    free(inst->data);
    free(inst);
}

static void check_range(memory_t mem, int index)
{
    if (index < 0) {
        fprintf(stderr, "index range error (%d)\n", index);
        exit(1);
    } else if (index >= mem->capacity) {
        fprintf(stderr, "SEGV\n");
        exit(1);
    }
}

char memory_at(memory_t mem, int index)
{
    check_range(mem, index);
        
    return mem->data[index];
}

void memory_set(memory_t mem, int index, char value)
{
    check_range(mem, index);

    mem->data[index] = value;
}
