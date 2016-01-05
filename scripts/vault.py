#!/usr/bin/env python
""" Vault puzzle """

START = (3, 0)
FINISH = (0, 3)
GOAL = 30
STARTVAL = 22

GRID = [['*', 8, '-', 1],
        [4, '*', 11, '*'],
        ['+', 4, '-', 18],
        [0, '-', 9, '*']]

OPS = {'*': lambda x, y: x*y,
       '+': lambda x, y: x+y,
       '-': lambda x, y: x-y,
      }

YDIR = {-1: 'north', 1: 'south'}
XDIR = {-1: 'west', 1: 'east'}

def d(x):
    return GRID[x[0]][x[1]]

# initial puzzle state
STATE = [{'t': STARTVAL, 'c': START, 'dirpath': []}]

# saved game states
MEMO = []

def solver():
    """ Breadth-first search """
    while STATE:
        state = STATE.pop(0)

        # remember the location and current total
        to_memo = (state['c'], state['t'])
        if to_memo in MEMO:  # move on if this has been seen
            continue
        MEMO.append(to_memo)

        # in final location?
        if state['c'] == FINISH:
            # have the right total?
            if state['t'] == GOAL:
                return state

            # cannot move into vault room and leave, so this game is over
            continue

        # append next moves
        yc, xc = state['c']

        def process_state(nc):
            new_state = {'t': state['t'], 'c': nc,
                         'dirpath': state['dirpath'] + [YDIR[y]]}

            if d(nc) in OPS:
                new_state['op'] = OPS[d(nc)]
            else:
                new_state['t'] = state['op'](state['t'], d(nc))

            STATE.append(new_state)

        for y in (-1, 1):
            if (yc + y < 0) or (yc + y > 3):
                continue

            process_state((yc+y, xc))

        for x in (-1, 1):
            if (xc + x < 0) or (xc + x > 3):
                continue

            process_state((yc, xc+x))

WINNER = solver()
print '\n'.join(WINNER['dirpath'])
print 'There are %s states remaining' % len(STATE)
print 'Memoized %s game states' % len(MEMO)
