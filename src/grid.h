/*
=====================================================================
    Project: Sudoku
     Author: Jean-Michel RICHER
      Email: jean-michel.richer@univ-angers.fr
 Created on: January, 2023
===================================================================== 
  Resolution of the Sudoku puzzle based on sequential or parallel
  implementations which can be iterative or recursive. We also
  provide a GPU iterative version.
=====================================================================
*/
#pragma once

#include <iostream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cstring>
#include <cstdint>
using namespace std;
#include "position.h"
#include "block_cost.h"

// Dimension of the grid + 1
const int DIM = 10;
// Values in the range of values allowed
const int MIN_VAL = 1;
const int MAX_VAL = 9;
const int ZERO = 0;


/**
 * Type of an element of the grid: use a 8, 16 or 32 integer
 * or unsigned integer
 */
typedef int32_t GridElementType;

/**
 * Definition of a grid which is a square matrix of size DIM.
 * Note that row 0 and column 0 are not used.
 */
typedef GridElementType Grid[ DIM ][ DIM ];

/**
 * Initialize a grid and fill all elements with 0
 */
void Grid_init( Grid& g );

/**
 * Fill grid with given string of integers
 *
 */
void Grid_fill( Grid& g, string s );

/**
 * Copy contents of source grid to destination grid
 *
 */
void Grid_copy( Grid& dst, Grid& src );

/**
 * Print grid on standard output
 * 
 */
ostream& Grid_print( ostream& out, Grid& g );


// Constants for constraints
// we use the values 0, 1, 3 in order to compute
// the result of the satisfaction of two constraints
// by applying a 'and' between the two results
//
//   X | U | A | S |       U = 0 (Unsatisfied)
//  ----------------       A = 1 (Almost satisfied)
//   U | U | U | U |       S = 3 (Satisfied)
//  ----------------
//   A | U | A | A |
//  ----------------
//   S | U | A | S |
//  ----------------
 
const int UNSATISFIED = 0;
const int ALMOST      = 1;
const int SATISFIED   = 3;

//
// The product of the elements in a row in a column or a block
// must be 9!

const int FACTORIAL_9 = 362880;

/**
 * Check if values in a row satisfy or violate the 
 * alldiff constraint
 *
 * If some values are not set (i.e. the corresponding cell
 * contains a value of 0) then we can not evaluate the
 * satisfaction of the constraint and we return ALMOST
 *
 * While we parse values we compute their product and
 * if it is equal to 9! then the constraint is SATISFIED
 */

int Grid_row_satisfied( Grid& g, int y );


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

int Grid_col_satisfied( Grid& g, int x );


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

int Grid_blk_satisfied( Grid& g, int b );

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

int Grid_satisfied( Grid& g );

/**
 * Find all positions in a grid that are not set with a value 
 * between 1 and 9. Positions are return in a vector.
 *
 */
void Grid_find_empty_positions( Grid& grid, vector<Position>& positions );

/**
 * Find all positions and thier costs where the cost is the number of 
 * elements already set in the block where the position occurs.
 *
 */
void Grid_find_empty_positions_costs( Grid& grid, vector<PositionCost>& positions_costs );

/**
 * Return number of values already set in a block where
 * the position occurs.
 *
 */
int Grid_position_nbr_values_set( Grid& grid, Position& p );

/**
 * Return number of values already set in a block 
 *  
 */
 
int Grid_block_nbr_values_set( Grid& grid, int block, vector<int>& values );

/**
 * Fill block of the grid with elements of values.
 * The number of elements of the vector values must be equal to
 * the number of elements not set in the block
 */
 
void Grid_fill_block( Grid& grid, int block, vector<int>& values );

