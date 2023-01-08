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

string satisfied_strings[] = { 
	"unsatisfied", 
	"almost", 
	"*** not defined ***", 
	"satisfied" 
};



/**
 * Recursively solve the Sudoku given the list of zero positions
 *
 */
void Grid_solve_recursive( Grid& g, vector<PositionCost>& epc, int n = 0 ) {

	if (n == static_cast<int>( epc.size() ) ) {
	
		if (Grid_satisfied( g ) == SATISFIED) {

			if (verbose_level >= 2) cout << g << endl;
			++nbr_solutions;

		}
	
	} else {
	
		PositionCost pc = epc[ n ];
		
		for (GridElementType v = MIN_VAL; v <= MAX_VAL; ++v ) {
		
			g[ pc._y ][ pc._x ] = v;
			
			if (Grid_satisfied( g ) != UNSATISFIED) {
				
				Grid_solve_recursive( g, epc, n + 1 );
			
			}
			
			g[ pc._y ][ pc._x ] = ZERO;
			
		}
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
		{ "reverse", no_argument, 0, 0 }, 
		{ 0, 0, 0, 0 }
		
	};

	int option_index = 0;
	while (true) {
	
		int c = getopt_long( argc, argv, "v:i:r", long_options, &option_index );
	
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
		
			cout << "error: could not open file '" << input_file_name << "'" << endl;
			
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
	
	if (satisfiability == UNSATISFIED) {
	
		cout << "Error: can't solve a grid that is already UNSATISFIED" << endl;
			
	} else {
	
		vector< PositionCost > empty_positions_costs;
		Grid_find_empty_positions_costs( initial_grid, empty_positions_costs );
		
		std::sort( empty_positions_costs.begin(), empty_positions_costs.end(), 
			[]( PositionCost& a, PositionCost& b) {
				return a._cost > b._cost;
			} 
		);
		
		if (reverse_flag) {
			std::reverse( empty_positions_costs.begin(), empty_positions_costs.end() );
		}
		
		if (verbose_level >= 2) {
			cout << "empty positions=" << empty_positions_costs.size() << endl;
			for (auto pc : empty_positions_costs) {
				cout << pc << endl;
			}
		}
		
		cout << endl;
		cout << "- start search" << endl;
		
		Grid_solve_recursive( initial_grid, empty_positions_costs );
		
	}	
		
	cout << endl;	
	cout << "- number of solutions=" << nbr_solutions << endl;
		
	return EXIT_SUCCESS;
}
	
