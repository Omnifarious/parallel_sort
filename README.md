# C++ Parallel Sort Test #

This is just me playing around with a simple parallelization for sorting
a large array of integers.

I'm mostly please with how I used a queue to end the whole thing with a
set of maximally parallel merges.

I also wrote a non-parallel version in C. C doesn't have widely
available portable threading.

It's the result of me having an argument with someone over C being
"inherently faster" than C++. At the time, threading wasn't widely
available as a part of standard C, and so my trick for winning the
argument hands down was to parallelize it.

But, I didn't have to. C++'s `::std::sort` is going to be faster than
C's `qsort` in basically all cases. I'll leave it as an exercise to the
reader as to why.
