#include <iostream>

#include "functions.hpp"

using namespace std;

string dir_input = "";

// The dimension of the Data is the #integers an item has
long int dim_data() {
    long int dim = 0;

    string line;

    ifstream Data_File(dir_input); /* Data File */

    while(getline(Data_File,line)){
        stringstream s;
        s << line;

        string first_item;
        s >> first_item;
        double d;

        while(s >> d){
            dim++;
        }

        s.str("");
        break;
    }

	Data_File.clear();
	Data_File.seekg(0,ios::beg); // Reseting the pointer
    Data_File.close();

	return dim;
}

long int num_of_points() {

	long int num_of_lines = 0;

	string str;

    ifstream Data_File(dir_input); /* Data File */

	while(getline(Data_File, str)) ++num_of_lines;

	Data_File.clear();
	Data_File.seekg(0,ios::beg); // Reseting the pointer
    Data_File.close();

	return num_of_lines;
}

double read_file(vector<vector<double>> &vec, string input_file){
    string line;
    if (dir_input == "") dir_input = input_file;
    ifstream Data_File(input_file);
    double max = -1;
    
    if (Data_File.is_open()){
        while(getline(Data_File,line)){
            stringstream s;
            s << line;                   //send the line to the stringstream object

            string first_item;
            s >> first_item;
            double d;
            vector<double> v1;

            while(s >> d){
                double num = static_cast<double>(d);
                if (num > max) max = num;
                v1.push_back(num);
            } 
            vec.push_back(v1);

            v1.clear();
            s.str("");
        }
        
    } else {
        cout << "Unable to open file";
        exit(EXIT_FAILURE);
    }
    Data_File.close();

    return max;
}

double Normal_distribution() {
    // Inspired from https://en.cppreference.com/w/cpp/numeric/random/normal_distribution
    random_device rd{};
    mt19937 gen{rd()};
 
    normal_distribution<float> d{0, 1};
 
    map<int, int> hist{};
    return round(d(gen));
}

// Calculate Euclidean Distance
long double euclidean_dis(vector<double> vec1, vector<double> vec2) {
    long double dist=0.0;

    auto itA = vec1.begin();
    auto itB = vec2.begin();
    // ++itA;
    // ++itB;

    while(itA != vec1.end() || itB != vec2.end())
    {
        dist = dist + (itA[0]-itB[0])*(itA[0]-itB[0]);
        if(itA != vec1.end()) {
            ++itA;
        }
        if(itB != vec2.end()) {
            ++itB;
        }
    }

	return sqrt(dist);
}

vector <pair<long double, int>> Nearest_N_brute(vector<vector<double>> data, vector<double> query, int N, string Metric) {
    long double d = (double) BIG; // Minimum distance

    vector <pair<long double, int>> near_items;

    int iter = 0;
    for (auto Item: data) {
        long double euc_dist;
        if (Metric == "discrete")
            euc_dist = discreteFrechetDistance(Item, query);
        else if (Metric == "continuous")
            euc_dist = Continuous_Frechet(Item, query);
        else
            euc_dist = euclidean_dis(Item, query);

        if (euc_dist < d) {
            if (near_items.size() >= N) {
                near_items.pop_back();
            }
            near_items.push_back(make_pair(euc_dist, iter));
            sort(near_items.begin(), near_items.end());
            d = near_items.back().first;
        }
        iter++;
    }

    return near_items;
}

long double Continuous_Frechet(vector<double> Item, vector<double> query) {
    Curve Item_curve(1);
    for (int point = 0; point < Item.size(); point++) {
        Point Item_point(1);
        Item_point.set(0, Item[point]);
        Item_curve.push_back(Item_point);
    }

    Curve query_curve(1);
    for (int point = 0; point < query.size(); point++) {
        Point query_point(1);
        query_point.set(0, query[point]);
        query_curve.push_back(query_point);
    }

    // continuous frechet distance
    long double dist = Frechet::Continuous::distance(Item_curve, query_curve).value;

    // delete(Item_curve);
    // delete(query_curve);

    return dist;
}

void read_ids(vector<string> &ids, string input_file){ 
    string line;
    ifstream Data_File(input_file);

    if (Data_File.is_open()){
        while(getline(Data_File,line)){
            stringstream s;
            s << line;

            string first_item;
            s >> first_item;
            ids.push_back(first_item);
            s.str("");
        }
        
    } else {
        cout << "Unable to open file";
        exit(EXIT_FAILURE);
    }
    Data_File.close();
}

vector<double> Filter_Curve(vector<double> item) {
    // Calculate the Mean of steps
    double sum = 0.0;
    for (auto it = item.begin(); it != item.end(); ++it) {
        if ((it + 1) == item.end()) break;
        sum += abs(it[0] - (it + 1)[0]);
    }

    sum /= item.size();

    // Filter out some values
    for (auto it = item.begin(); it != item.end() && (it + 1) != item.end() && (it + 2) != item.end(); ++it) {
        if (abs(it[0] - (it + 1)[0]) < sum && abs((it + 1)[0] - (it + 2)[0]) < sum) {
            item.erase((it + 1));
        }
    }

    return item;
}