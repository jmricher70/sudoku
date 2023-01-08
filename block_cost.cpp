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
  
  See the LICENSE file.
  
=====================================================================
*/

#include "block_cost.h"

ostream& BlockCost_print( ostream& out, BlockCost bc ) {
	out << "(block=" << bc._block << ", cost=" << bc._cost;
	out << ", permutations=" << bc._permutations << ", values=[";
	for (auto v : bc._values) out << v << " ";
	out << "]";
	return out;
}


