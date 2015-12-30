# Implementation notes

## Base VM
No major issues; spec is clear and the VM came together nicely. Decided on C because I didn't know how computationally demanding the VM is, and figured on fast instead of sophisticated. The structure is basic -- a large switch statement and global state.

The stack is a compiler-defined double-linked list, which is O(1) for insert and removal, and meets the "unbounded" condition, but if there is too much activity, the malloc()'s become dominant, in terms of performance. So far, it doesn't look like this is a major hang-up. I have contemplated keeping track of the maximum stack depth and then just hard coding in an array.

## Text Adventure Game
I haven't drawn up maps like this in a very long time. The engine is fairly simple -- reminiscent less of Zork and more of the Scott Adams games.

A replay mechanism is useful, since there's permadeath, so I hacked one into the key input section, and setup a script with a series of moves.

## Number puzzle
After collecting the coins, I just quickly hacked up a python script to find the answer, although working it out by hand is reasonable. Either way, the solution is unique and now part of the replay script

## Teleporter
If I understand the book correctly, this is all about setting the 8th register to some value.

There are two ways about this:
* figure out what the correct value is
* use some remote control to simply detect a bad value and try all of them

I decided on trying to reverse-engineer the code, since that seems to be the intent of the problem (reading into the strange book).

My first thought was to have the VM scan for any access of that register. Before the value would be used, set it to a non-zero value (say, 1) and see what happens. I can't use the first access of that register, as the self-test then blows up. Naively setting it does produce a different result, so that was encouraging.

Now, I need some semblance of a debugger, to track through the code and register states. Not sure *how much of one*, but at least something.

The first instruction to note was:

    5451   jf reg[7] 5605

If the value is zero, then we get the default behaviour and jump to Synacor HQ, so the path we want isn't at 5605, but continues afterwards.

After some register sets,

    call 1458

This routine calls another one, at *1531*, which on inspection is an *XOR* operation.

My reading is that it's a decoding of some other block of memory to output this:

    "Unusual setting detected!  Starting confirmation process!  Estimated time to completion: 1 billion years."

Setting a breakpoint after this routine returns confirms at least the output part of that. After a number of NOPs, the real work appears to begin at *5483*.
