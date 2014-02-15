#include "machine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#undef DEBUG

#ifdef DEBUG
#define dputs(msg) fprintf(stderr, "%s\n", msg)
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
static void create_jumptable(machine_t m);

machine_t machine_new(const char *code)
{
    machine_t inst = calloc(sizeof(_machine), 1);

    inst->mem = memory_new();
    inst->code = strdup(code);
    dprintf("code size %d bytes\n", strlen(code));
    inst->code_size = strlen(code);
    inst->jumptable = calloc(sizeof(int), inst->code_size);
    create_jumptable(inst);
    return inst;
}

void machine_free(machine_t inst)
{
    free(inst->jumptable);
    free(inst->code);
    memory_free(inst->mem);
    free(inst);
}

static void create_jumptable(machine_t m)
{
    /* start: '[' の位置 */
    /* 対応する ']' の位置を返す */
    int brackets(machine_t m, int start) {
        int i;
        for (i=start+1; i < m->code_size;i++) {
        dprintf("brackets: i=%d\n", i);
            switch (m->code[i]) {
            case '[':
                /* 新たなブロック発見 */
                dputs ("found open bracket");
                i = brackets(m, i);
                break;
            case ']':
                /* [ は ]+1 にジャンプし、] は [+1 にジャンプする */
                dputs ("found close bracket");
                m->jumptable[start] = i+1;
                m->jumptable[i] = start+1;
                return i;
            }
        }
        assert(0); /* unreachable */
    }

    /* 最初の [ を見付ける */
    int i = 0;

    for (;;) {
        for (; i < m->code_size && m->code[i] != '['; i++)
            ;

        if (i == m->code_size) break;

        i = brackets(m, i) + 1;
    }

#ifdef DEBUG
    for (i = 0; i < m->code_size; ++i) {
        if (m->code[i] == '[' || m->code[i] == ']') {
            fprintf(stderr, "a[%d]=%d\n", i, m->jumptable[i]);
        }
    }
#endif
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
    if (memory_at(m->mem, m->dp)==0) {
        assert(m->jumptable[m->ip] != 0);
        m->ip = m->jumptable[m->ip];
    } else {
        incip(m);
    }
}

/* データが 0 以外だったら対応する開括弧の後に移動する */
static void closebracket(machine_t m)
{
    dputs("close bracket");
    if (memory_at(m->mem, m->dp)!=0) {
        assert(m->jumptable[m->ip] != 0);
        m->ip = m->jumptable[m->ip];
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
