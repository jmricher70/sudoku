#include "grid.h"

void Grid_init( Grid& g ) {

	memset( &g, 0, sizeof( Grid ) );
	
}



void Grid_fill( Grid& g, string s ) {

	istringstream ifs( s );
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
		for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
	
			GridElementType v;
			
			ifs >> v;
			
			g[ y ][ x ] = v;
			
		}
	} 
	
}


void Grid_copy( Grid& dst, Grid& src ) {
	
	memcpy( &dst, &src, sizeof( Grid ) );
	
}


ostream& Grid_print( ostream& out, Grid& g ) {

	out << "------------------------" << endl;

	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
		out << y << "| ";
		for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
			out << g[ y ][ x ] << " ";
			if ((x % 3) == 0) out << "|";
		}
		out << endl;
		if ((y % 3) == 0) {
			out << "------------------------" << endl;
		}

	} 
	
	return out;
	
}

int Grid_row_satisfied( Grid& g, int y ) {

#ifdef DEBUG
	assert( (MIN_VAL <= y) and (y <= MAX_VAL) );
#endif
	
	// store values found as powers of 2
	// this represents the alldiff constraint
	int values = 0;

	int product = 1;	
	
	for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
	
		GridElementType v = g[ y ][ x ];

		//printf("product=%d, value=%d\n", product, v );
		
		if (v == ZERO) continue;

		if ((values & (1 << v)) != 0) {
			return UNSATISFIED;
		}
		product *= v;
		values |= (1 << v);
	}
	
	//printf("GridRowCtr y=%d prod=%d\n", y, product );
	
	return (product == FACTORIAL_9) ? SATISFIED : ALMOST; 
	
}

int Grid_col_satisfied( Grid& g, int x ) {


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

int Grid_blk_satisfied( Grid& g, int b ) {

	int y = ((b - 1) / 3) * 3 + 1;
	int x = ((b - 1) % 3) * 3 + 1;
	
#ifdef DEBUG	
	assert( (MIN_VAL <= x) and (x <= MAX_VAL) );
#endif
	
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


int Grid_satisfied( Grid& g ) {

	// by default we consider the problem as SATISFIED
	int satisfiability = SATISFIED;
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
	
		int tmp_satisfiability = Grid_row_satisfied( g, y );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
		
		//printf("satisfiability=%d\n", satisfiability );	
	}
	
	for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
	
		int tmp_satisfiability = Grid_col_satisfied( g, x );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
		
	}

	for (int b = MIN_VAL; b <= MAX_VAL; ++b) {
	
		int tmp_satisfiability = Grid_blk_satisfied( g, b );
		if (tmp_satisfiability == UNSATISFIED) return tmp_satisfiability;
		satisfiability &= tmp_satisfiability;
		
	}
	
	return satisfiability;
	
}


void Grid_find_empty_positions( Grid& g, vector<Position>& positions ) {

	Position p;
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
		for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
		
			GridElementType v = g[ y ][ x ];
					
			if ( v == ZERO ) {

				p._y = y;
				p._x = x;

				positions.push_back( p );
				
			}
		}
	}	
	
	
}


int Grid_position_nbr_values_set( Grid& g, Position& p ) {

	int block = ((p._y-1) / 3) * 3 + ((p._x-1) / 3) + 1;
	
	int y = ((block - 1) / 3) * 3 + 1;
	int x = ((block - 1) % 3) * 3 + 1;
	
	int count = 0;
	
	for (int r = 0; r < 3; ++r) {
		for (int s = 0; s < 3; ++s) {
		
			GridElementType v = g[ y + r ][  x + s ];
			
			if (v != ZERO) ++count;
			
		}
	}
	
	return count;
}

int Grid_block_nbr_values_set( Grid& g, int block, vector<int>& missing_values ) {

	int y = ((block - 1) / 3) * 3 + 1;
	int x = ((block - 1) % 3) * 3 + 1;
	
	int count = 0;
	int values = 0;
	
	for (int r = 0; r < 3; ++r) {
		for (int s = 0; s < 3; ++s) {
		
			GridElementType v = g[ y + r ][ x + s ];
			
			if (v != ZERO) {
				++count;
				values |= (1 << v);
			}
		}
	}
	
	for (int v = MIN_VAL; v <= MAX_VAL; ++v) {
		if ((values & (1 << v)) == 0) {
			missing_values.push_back( v );
		}
	}
	
	return count;
	
}


void Grid_find_empty_positions_costs( Grid& g, vector<PositionCost>& positions_costs ) {

	PositionCost pc;
	Position p;
	
	for (int y = MIN_VAL; y <= MAX_VAL; ++y) {
		for (int x = MIN_VAL; x <= MAX_VAL; ++x) {
		
			GridElementType v = g[ y ][ x ];
					
			if ( v == ZERO ) {

				pc._y = p._y = y;
				pc._x = p._x = x;
				pc._cost = Grid_position_nbr_values_set( g, p );
				
				positions_costs.push_back( pc );
			}
		}
	}	
	
	
}


void Grid_fill_block( Grid& g, int block, vector<int>& values ) {
	
	int y = ((block - 1) / 3) * 3 + 1;
	int x = ((block - 1) % 3) * 3 + 1;
	
	int k = 0;
	
	for (int r = 0; r < 3; ++r) {
		for (int s = 0; s < 3; ++s) {
	
			int grid_value = g[ y + r ][ x + s ];
			
			if (grid_value == ZERO) { 
			
				g[ y + r ][ x + s ] = values[ k ];
				++k;
				
			}	
		
		}
	} 
	
}
