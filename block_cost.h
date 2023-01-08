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
#include <vector>
using namespace std;

/**
 * Structure that records a block (from 1 to 9)
 * and the cost (number of values already set)
 * of the block, the values set in the block
 * and the number of permutations of the values
 */
typedef struct BlockCost {
	
	int _block;
	int _cost;
	int _permutations;
	std::vector<int> _values;
	
} BlockCost;

ostream& BlockCost_print( ostream& out, BlockCost bc );


