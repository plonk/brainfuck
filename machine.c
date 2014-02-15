#include "machine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#undef DEBUG

#ifdef DEBUG
#define dputs(msg) puts(msg)
#define dprintf(args...) fprintf(stderr, args)
#else
#define dputs(msg)
#define dprintf(args...)
#endif

static bool running(machine_t m);
static void output(machine_t m);
static void input(machine_t m);
static void incip(machine_t m);
static void decip(machine_t m);
static void incdp(machine_t m);
static void decdp(machine_t m);
static void incdata(machine_t m);
static void decdata(machine_t m);
static void openbracket(machine_t m);
static void closebracket(machine_t m);
static void nop(machine_t m);
static char fetch_op(machine_t m);

machine_t machine_new(const char *code)
{
    machine_t inst = calloc(sizeof(_machine), 1);

    inst->mem = memory_new();
    inst->code = strdup(code);
    dprintf("code size %d bytes\n", strlen(code));
    inst->code_size = strlen(code);
    return inst;
}

void machine_free(machine_t inst)
{
    memory_free(inst->mem);
    free(inst->code);
    free(inst);
}

static bool running(machine_t m)
{
    return m->ip < m->code_size;
}

static void output(machine_t m)
{
    char byte = memory_at(m->mem, m->dp);

    dputs("output");
    putchar(byte);
    incip(m);
}

static void input(machine_t m)
{
    char c;

    dputs("input");
    for (;;) {
        c = getchar();

        if (c == EOF)
            continue;
        if (c == '\n')
            continue;
        break;
    }
    memory_set(m->mem, m->dp, c);
    incip(m);
}

static void incip(machine_t m)
{
    dputs("incip");
    if (m->ip == m->code_size) {
        fprintf(stderr, "incip: instruction pointer too large\n");
        exit(1);
    }
    m->ip++;
}

static void decip(machine_t m)
{
    dputs("decip");
    if (m->ip == 0) {
        fprintf(stderr, "negative instruction pointer\n");
        exit(1);
    }
    m->ip--;
}

static void incdp(machine_t m)
{
    dputs("incdp");
    m->dp++;
    incip(m);
}

static void decdp(machine_t m)
{
    dputs("decdp");
    m->dp--;
    incip(m);
}

static void incdata(machine_t m)
{
    char byte = memory_at(m->mem, m->dp);
    dputs("incdata");
    memory_set(m->mem, m->dp, byte+1);
    incip(m);
}

static void decdata(machine_t m)
{
    char byte = memory_at(m->mem, m->dp);
    dputs("decdata");
    memory_set(m->mem, m->dp, byte-1);
    incip(m);
}

/* データが 0 だったら対応する閉括弧の後に移動する */
static void openbracket(machine_t m)
{
    dputs("open bracket");
    if (memory_at(m->mem, m->dp)==0) {
        int inner_block;

        incip(m);
        for (inner_block=0;
             !(inner_block==0 && m->code[m->ip]==']');
             incip(m)) {
            if (m->code[m->ip]=='[') {
                inner_block++;
            } else if (m->code[m->ip]==']') {
                inner_block--;
            }
        }
        incip(m);
    } else {
        incip(m);
    }
}

/* データが 0 以外だったら対応する開括弧の後に移動する */
static void closebracket(machine_t m)
{
    dputs("close bracket");
    if (memory_at(m->mem, m->dp)!=0) {
        int inner_block;

        decip(m);
        for (inner_block=0;
             !(inner_block==0 && m->code[m->ip]=='[');
             decip(m)) {

            if (m->code[m->ip]==']') {
                inner_block++;
            } else if (m->code[m->ip]=='[') {
                inner_block--;
            }
        }
        incip(m);
    } else {
        incip(m);
    }
}

static void nop(machine_t m)
{
    incip(m);
}

static char fetch_op(machine_t m)
{
    return m->code[m->ip];
}

void machine_run(machine_t m)
{
    while (running(m)) {
        char op = fetch_op(m);

        dprintf("ip=%d\n", m->ip);
        switch (op) {
        case '.': output(m);       break;
        case ',': input(m);        break;
        case '>': incdp(m);        break;
        case '<': decdp(m);        break;
        case '+': incdata(m);      break;
        case '-': decdata(m);      break;
        case '[': openbracket(m);  break;
        case ']': closebracket(m); break;
        default:  nop(m);          break;            /* just ignore */
        }
    }
}
