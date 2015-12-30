/* Synacor VM
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../uthash/include/utlist.h"

#include "vm.h"
#include "machine.h"

MEM_P memory;
bool debug;
int pc;
element *head = NULL;
int selfcheckflag = 0;
int breakpoint = 5451;

int init_vm(bool dbg) {
    memory = (MEM_P)(malloc(sizeof(DT)*MAX_MEMORY));

    // return error if failure to initialize
    if (memory == NULL) return 1;

    memset(memory, 0, sizeof(DT)*MAX_MEMORY);

    // set debug flag
    debug = dbg;

    pc = 0;

    return 0;
}

int load_memory(FILE* file) {
    // check to make sure image will fit in memory
    fseek(file, 0, SEEK_END);
    size_t sz = ftell(file);

    printf("Loading image of size %zu\n", sz);
    
    if (sz > MAX_MEMORY) {
        printf("Image is larger (%i) than available memory (%zu)\n", 
                MAX_MEMORY, sz);
        return 1;
    }

    if (!sz) {
        printf("Empty memory image\n");
        return 1;
    }
    
    rewind(file);

    // ok: now do load the image
    size_t rv = fread(memory, 1, sz, file);
    if (rv != sz) {
        perror("Image read error");
        return 1;
    }

    // image loaded
    return 0;
    }

int print_instructions(int start, int numinstr) {
    char spacer;
    char bp;

    for (int i=0 ; i < numinstr; i++) {
        spacer = (start == pc) ? '>' : ' ';
        bp = (breakpoint == start) ? '*' : ' ';
        printf("%c%c %5d ", spacer, bp, start);

        int opcode = memory[start++];
        if (opcode > 21) {
            printf("%i ??\n", opcode);
            continue;
        }
        if (opcode == 19) {
            int value = memory[start];
            if (value > 32767 && value < 32776) {
                printf("%4s reg[%i]\n", OPLIST[opcode].op, value - 32768);
            } else {
                printf("%4s %c\n", OPLIST[opcode].op, value);
            }
            start++;
            continue;
        }

        printf("%4s ", OPLIST[opcode].op);
        for (int j=0; j < OPLIST[opcode].instrs; j++) {
            int value = memory[start++];
            if (value > 32767 && value < 32776) {
                printf("reg[%i] ", value - 32768);
            } else {
                printf("%i ", value);
            }
        }
        printf("\n");
    }

    return start;
}

void debugger() {
    int debugpc = print_instructions(pc, 10);
    int done = 0;

    while (!done) {
        char op;
        int param, reg;

        printf("> ");
        // cmd
        scanf("%c", &op);
        switch (op) {
            case 'x':  // exit
                done = 1;
                break;
            case 'd':  // detail
                scanf("%i", &param);
                printf("%i %i\n", param, memory[param]);
                break;
            case 'g':  // go
                scanf("%i", &debugpc);
            case 'c':  // continue
                debugpc = print_instructions(debugpc, 10);
                break;
            case 'r':  // register
                for (int i=0; i < 8; i++)
                    printf("reg[%i]: %i\n", i, memory[32768+i]);
                break;
            case 'b':  // breakpoint
                scanf("%i", &breakpoint);
                break;
            case 's':  // set register
                scanf("%i %i", &reg, &param);
                memory[32768+reg] = param;
                break;
            default:
                continue;
        }
        getchar();
    }
}

int deref(int idx) {
    return (memory[idx] > 32767) ? memory[memory[idx]] : memory[idx];
}

void reg_or_memset(int pc, int value) {
    if (memory[pc] > 32767) {
        memory[memory[pc]] = value;
    } else {
        memory[pc] = value;
    }
}

/** Run the VM
 *
 * if the user has specified a script file, then feed that in while it lasts
 * into the 'in' operation
 */
int start_vm(FILE* inscript) {
    while (1) {
        if (pc == breakpoint) debugger();

        // instruction
        DT opcode = memory[pc];
        pc++;

        // temporaries
        int idx = 0;
        int result;
        element *item;

        switch (opcode) {
            case 0:     // halt
                printf("\nHALT\n");
                exit(0);
            case 1:     // set a b
                idx = 2;
                memory[memory[pc]] = deref(pc+1);
                break;
            case 2:     // push a
                idx = 1;
                item = malloc(sizeof(element));
                item->value = deref(pc);
                DL_APPEND(head, item);
                break;
            case 3:     // pop a
                idx = 1;
                if (!head) {
                    printf("Cannot pop an empty stack\n");
                    exit(1);
                }
                item = head->prev;
                reg_or_memset(pc, item->value);
                DL_DELETE(head, item);
                free(item);
                break;
            case 4:     // eq a b c
                idx = 3;
                result = 0;
                if (deref(pc+1) == deref(pc+2)) {
                    result = 1;
                }
                reg_or_memset(pc, result);
                break;
            case 5:     // gt a b c
                idx = 3;
                result = 0;
                if (deref(pc+1) > deref(pc+2)) {
                    result = 1;
                }
                reg_or_memset(pc, result);
                break;
            case 6:     // jmp a
                pc = deref(pc);
                break;
            case 7:     // jt a b
                if (deref(pc)) {
                    pc = deref(pc+1);
                } else {
                    idx = 2;
                }
                break;
            case 8:     // jf a b
                if (deref(pc) == 0) {
                    pc = deref(pc+1);
                } else {
                    idx = 2;
                }
                break;
            case 9:     // add a b c
                idx = 3;
                reg_or_memset(pc, (deref(pc+1) + deref(pc+2)) % 32768);
                break;
            case 10:    // mult a b c
                idx = 3;
                reg_or_memset(pc, (deref(pc+1) * deref(pc+2)) % 32768);
                break;
            case 11:    // mod a b c
                idx = 3;
                reg_or_memset(pc, deref(pc+1) % deref(pc+2));
                break;
            case 12:    // and a b c
                idx = 3;
                reg_or_memset(pc, deref(pc+1) & deref(pc+2));
                break;
            case 13:    // or a b c
                idx = 3;
                reg_or_memset(pc, deref(pc+1) | deref(pc+2));
                break;
            case 14:    // not a b
                idx = 2;
                reg_or_memset(pc, 32767 - deref(pc+1));
                break;
            case 15:    // rmem a b
                idx = 2;
                reg_or_memset(pc, memory[deref(pc+1)]);
                break;
            case 16:    // wmem a b
                idx = 2;
                reg_or_memset(deref(pc), deref(pc+1));
                break;
            case 17:    // call a
                item = malloc(sizeof(element));
                item->value = pc + 1;
                DL_APPEND(head, item);
                pc = deref(pc);
                break;
            case 18:    // ret
                if (!head) {
                    printf("RET failed on empty stack\n");
                    exit(0);
                }
                item = head->prev;
                pc = item->value;
                DL_DELETE(head, item);
                free(item);
                break;
            case 19:    // out a
                idx = 1;
                putchar(deref(pc));
                break;
            case 20:    // in
                idx = 1;
                if (inscript && !feof(inscript)) {
                    result = fgetc(inscript);
                    if (result == EOF) {
                        fclose(inscript);
                        inscript = NULL;
                        result = getchar();
                    } else {
                        putchar(result);
                    }
                    reg_or_memset(pc, result);
                } else {
                    reg_or_memset(pc, getchar());
                }
                break;
            case 21:    // noop
                break;
            default:
                printf("\nUnrecognized op: %i\n", opcode);
                exit(1);
        }
        pc += idx;
    }

    return 0; // success
}
