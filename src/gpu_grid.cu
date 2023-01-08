#include "gpu_grid.h"

__device__ 
int GPU_Grid_row_satisfied( Grid& g, int y ) {
	
	// store values found as powers of 2
	// this represents the alldiff constraint
	int values = 0;

	int product = 1;	

	for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
	
		GridElementType v = g[ y ][ x ];
		 
		if (v == ZERO) continue;
		
		if ((values & (1 << v)) != 0) {
			return UNSATISFIED;
		}
		product *= v;
		values |= (1 << v);
	}

	return (product == FACTORIAL_9) ? SATISFIED : ALMOST; 
	
}


__device__ 
int GPU_Grid_col_satisfied( Grid& g, int x ) {


	// store values found as powers of 2
	// this represents the alldiff constraint
	int values = 0;

	int product = 1;	
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
		
		GridElementType v = g[ y ][ x ];	
		
		if (v == ZERO) continue;
		
		if ((values & (1 << v)) != 0) {
			return UNSATISFIED;
		}
		
		product *= v;
		values |= (1 << v);
	}
	
	return (product == FACTORIAL_9) ? SATISFIED : ALMOST; 
}


__device__ 
int GPU_Grid_blk_satisfied( Grid& g, int b ) {

	int y = ((b - 1) / 3) * 3 + 1;
	int x = ((b - 1) % 3) * 3 + 1;
	
	
	// store values found as powers of 2
	// this represents the alldiff constraint
	int values = 0;

	int product = 1;	
	
	for (int r = 0; r < 3; ++r) {
		for (int s = 0; s < 3; ++s) {
		
			GridElementType v = g[ y + r ][ x + s ];	
			
			if (v == ZERO) continue;
			
			if ((values & (1 << v)) != 0) {
				return UNSATISFIED;
			}
			
			product *= v;
			values |= (1 << v);
		}
	}
	
	return (product == FACTORIAL_9) ? SATISFIED : ALMOST; 
}


__device__ 
int GPU_Grid_satisfied( Grid& g ) {


	// by default we consider the problem as SATISFIED

	int satisfiability = SATISFIED;
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
	
		int tmp_satisfiability = GPU_Grid_row_satisfied( g, y );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
	
	}
	
	for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
	
		int tmp_satisfiability = GPU_Grid_col_satisfied( g, x );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
		
	}

	for (int b = MIN_VAL; b <= MAX_VAL; ++b) {
	
		int tmp_satisfiability = GPU_Grid_blk_satisfied( g, b );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
		
	}
	
	return satisfiability;
	
}


