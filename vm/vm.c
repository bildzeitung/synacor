/* Synacor VM
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vm.h"

MEM_P memory;
bool debug;
int pc;

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

    sz = (sz > 32767) ? 32767 : sz;
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

DT deref(int idx) {
    if (memory[idx] > 32767) {
        return memory[memory[idx]];
    }

    return memory[idx];
}

int start_vm() {
    while (1) {
        //printf("Got opcode: %i\n", memory[0]);

        DT opcode = memory[pc];
        pc++;  // instruction
        int idx = 0;
        int result;
        switch (opcode) {
            case 0:     // halt
                printf("\nHALT\n");
                exit(0);
            case 1:     // set a b
                idx = 2;
                memory[memory[pc]] = memory[pc+1];
                break;
            case 2:     // push a
                // TODO: push to stack
                idx = 1;
                break;
            case 3:     // pop a
                // TODO: pop from stack
                idx = 1;
                break;
            case 4:     // eq a b c
                idx = 3;
                result = 0;
                if (deref(pc+1) == deref(pc+2)) {
                    result = 1;
                }
                if (memory[pc] > 32767) {
                    memory[memory[pc]] = result;
                } else {
                    memory[pc] = result;
                }
                break;
            case 6:     // jmp a
                pc = deref(pc);
                //printf("JMP to %i\n", memory[pc]);
                break;
            case 7:     // jt a b
                idx = 2;
                if (deref(pc)) {
                    idx = 0;
                    //printf("JT to %i (%i)\n", memory[pc+1], memory[pc]);
                    pc = deref(pc+1);
                }
                break;
            case 8:     // jf a b
                idx = 2;
                if (!deref(pc)) {
                    idx = 0;
                    //printf("JF to %i (%i)\n", memory[pc+1], pc+1);
                    pc = deref(pc+1);
                }
                break;
            case 9:     // add a b c
                idx = 3;
                result = (deref(pc+1) + deref(pc+2)) % 32768;
                if (memory[pc] > 32767) {
                    memory[memory[pc]] = result;
                } else {
                    memory[pc] = result;
                }
                break;
            case 19:    // out a
                printf("%c", deref(pc));
                idx = 1;
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
