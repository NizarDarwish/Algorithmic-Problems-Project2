#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <random>
#include <iostream>
#include <sstream>
#include <string.h>

#define BIG 4294967291

using namespace std;

#include "../curves/discretef.hpp"

long int dim_data(); /* Get the dimensions of the data */
long int num_of_points(); /* Get the number of the items */
void read_file(vector<vector<double>>&, string);

double Normal_distribution(); /* Generates a sequence of random normal numbers */

vector <double> Nearest_N_brute(vector<vector<double>>, vector<double> , int, string);

long double euclidean_dis(vector<double> , vector<double> ); /* Calculate Euclidean Distance */