#define INITIAL_CAPACITY 30000

typedef struct {
    char *data;
    int capacity;
} _memory, *memory_t;

memory_t memory_new(void);

void memory_free(memory_t mem);

char memory_at(memory_t mem, int index);

void memory_set(memory_t mem, int index, char value);
