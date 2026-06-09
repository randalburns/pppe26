## Summary of files

We should start with an overview of what std::sort does and why it's so complex?

timing_size_breakdown.md -- characterize why quicksort alone is not good enough. all time spent in small partitions. Yes, it's the best algorithm, but no practical sort uses one algorithm.

small_sorting.md -- optimizing the base case with bitonic sorting networks.

vqs_highway.md -- how about using vectorizaiton for big data too?

full_sorting.md -- improving std::sort -- sorta amazing.