#include "machine.h"
#include <stdio.h>
#include <stdlib.h>

#define HELLO_WORLD "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>."
#define SAMPLE ",."

int main(int argc, char **argv)
{
    machine_t mach;

    if (argc != 2) {
        fprintf(stderr, "Usage: bf <filename>\n");
        exit(1);
    }

    char *filename = argv[1];
    char code[64*1024]; 

    FILE *file = fopen(filename, "r");

    if (file==NULL) {
        perror("fopen");
        exit(1);
    }

    size_t size = fread(code, 1, 64*1024, file);
    code[size] = '\0';

    mach = machine_new(code);

    machine_run(mach);

    machine_free(mach);

    return 0;
}
