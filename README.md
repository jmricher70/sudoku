# sudoku
Resolution of the SUDOKU puzzle with CPU and GPU and sequential or parallel implementation.

# Implementations

We have defined five different implementations:
- 4 implementations for the resolution of the problem on the CPU
- 1 implementations on the GPU

## CPU Implementations

For the CPU we have the following implementations:
- sequential (or mono thread) with a recursive search
- sequential with an iterative search
- parallel (or multi threads) with a recursive search
- parallel with an interative search

## GPU Implementation

For the GPU we have only one parallel (or many threads) iterative implementation

Note that for GPUs a recursive implementation will often fail due to the amount of
recursive calls. It is then necessary to use an iterative version of the code.

You can check the code of the source code in file 'src/sudoku_gpu_iterative_parallele.cu'.
Here, parallel is written with a 'e' at the end like in french ;-)
There is probably a better way to code this iterative version but I didn't have time to
do this.

## Compilation and execution

To compile the binaries, simply type:

```
make
```

To run a simple performance test to compare the different implementations, type:

```
make tests
```
