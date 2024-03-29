#ifndef LLOYD_H
#define LLOYD_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <cmath>
#include <map>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <time.h>
#include <algorithm>
#include <chrono>

#include "./Handling_input_cluster.hpp"
#include "../LSH_Folder/LSH.hpp"
#include "../Hypercube/hypercube.hpp"
#define NUM 1.79769e+308 // Biggest double num that can be defined

using std::chrono::high_resolution_clock;
using std::chrono::duration;

class Cluster {
    private:
        bool complete = false;
        bool silhouette;
        double max_value; // For padding
        string Method;
        string assignment;
        int L=3; // num of hash tables for LSH
        int k=4; // k of LSH
        int num_of_Items;
        int number_of_clusters;
        int max_number_M_hypercube = 10;
        int number_of_hypercube_dimensions = 3;
        int number_of_probes = 2;
        string input_file, config_file, output_file;

        duration<double> Cluster_time;
        // The centroids kMeans++ initialized
        vector<int> centroids;
        // Silhouette results
        vector<long double> s;
        // The first vector is the centroid and the second is a vector of indexes
        vector<pair<vector<double>, vector<int>>> Lloyd;

		// vector that holds the index of the centroid that the current index's vector is assigned to
		vector<int> assigned_centroid;

        // The first vector is the centroid and the second is a vector of indexes
        vector<pair<vector<double>, vector<int>>> reverse_centroids;

		// in case of hypercube, we need these extra variables
		Hypercube* hypercube_ptr;

    public:
        vector<vector<double>> data; // Input data

        Cluster(string, string, string, bool, string, string, bool, double);
        ~Cluster();

        void kMeanspp_Initialization(); /* Initializes the centroids */
        void Lloyd_method();
        vector<double> Calculate_Mean(vector<int>);
        bool Compare(vector<vector<int>>);
        bool Compare1(vector<pair<vector<double>, vector<int>>>);
        bool Check(vector<pair<vector<double>, vector<int>>>);
        void Silhouette();
        string get_method(void){return Method;}
        bool get_silhouette() { return silhouette; }
        void read_config(string );

        void output();
        void print();

        int unassigned_count();
        int nearest_centroid(vector<double> vec);
        long int  min_distance_between_centroids(void);
        int reverse_assignment(void);
};

#endif