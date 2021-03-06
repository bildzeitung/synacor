/* Synacor Machine
 */

// 15-bit address space
#define MAX_MEMORY (int)(65536)

#define DT unsigned short
#define MEM_P DT*

typedef struct element {
    DT value;
    struct element *next;
    struct element *prev;
} element;

