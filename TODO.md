## Different on Ryzen than M5
  * vectorization 
  * sorting -- highway has native performance (do it on Ryzen)
  * sharing CCX zones NUMA?

## RB Thoughts

There are really only 2 computations in all of CS.
  * sorting
  * matrix multiplication

* start with speedup
  * loop optimizations as examples of speedup.
* then pipelining note that these are processor things
    * and pipeline examples
* then let's talk about compilers
  * and do code generation
  * even last year it was generate and read assembly
  * now AI can tell you a lot about what's going on
* then ILP/CPI note that these are processor ops
  * out of order and speculative execution
* now that we have a deep undertand of regular, serial code let's do sorting
  * 


# Structure

* Intro (Currently Lec 0)
  * what simple example to DO???



* Amdahl's law and speedup (Lec 2)
* Looping unrolling as first form of optimization
  * think about the processor instructions, overhead, and branches 
    * unrolling (1 whole lecture)
    * loop fusion

* Compiler and optimizations levels
  * 02 -- auto vectorization, why we run all stuf at 01
  * strength reduction as something compiler does

* Branches
   * TODO something demonstrating how branch predication works.

* CPU stuff to realize ILP: CPI
  * OOO and speculative execution

* Pipelining -- 

* Cache hierarchy
  * loop interchange and tiling blocking. loop fission.
  * false sharing.

* Roofline -- before we do parallelism -- understand using the processor well.

* Joblib -- concept of parallelism
  * hazard of choosing to parallelize bad imnplementataions
  * roofline is our guide.

### Multicore Parallelism

OpenMP block parallelism -- prefix sum

Cilk and work stealing -- sparse_col_sum
  * parallel for
  * work stealing
  * dynamic OpenMP scheduling

Compare and contrast of OpenMP and Cilk.



### Stuff to cover from Claude

** Loop Optimizations **
Loop unrolling
Loop fusion
Loop fission/distribution
Loop interchange
Loop tiling/blocking


** Software pipelining **

Data Dependency Reduction
Multiple accumulators
Temporary variables
Strength reduction
Common subexpression elimination
Dead code elimination

** Memory Optimizations **

Data prefetching
Cache blocking
Structure of Arrays (SoA) vs Array of Structures (AoS). TODO
Memory alignment. TODO
Avoiding false sharing
Reducing pointer aliasing (restrict keyword). TODO

** Branch Optimizations **
Branch-free code (using arithmetic/bitwise ops)
Likely/unlikely hints
Lookup tables instead of branches
Predicated execution
Loop unswitching

** Instruction Scheduling **
Instruction reordering. (part of piplelining)
Separating dependent instructions (in ILP)
Interleaving independent operations (part of separating dependent instructions1)  -- this is redundant with loop fusion and other stuff.

