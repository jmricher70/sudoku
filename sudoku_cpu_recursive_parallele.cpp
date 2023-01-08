#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <iterator>
#include <algorithm>
using namespace std;
#include <getopt.h>
#include "grid.h"


ostream& operator<<( ostream& out, Position& pos ) {
	return Position_print( out, pos );
}

ostream& operator<<( ostream& out, PositionCost& pc ) {
	return PositionCost_print( out, pc );
}

ostream& operator<<( ostream& out, BlockCost& bc ) {
	return BlockCost_print( out, bc );
}

ostream& operator<<( ostream& out, Grid& grid ) {
	return Grid_print( out, grid );
}

// ==================================================================
// GLOBAL VARIABLES
// ==================================================================
int nbr_solutions = 0;
int verbose_level = 1;
bool reverse_flag = false;
int nbr_blocks = 1;

string satisfied_strings[] = { 
	"unsatisfied", 
	"almost", 
	"*** not defined ***", 
	"satisfied" 
};

void Grid_solve_recursive_( Grid& g, vector< Position > &empty_positions, int n ) {

	if (n >= static_cast<int>( empty_positions.size() ) ) {
	
		if (Grid_satisfied( g ) == SATISFIED) {
		
			#pragma omp critical
			{
				if (verbose_level >= 2) cout << g << endl;
				++nbr_solutions;
			}
						
		}
	
	} else {
	
		Position p = empty_positions[ n ];
		
		for (GridElementType v = MIN_VAL; v <= MAX_VAL; ++v ) {
		
			g[ p._y ][ p._x ] = v;
			
			if (Grid_satisfied( g ) != UNSATISFIED) {
				
				Grid_solve_recursive_( g, empty_positions, n + 1 );
			
			}
			
			g[ p._y ][ p._x ] = ZERO;
			
		}
	}

}



/**
 * Recursively solve the Sudoku given the list of zero positions
 *
 */
void Grid_solve_recursive( int nbr_grids, Grid *tab_grids ) {

	vector< Position > empty_positions;
	
	Grid_find_empty_positions( tab_grids[ 0 ], empty_positions );
	
	if (reverse_flag) {
		reverse( empty_positions.begin(), empty_positions.end() );
	}
	
	if (verbose_level >= 2) {
		cout << "- empty positions=" << empty_positions.size() << endl;
		for (auto p : empty_positions) {
			cout << p << endl;
		}
	}

	#pragma omp parallel for
	for ( int grid_id = 0; grid_id < nbr_grids; ++grid_id ) {
	
		Grid_solve_recursive_( tab_grids[ grid_id ], 
			empty_positions, 0 );
			
	}


}


/**
 * Recursively instantiate some cells
 *
 */
void fill_grids( Grid *tab_grids, int nbr_blocks, 
		vector< BlockCost >& blocks_costs, int& grid_id, int n ) {
	
	if ( n >= nbr_blocks ) {
	
		for (int i = 0; i < n; ++i) {

			int block = blocks_costs[ i ]._block;
			vector<int>& values = blocks_costs[ i ]._values;
			
			Grid_fill_block( tab_grids[ grid_id ], block, values );
			
		}
		
		++grid_id;
		
	} else {
	
		vector<int>& values = blocks_costs[ n ]._values;
		
		do {
		
			fill_grids( tab_grids, nbr_blocks, blocks_costs, grid_id, n + 1 );
			
		} while ( next_permutation( values.begin(), values.end() ) );
		
	}
	
}


/**
 * main function
 *
 */
int main( int argc, char *argv[] ) {

	string input;
	string input_file_name;
		
	static struct option long_options[] = {
	
		{ "verbose-level", required_argument, 0,  0 },
		{ "input", required_argument, 0,  0 }, 
		{ "blocks", required_argument, 0,  0 }, 
		{ "reverse", no_argument, 0, 0 }, 
		{ 0, 0, 0, 0 }
		
	};

	int option_index = 0;
	while (true) {
	
		int c = getopt_long( argc, argv, "v:i:b:r", long_options, &option_index );
	
		if (c == -1) break;

		switch( c ) {
			case 'v': 
				verbose_level = atoi( optarg );
				break;
				
			case 'i':
				input_file_name = optarg;
				break;
					
			case 'r':
				reverse_flag = true;
				break;
				
			case 'b':
				nbr_blocks = atoi( optarg );
				break;	
					
			default:
				cerr << "Unknown option	!" << endl;
				exit( EXIT_FAILURE );
		}
		
	}
		
	srand( time( nullptr ) );
	
	Grid initial_grid;
	
	Grid_init( initial_grid );
		
	// 2315 solutions
	input = 
	"1 2 5 7 3 6 4 0 0 "
	"0 0 0 0 1 8 5 0 0 "
	"0 0 0 5 0 4 0 0 0 "
	"0 0 0 0 5 0 0 2 0 "
	"0 0 0 0 0 0 0 0 0 "
	"0 0 0 2 7 0 0 0 0 "
	"0 8 3 0 0 0 0 0 0 "
	"0 0 0 6 0 2 3 9 0 "
	"0 0 0 0 0 0 8 4 7 ";

	
	if (input_file_name.size() != 0) {
		
		cout << "- read file " << input_file_name << endl;

		ifstream ifs( input_file_name );
				
		if (ifs.is_open()) {

			std::string str( (std::istreambuf_iterator<char>(ifs)),
                 std::istreambuf_iterator<char>());
                 
            input = str;     

		} else {
		
			cout << "! error: could not open file '" << input_file_name << "'" << endl;
			
		}
		
	}
	
	Grid_fill( initial_grid, input );
	
	if (verbose_level >= 1) {
		cout << endl;
		cout << "- initial grid" << endl;
		cout << initial_grid << endl;
	}
		
	
	int satisfiability = Grid_satisfied( initial_grid );
	cout << endl;
	cout << "- initial grid satisfied ? " << satisfied_strings[ satisfiability ] << endl;
	cout << endl;
	
	if (satisfiability == UNSATISFIED) {
	
		cout << "! error: can't solve a grid that is already UNSATISFIED" << endl;
			
	} else {
	
		vector< BlockCost > blocks_costs;
		
		for (int block = MIN_VAL; block <= MAX_VAL; ++block) {
			BlockCost bc;
			bc._block = block;
			bc._permutations = 0;
			bc._cost = Grid_block_nbr_values_set( initial_grid, block, bc._values );
			
			if (Grid_blk_satisfied( initial_grid, block ) != SATISFIED) {
			
				blocks_costs.push_back( bc );
				
			}
		}
		
		// sort by most constrained block
		std::sort( blocks_costs.begin(), blocks_costs.end(), []( BlockCost& a, BlockCost& b) {
			return a._cost > b._cost;
		} );	
		
		//
		// By using 'nbr_blocks' and their permutations, 
		// we estimate the number of grids to use and fill
		//
				
		cout << "- evaluate number of permutations:" << endl;
				
		int total_permutations = 1;
		
		for (int i = 0; i < nbr_blocks; ++i) {
		
			int nbr_permutations = 0;		
			
			std::vector<int>& values = blocks_costs[ i ]._values;
			
			do {
			
				++nbr_permutations;
				
			} while (next_permutation( values.begin(), values.end()));
		
			if (verbose_level >= 1) {
				cout << "-- number of permutations for block " << blocks_costs[ i ]._block;
				cout << " = " << nbr_permutations << endl;		
			}
			
			blocks_costs[ i ]._permutations = nbr_permutations;
			
			total_permutations *= nbr_permutations;
		}
		
		if (verbose_level >= 2) {
			cout << "blocks costs=" << blocks_costs.size() << endl;
			for (auto bc : blocks_costs) {
				cout << bc << endl;
			}
		}
		
		cout << "- total number of permutations with " << nbr_blocks << " block(s) = ";
		cout << total_permutations << endl;;
		
	
		//
		// Now we create the grids from the initial grid and fill them
		//
		
		Grid *tab_grids = new Grid[ total_permutations ];
		for (int i = 0; i < total_permutations; ++i) {
			Grid_copy( tab_grids[ i ], initial_grid );
		}
		
		int grid_id = 0;
		
		fill_grids( tab_grids, nbr_blocks, blocks_costs, grid_id, 0 );
		
		if (verbose_level >= 2) {

			cout << "- all grids" << endl;
			
			for (int i = 0; i < total_permutations; ++i) {
			
				cout << i << endl << tab_grids[ i ] << endl;
			}
		
		}	
				
		cout << endl;
		cout << "- start search" << endl;
		
		Grid_solve_recursive( total_permutations, tab_grids );
		
	}	
		
	cout << endl;
	cout << "- number of solutions=" << nbr_solutions << endl;
		
	return EXIT_SUCCESS;
}
	
