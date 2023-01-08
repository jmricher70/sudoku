# sudoku
Resolution of the SUDOKU puzzle with CPU and GPU and sequential or parallel implementation.

# Implementations

We have defined five different implementations:
- 4 implementations for the resolution of the problem on the CPU
- 1 implementation on the GPU

## CPU Implementations

For the CPU, we have the following implementations:
- sequential (or mono thread) with a recursive search
- sequential with an iterative search
- parallel (or multi threads) with a recursive search
- parallel with an interative search

## GPU Implementation

For the GPU we have only one parallel (or many threads) iterative implementation

Note that for GPUs a recursive implementation will often fail due to the amount of
recursive calls. It is then necessary to use an iterative version of the code.

You can find the code of the iterative version in file 'src/sudoku_gpu_iterative_parallele.cu'.
Here, parallel is written with an 'e' at the end like in french ;-)
There is probably a better way to code this iterative version but I didn't have time to
do this. Would copilot be able to do such thing ?

## Compilation and execution

To compile the binaries, simply type:

```
make
```

To run a simple performance test to compare the different implementations, type:

```
make tests
```

Results will be put under a subdirectory in directory 'results'.
You will find the execution time for my processor and GPU in
'results/AMD_Ryzen_5_5600G_with_Radeon_Graphics_NVIDIA_GeForce_RTX_3050':

---------------------------------------------------------------
Implementation           USER  ELAPSED SOLUTIONS
---------------------------------------------------------------
CPU Sequential Recursive 0.20  0.20      2315
CPU Sequential Iterative 0.20  0.20      2315
CPU   Parallel Recursive 2.94  1.96      2315
CPU   Parallel Iterative 2.98  2.00      2315
GPU   Parallel Iterative 4.75  4.84      2315
---------------------------------------------------------------
number of solutions expected is 2315


