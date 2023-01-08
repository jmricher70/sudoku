#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <iterator>
#include <algorithm>
#include <numeric>
using namespace std;
#include <getopt.h>
#include "grid.h"


#include "gpu_grid.cu"

// ==================================================================
// CUDA / CUME
// ==================================================================
#include <cuda.h>

#define H2D cudaMemcpyHostToDevice
#define D2H cudaMemcpyDeviceToHost

// ------------------------------------------------------------------
// definition of a macro instruction that checks if a CUDA function
// was successull or not. If the execution of the function resulted
// in some error we display it and stop the program
// ------------------------------------------------------------------
#define cume_check(value) {     \
  cudaError_t err = value; \
  if (err != cudaSuccess) {       \
    cerr << endl; \
    cerr << "============================================\n"; \
    cerr << "Error: " << cudaGetErrorString(err) << " at line "; \
    cerr << __LINE__ << " in file " <<  __FILE__;   \
    cerr <<  endl; \
    exit(EXIT_FAILURE); \
  } \
}
 
// ------------------------------------------------------------------
// Same as cuda_check but for kernel. This macro instruction is used
// after the execution of the kernel (see the macros KERNEL_EXECUTE_NR
// and KERNEL_EXECUTE_WR in cume_kernel.h)
// ------------------------------------------------------------------
#define cume_check_kernel() { \
  cudaError_t err = cudaGetLastError(); \
  if (err != cudaSuccess)  { \
    cerr << endl; \
    cerr << "============================================\n"; \
    cerr << "Kernel Error: " << cudaGetErrorString(err) << " at line "; \
    cerr << __LINE__ << " in file " <<  __FILE__;   \
    cerr <<  endl; \
    exit(EXIT_FAILURE); \
  } \
}

#define dump(var) cout << #var << "=" << var << endl;

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
bool print_first_flag = false;

string satisfied_strings[] = { 
	"unsatisfied", 
	"almost", 
	"*** not defined ***", 
	"satisfied" 
};

extern __device__ int GPU_Grid_satisfied( Grid *g );

/**
 * Iteratively solve the Sudoku given the list of zero positions
 *
 */
__device__
int device_Grid_solve_iterative( int gtid, 
	Grid& g, 
	int *tab_nbr_solutions,
	int nbr_positions, 
	Position* tab_positions ) {
	
	int nb_sol = 0;
	 
	int m = nbr_positions; 
	
	int i = 0;
	while (i < m) {
				
		if (i == m) {
		
			--i;
			
		} else {	
			
			int y = tab_positions[ i ]._y;
			int x = tab_positions[ i ]._x;
			++g[ y ][ x ];
			
			
			bool exit_loop = false;
			while ((i > 0) and (exit_loop == false)) {
				if ( g[ y ][ x ] > MAX_VAL) {
					g[ y ][ x ] = 0;
					--i;
					y = tab_positions[ i ]._y;
					x = tab_positions[ i ]._x;
			
					++g[ y ][ x ];
				} else {
					exit_loop = true;
				}
			}
		}

		if ( g[ tab_positions[ 0 ]._y ][ tab_positions[ 0 ]._x ] > MAX_VAL) break;
		
		int sat = GPU_Grid_satisfied( g );
		
		
		if ( sat  != UNSATISFIED ) {
			++i;
		}
				
		if (i == m) {
			
			if ( GPU_Grid_satisfied( g ) == SATISFIED ) {
					
				++nb_sol;

			}
			
			--i;		
		}	
		
	}
	
	return nb_sol;

}

__global__
void kernel_Grid_solve_iterative( int nbr_grids, 
	Grid *tab_grids, 
	int *tab_nbr_solutions,
	int nbr_positions, 
	Position* tab_positions ) {


	int gtid = blockDim.x * blockIdx.x + threadIdx.x;
		
	if (gtid < nbr_grids) {
	
		tab_nbr_solutions[ gtid ] =
		device_Grid_solve_iterative( gtid, 
			tab_grids[ gtid ],
			tab_nbr_solutions,
			nbr_positions, 
			tab_positions 
		);
		
	}
	
	
}

/**
 * Iteratively solve the Sudoku given the list of zero positions
 *
 */
void Grid_solve_iterative( int nbr_grids, Grid *cpu_tab_grids ) {

	vector< Position > empty_positions;
	
	Grid_find_empty_positions( cpu_tab_grids[ 0 ], empty_positions );

	int nbr_positions = static_cast<int>( empty_positions.size() );
	
	if (reverse_flag) {
	
		reverse( empty_positions.begin(), empty_positions.end() );
		
	}
	
	if (verbose_level >= 2) {
		cout << "- empty positions=" << empty_positions.size() << endl;
		for (auto p : empty_positions) {
			cout << p << endl;
		}
	}

	Position *cpu_tab_positions = new Position[ nbr_positions ] ;
	{
	
		int i = 0;
		for (auto p : empty_positions) {
			cpu_tab_positions[ i ] = p;
			++i;
		}
		
	}
	
	Position *gpu_tab_positions;

	cume_check( cudaMalloc( (void **) &gpu_tab_positions, nbr_positions * sizeof( Position ) ) );
	cume_check( cudaMemcpy( gpu_tab_positions, cpu_tab_positions, nbr_positions * sizeof( Position ), H2D ) );

	int *cpu_tab_nbr_solutions = new int [ nbr_grids ];

	int *gpu_tab_nbr_solutions;
	
	cume_check( cudaMalloc( (void **) &gpu_tab_nbr_solutions, nbr_grids * sizeof( int ) ) );
	//cume_check( cudaMemset( gpu_tab_nbr_solutions, 0, nbr_grids * sizeof( int ) ) );


	Grid *gpu_tab_grids;
	
	cume_check( cudaMalloc( (void **) &gpu_tab_grids, nbr_grids * sizeof(Grid) ) );
	cume_check( cudaMemcpy( gpu_tab_grids, cpu_tab_grids, nbr_grids * sizeof(Grid), H2D ) );

	dim3 cuda_grid(1,1,1) , cuda_block(1,1,1);
	
	const int MAX_THREADS_PER_BLOCK = 1024;
	
	if (nbr_grids < MAX_THREADS_PER_BLOCK) {
		cuda_block.x = nbr_grids;
	} else {
		cuda_block.x = MAX_THREADS_PER_BLOCK;
		cuda_grid.x = ((nbr_grids + MAX_THREADS_PER_BLOCK - 1) / MAX_THREADS_PER_BLOCK);
	}
	
	cout << endl;
	cout << "- cuda  grid( x=" << cuda_grid.x << ",y=" << cuda_grid.y << ",z=" << cuda_grid.z << " )" << endl;
	cout << "- cuda block( x=" << cuda_block.x << ",y=" << cuda_block.y << ",z=" << cuda_block.z << " )" << endl;
	
	cout << "- start kernel" << endl;
	
	kernel_Grid_solve_iterative<<< cuda_grid, cuda_block >>>( nbr_grids,
		gpu_tab_grids, 
		gpu_tab_nbr_solutions, 
		nbr_positions,
		gpu_tab_positions 
	);
	cume_check_kernel();
	
	cume_check( cudaMemcpy( cpu_tab_grids, gpu_tab_grids, nbr_grids * sizeof(Grid), D2H) );
	
	cume_check( cudaMemcpy( cpu_tab_nbr_solutions, gpu_tab_nbr_solutions, nbr_grids * sizeof(int), D2H) );
	
	//cout << "solutions:" << endl;
	//copy( &cpu_tab_nbr_solutions[ 0 ], &cpu_tab_nbr_solutions[ nbr_grids ], ostream_iterator<int>( cout, " ") ); 
	
	nbr_solutions = std::accumulate( &cpu_tab_nbr_solutions[ 0 ], &cpu_tab_nbr_solutions[ nbr_grids ], 0 );
	
	cudaFree( gpu_tab_positions );
	cudaFree( gpu_tab_grids );
	cudaFree( gpu_tab_nbr_solutions );
	
	delete [] cpu_tab_nbr_solutions;
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
		{ "print-first", no_argument, 0, 0 },
		{ 0, 0, 0, 0 }
		
	};

	int option_index = 0;
	while (true) {
	
		int c = getopt_long( argc, argv, "v:i:b:rf", long_options, &option_index );
	
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
			
			case 'f': 
				print_first_flag = true;
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
		int nbr_grids = total_permutations;
		
		Grid *tab_grids = new Grid[ nbr_grids ];
		
		for (int i = 0; i < nbr_grids; ++i) {
		
			Grid_copy( tab_grids[ i ], initial_grid );
			
		}
		
		int grid_id = 0;
		
		fill_grids( tab_grids, nbr_blocks, blocks_costs, grid_id, 0 );
		
		if (verbose_level >= 2) {

			cout << "- all grids" << endl;
			
			for (int i = 0; i < nbr_grids; ++i) {
			
				cout << i << endl << tab_grids[ i ] << endl;
			}
		
		}	
				
		Grid_solve_iterative( nbr_grids, tab_grids );
		
	}	
	
	cout << endl;	
	cout << "- number of solutions=" << nbr_solutions << endl;
		
	return EXIT_SUCCESS;
}
	
