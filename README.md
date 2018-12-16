# C++ Parallel Sort Test #

This is just me playing around with a simple parallelization for sorting
a large array of integers.

I'm mostly please with how I used a queue to end the whole thing with a
set of maximally parallel merges.

I also wrote a non-parallel version in C. C doesn't have widely
available portable threading.
