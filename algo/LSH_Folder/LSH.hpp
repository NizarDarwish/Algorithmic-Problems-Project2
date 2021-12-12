#ifndef LSH_H
#define LSH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <cmath>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <chrono>

#define M 4294967291

#include "../functions/functions.hpp"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

class Bucket
{
    public:
    // For each input item we store its Item_id, lsh id (after first mod) and the g value (position in hashtable)
    // pair((Item_id, hash ID), hash value)
        vector<pair<pair<long long int, long long int>, long int>> points;     

        void add(long long int, long int, long long int);

};

class Euclidean_Hash_Function {
    private:
        vector<vector <double>> v; /* Vectors with random normal numbers, used for hashfunction */
        vector<double> t; // Vector with random numbers between 0 and w picked uniformly  
    public:
        Euclidean_Hash_Function() {};
        Euclidean_Hash_Function(int k, int dim); /* Here we initialize all vectors */
        ~Euclidean_Hash_Function();
        vector<vector <double>> get_vector_v() { return v; }
        vector<double> get_vector_t() { return t; }
};

class LSH {
    private:
        int k; /* #LSH Functions */
        int L; /* #Hash Tables  */
        // #Points
        long long int points_num;

        // #Dimensions of the point
        // #Numbers in a vector of an item
        int dimension;
        int w;
        vector<int> r;

        double delta;
        double max_value; // For padding
        // Vector t for Gt (for shifting)
        vector<double> shift;
        string metric;

        int R,N;
        long int hashtable_size;
        Euclidean_Hash_Function *Hash_Funs;
        Bucket*** hashtables;
    public:
        string input_file, query_file, output_file;
        vector<vector<double>> data; // Input Data
        vector<vector<double>> queries_data; // Query Data
        duration<double, std::milli> ANN_time;
        duration<double, std::milli> NNB_time;

        LSH(string, string, string, int L_, int N_, int k_, long long int n, int dim, vector<vector<double>>, double, string, double);
        ~LSH();
        int get_pointsnum() { return points_num; }
        long int get_hashtablesize() { return hashtable_size; }
        int get_w() { return w; }
        int get_L() { return L; }
        int get_k() { return k; }
        int get_N() { return N; }
        int get_R() { return R; }
        string get_metric() { return metric; }
        int get_dimension() { return dimension; }
        Bucket*** get_hashtables(){ return hashtables; }
        Euclidean_Hash_Function* get_hash_functions() { return Hash_Funs; }
        void set_w(int value) { w = value; }
        vector<std::pair<long double,int>> Search_by_range3(vector<double> query,long int R_custom);

        int Calculate_w();
        vector<long long int> Specific_Hash_Value(int g, vector<double> item);

        void print_buckets(); /* Used for Debugging */

        vector<pair<long double,int>> Search_by_range2(vector<double> query,long int R_custom);

        vector<double> Grid(int, vector<double> );

        vector<double> Filter_Curve(vector<double> );
};

void LSH_Insert_Points_To_Buckets(LSH* info); /* Initialize Data (Input) */

long long int mod(long long int, long int); /* This mod can handle negative values */

void Print_values(); /* Used for Debugging */

/* Finds the N-Nearest Items to the query using LSH hash function */
vector<pair<long double, int>> Nearest_N_search(vector<double> );

/* Finds the Items that are within a range relative to the query using LSH hash function */
vector<int> Search_by_range(vector<double> );

#endif