#include<iostream>
#include <cstdlib>
#include  <string>
#include <cmath>
#include <vector>
#include <limits>
using namespace std;

long double discreteFrechetDistance( vector<double> curve_query, vector<double> curve);
long double** create_DFD_Table( vector<double> curve_query, vector<double> curve);