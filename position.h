#pragma once
#include <iostream>
using namespace std;

/**
 * Position in the grid where _x and _y vary from
 * 1 to 9
 */
typedef struct Position {
	int _y;
	int _x;
	
} Position;

ostream& Position_print( ostream& out, Position p );

/**
 * Position in the grid where _x and _y vary from
 * 1 to 9 and cost (number of values set in the block
 * where (_y,_x) appears
 */
typedef struct PositionCost {
	int _y;
	int _x;
	int _cost;
	
} PositionCost;

ostream& PositionCost_print( ostream& out, PositionCost pc );



