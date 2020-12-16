# Lighter Store-Load barrier for x86 leveraging memory ambiguity
This is an example on how to implement a Store-Load barrier on x86 without resorting to a full memory barrier (`mfence`) or a locked instruction (`xchg`).
This can be used to implement eg. [Peterson's algorithm](https://en.wikipedia.org/wiki/Peterson%27s_algorithm).

Note: I do not think it is really usable in practice as it relies on undocument behaviour of x86 implementation.

## The problem it solves
x86 memory model can be found Intel manual Section 8.2.2 "Memory Ordering in P6 and More Recent Processor Families".
The most important bits to be aware of are:
 1. Loads are not reordered with other loads.
 2. Stores are not reordered with other stores.
 3. Stores are not reordered with older loads.
 4. **Loads  may  be  reordered  with  older  stores  to  different locations**  but  not  with  older  stores to the same location.
 5. Reads or writes cannot be reordered with I/O instructions, locked instructions, or serializing instructions.

Point 4 is the issue. To enforce that a load will always happen after a store to a different location, we must either use:
 * a locked instruction (eg. `xchg`)
 * a full memory barrier (`mfence`)

Both can have an order of magnitude of ~100 cycles.

## The lighter alternative

What if we could combine point 1 and 4 to our advantage? Ie adding a dummy load depending from our store happening before the other load?
Unfortunately we have one more issue to manage: store-forwarding. With store-forwarding, concurrent stores by two processors to be seen in different orders by those two processors.
As of now, a known way to prevent store-forwarding is to have a dependent load targeting overlapping address with the dependent store. This will prevent the processor to match the load target address with the previous store address.
Phew.
