
in pipeline.c -- dependent chain and multiple accumulators

02 and 03 make this go away. probably doing vector computations???  Take a look.

Prompt history for this is interesting.   Claude Opus 4.5 have lots of wrong answers.


In pipeline.tempvar.cpp

Why does the code >>2?

The >> 2 isn't special — it's there to   
  make the computation complex enough that 
  the compiler can't optimize it away or
  reduce it to a simple linear recurrence. 
                                           
  The expression t * 3 ^ (t >> 2) does two 
  things together:                         
                                           
  1. * 3 — a multiply (high latency, ~3    
  cycles) that creates a real dependency
  worth measuring                          
  2. ^ (t >> 2) — XOR with a right-shifted
  copy, which mixes bits so successive     
  values don't form a trivial arithmetic
  pattern the compiler could collapse      
                  
  Without the XOR, x = x * 3 + data[i] over
   100M iterations is just a linear
  recurrence, and the compiler (especially 
  at -O2) might recognize the pattern and
  transform the loop. Adding ^ (t >> 2)
  breaks that — the output depends on the
  input in a non-linear way, forcing the
  compiler to actually execute each
  iteration.

  The specific value 2 isn't meaningful. It
   could be 1 or 3 and behave the same way.
   The original pipeline.cpp even uses both
   — >> 1 and >> 2 — in dependentChain for
  the same reason: just to create enough
  bit-mixing that the loop is opaque to the
   optimizer.

  Short version: it's a complexity anchor  
  to keep the benchmark honest, not an
  intentional algorithmic choice.  



** why does -O3 make the runtime go to 0?  **
                                         
⏺ Two things happen at -O3:                
                                           
  1. Auto-vectorization (SIMD)             
                  
  -O3 enables the vectorizer. withTempVars 
  is a perfect candidate — four independent
   chains with no cross-dependencies. The  
  compiler rewrites the loop to use SIMD   
  registers (e.g., AVX2 processes 4× 64-bit
   values in a single instruction). What
  took 28ms at -O2 now finishes in
  microseconds. duration_cast<milliseconds>
   rounds that to 0.

  noTempVars is harder to vectorize (serial
   dependency on x), but -O3 unrolls it
  aggressively and may still manage partial
   vectorization. 

  2. The computation may be eliminated     
  entirely
                                           
  The clobber() barrier tells the compiler 
  "assume memory changed" — but x, x0..x3,
  and t0..t3 live entirely in registers,   
  never touching memory. At -O3 with
  aggressive inlining, the compiler can see
   through the whole benchmark template,
  determine the data array never changes,
  and constant-fold the result at compile
  time. If that happens, the timing loop
  runs with no real work inside it.

  How to tell which one it is:             
   
  g++ -O3 -S -o pipeline.tempvar.s         
  pipeline.tempvar.cpp                     
   
  Look for vmulq / vpaddq (SIMD) — that's  
  vectorization. If you see the result
  loaded as an immediate constant, it was  
  folded away.    

  Fix: switch the timer to microseconds:   
   
  long long t =                            
  duration_cast<microseconds>(end -        
  start).count();
                                           
  and print " µs" instead of " ms". That   
  will show the real SIMD speedup rather
  than 0.                                 
