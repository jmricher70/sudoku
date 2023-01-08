#pragma once
#include "grid.h"


__device__ 
int GPU_Grid_row_satisfied( Grid& g, int y );


/**
 * Check if values in a column satisfy or violate the 
 * alldiff constraint
 *
 * If some values are not set (i.e. the corresponding cell
 * contains a value of 0) then we can not evaluate the
 * satisfaction of the constraint and we return ALMOST
 *
 * While we parse values we compute their product and
 * if it is equal to 9! then the constraint is SATISFIED
 */

__device__ 
int GPU_Grid_col_satisfied( Grid& g, int x );


/**
 * Check if values in a block satisfy or violate the 
 * alldiff constraint
 *
 * If some values are not set (i.e. the corresponding cell
 * contains a value of 0) then we can not evaluate the
 * satisfaction of the constraint and we return ALMOST
 *
 * While we parse values we compute their product and
 * if it is equal to 9! then the constraint is SATISFIED
 *
 */

__device__
int GPU_Grid_blk_satisfied( Grid& g, int b );

/**
 * Check if a grid is satisfied by determining if
 * all constraints on rows, columns and blocks
 * are satisfied.
 *
 * This function returns:
 * - UNSATISFIED if ONE of the constraints is UNSATISFIED
 * - SATISFIED if ALL constraints are SATISFIED
 * - ALMOST if ONE of the constraints is ALMOST satisfied
 */

__device__ 
int GPU_Grid_satisfied( Grid& g );

