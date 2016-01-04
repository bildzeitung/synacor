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

Setting a breakpoint after this routine returns confirms at least the output part of that. After a number of NOPs, the real work appears to begin at *5483*:

    5483  set reg[0] 4
    5486  set reg[1] 1
    5489 call 6027
    5491   eq reg[1] reg[0] 6
    5495   jf reg[1] 5579

The initial values to the function are in registers zero and one. The subroutine that is doing the checking is:

    6027   jt reg[0] 6035
    6030  add reg[0] reg[1] 1
    6034  ret
    6035   jt reg[1] 6048
    6038  add reg[0] reg[0] 32767
    6042  set reg[1] reg[7]
    6045 call 6027
    6047  ret
    6048 push reg[0]
    6050  add reg[1] reg[1] 32767
    6054 call 6027
    6056  set reg[1] reg[0]
    6059  pop reg[0]
    6061  add reg[0] reg[0] 32767
    6065 call 6027
    6067  ret

With that routine isolated, it's taken me far too long to realise that it's an implementation of Ackermann's function, *A(m, n)*, where *m* is *r0* and *n* is *r7*. The [Wikipedia entry][ackermann] has a table for *m=4*, and it's two to the power *(n+3)* - 3. The bonus here is that the math is 15-bit, so it's actually a modular exponentiation calculation.

The goal seems to be to determine a value for *A(4, n)* where the output is *6* (after the modulus). Since Ackermann remapped under a modulus eventually stabilises for most values (except, oddly, 1969), I'm hoping that there's a (correct) value that I can hit before then.

Ok, the goal is *not* that. A careful reading of the code reveals that:
* we are dealing with an Ackermann _mod-n_ variant, so we max out the recursion at 32768
* meaning that we can memoize all sorts of results, and the max size of the table is reasonable
* traslating the assembly above: r0 == m, and r1 == n
* so that leaves r7 as ... what, exactly?
* in the m &gt; 1, n == 0 case, n is being set to r7, instead of its usual 1
* this means that we're always calculating the same Ackermann function: A(4, 1)
* but, the algorithm in one part of the three cases is non-standard -- at line *6042*

The goal is to find an r7 such that *A(4, 1)* == 6.

To figure that out, I wrote a more standard-form Ackermann function, with memoization, that steps though all values from [0, 32768]. There is only one result, and it is when *r7 == 25734*.

The sequence of operations is:
* break before the call to 6027 is made
* set r0 = 6, r7 = 25734
* reset the program counter to 5491 (the line after the call to 6027)
* resume execution

## Debugger
To facilitate exploring the VM, the debugger has the following functions:

* *exit*: continue running the VM
* *detail*: inspect a single memory location
* *goto*: jump to a section of memory and print out disassembled instructions
* *continue*: print out the next few instructions
* *register*: dump contents of all registers
* *breakpoint*: set a breakpoint (initial setting is *5451*)
* *set*: set register to a given value

[ackermann]:https://en.wikipedia.org/wiki/Ackermann_function 
