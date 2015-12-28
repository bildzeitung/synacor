#!/usr/bin/env python

from itertools import permutations

DATUM = (2, 3, 5, 7, 9)

for x in permutations(DATUM, 5):
    a, b, c, d, e = x
    result = a + b * c**2 + d**3 - e
    if result == 399:
        print x
