## RB 
Prefetching doesn't really work on either platform.

## Prefetch

With simple examples, there is no benefit. HW already does a perfect job prefetching. Have to do something kind of contrived to break the HW prefetcher.

RB -- amazing to me that I can't build a prefetch example. I used these primtives successfully in a tree-example as recently as 2019.


### Claude had to work hard on this example.

> The prfm pldl3keep instruction IS emitted. Apple Silicon's hardware prefetcher is just very good. Let me try the canonical pointer-chasing (linked list) pattern —  that's genuinely unpredictable for hardware: 


> The data fits in M-series's large L3 (37MB+). Let me push the dataset larger than L3 to force real DRAM accesses:                                                    
─────     ────────────────────────

> Apple Silicon's unified memory has much lower latency than traditional DRAM, which is why the benefit is negligible here. The technique is still correct and widely used — it shows its value most on x86 or systems with higher memory latency. Let me write the final version that's honest about this:  

I encountered this problem in this class when I bought my first M1 processor.


This blog does a nice job with prefetching and when to use it.  

https://johnnysswlab.com/the-pros-and-cons-of-explicit-software-prefetching/