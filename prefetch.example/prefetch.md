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

### Rerun on x86 (AMD Ryzen AI 9 HX 370, Zen 5)

The source predicts the technique "shows its value most on x86." It does not — at least not
with this example on this machine. Measured best-of-5, built two ways (g++ 13 with libstdc++,
and clang 18 with libc++); both toolchains agree:

| Case | plain | prefetched | verdict |
|---|---:|---:|---|
| Sequential | 0.13–0.22 ms | 0.17–0.22 ms | flat / slightly worse (HW prefetcher already covers it) |
| Pointer chasing | ~107–109 ms | ~108–110 ms | **no benefit** |

So software prefetch is a wash here, just as it was on Apple Silicon — but for a different reason
than "low memory latency." The pointer-chasing loop is a **dependent load chain** (`n = n->next`):
you can't prefetch far ahead because node k+3's address isn't known until k+2 has been
dereferenced. The example prefetches only two hops (`n->next->next`), and with a near-empty loop
body (`s += n->value`) there is no independent computation to overlap the miss against. The dataset
(1M × 64 B nodes = 64 MB) overflows the 24 MB L3, so every hop is a real DRAM miss, and two hops of
lead time isn't enough to hide ~100 ns of latency. The serial `n = n->next` dependency — not the
prefetch of `next->next` — is the actual bottleneck, and software prefetch can't break it.

Takeaway: explicit prefetch pays off when addresses are known ahead of time (so you can prefetch
*far* ahead) and there is real work to overlap the miss with. A tight, latency-bound linked-list
traversal has neither, which is why it stays flat on both ARM and x86.