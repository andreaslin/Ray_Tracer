#include <iostream>
#include "scene_lua.hpp"
#include <sstream>
#include <cstdlib>						// exit
#include <cstring>						// strcmp
using namespace std;					// direct access to std

int AA_value;
bool BOUNDING;
int MAX_RECURSIVE_LEVEL;
bool PHOTON_MAPPING;
int NUM_THREADS;

bool convert( int &val, char *buffer ) {	// convert C string to integer
    std::stringstream ss( buffer );			// connect stream and buffer
    ss >> dec >> val;						// convert integer from buffer
    return ! ss.fail() &&					// conversion successful ?
	// characters after conversion all blank ?
	string( buffer ).find_first_not_of( " ", ss.tellg() ) == string::npos;
} // convert


void usage( char *argv[] ) {
    cerr << "Usage: " << argv[0]
	 << " [ [ [ [ [ [ File-Name " << " [ AA_value (0/1/2/3/4) "
	 << " [ BOUNDING (0/1) [ MAX_RECURSIVE_LEVEL ( 1-10 ) [ PHOTON_MAPPING (0/1) "
     << " [ NUM_THREADS( 1-32 ) ] ] ] ] ] ]" << endl;
    exit( EXIT_FAILURE );				// TERMINATE
} // usage

int main(int argc, char** argv)
{
  std::string filename = "scene.lua";
  int temp;
  BOUNDING = 0;
  AA_value = 0;
  MAX_RECURSIVE_LEVEL = 5;
  PHOTON_MAPPING = false;
  NUM_THREADS = 1;

  switch ( argc ) {
  case 7:
	  if ( !convert( temp, argv[6] ) || temp < 1 || temp > 32 ) usage (argv);
	  NUM_THREADS = temp;
	  // FALL THROUGH
  case 6:
	  if ( strcmp( argv[5], "1" ) == 0 ) {
		  PHOTON_MAPPING = true;
	  } else if ( strcmp( argv[5], "0" ) == 0 ) {
		  PHOTON_MAPPING = false;
	  } else {
		  usage(argv);
	  }
	  // FALL THROUGH
  case 5:
	  if ( !convert( temp, argv[4] ) || temp < 1 || temp > 10 ) usage (argv);
	  MAX_RECURSIVE_LEVEL = temp;
	  // FALL THROUGH
  case 4:
	  if ( strcmp( argv[3], "1" ) == 0 ) {
		  BOUNDING = true;
	  } else if ( strcmp( argv[3], "0" ) == 0 ) {
		  BOUNDING = false;
	  } else {
		  usage(argv);
	  }
	  // FALL THROUGH
  case 3:
	  if ( !convert( temp, argv[2] ) || temp < 0 || temp > 4) usage(argv);
	  AA_value = temp;
	  // FALL THROUGH
  case 2:
	  filename = argv[1];
	  break;
  case 1:
	  // Use "scene.lua" as file name
	  break;
  default:
	  usage(argv);
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}

