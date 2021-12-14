#include<iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <random>
#include <limits>
#include <algorithm>
#include <chrono>
using namespace std;

long double discreteFrechetDistance( vector<double> curve_query, vector<double> curve);
long double** create_DFD_Table( vector<double> curve_query, vector<double> curve);

vector<double> create_mean_curve_tree(vector<int> cluster, vector<double> center, vector<vector<double>> data, int dim);
vector<double> mean_Discrete_Frechet_Curve(vector<double> P, vector<double> Q);
vector<pair<double,double>> optimal_Traversal_Computation(vector<double> P, vector<double> Q);
int min3_index(double a, double b, double c);