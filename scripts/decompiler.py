#!/usr/bin/env python

import array

OPCODES = {0: ['halt', 0],
        1: ['set', 2],
        2: ['push', 1],
        3: ['pop', 1],
        4: ['eq', 3],
        5: ['gt', 3],
        6: ['jmp', 1],
        7: ['jt', 2],
        8: ['jf', 2],
        9: ['add', 3],
        10: ['mult', 3],
        11: ['mod', 3],
        12: ['and', 3],
        13: ['or', 3],
        14: ['not', 2],
        15: ['rmem', 2],
        16: ['wmem', 2],
        17: ['call', 1],
        18: ['ret', 0],
        19: ['out', 1],
        20: ['in', 1],
        21: ['noop', 0],
        }

with open('../materials/challenge.bin','rb') as infile:
    THING = array.array('H')
    THING.fromfile(infile, 3050)

print 'TOTAL ITEMS: ', len(THING)
IDX = 0
while IDX < len(THING):
    opcode = THING[IDX]
    orig_idx = IDX
    IDX += 1

    if opcode in OPCODES:
        instr, nargs = OPCODES[opcode]
    else:
        instr = '?'
        nargs = 0

    args = []
    for i in xrange(nargs):
        IDX += 1
        args.append(THING[IDX])

    print orig_idx, instr, args
    print IDX, len(THING)
