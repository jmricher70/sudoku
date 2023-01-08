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

## Compilation and execution

To compile the binaries, simply type:

```
make
```

To run a simple performance test to compare the different implementations, type:

```
./test.sh
```
