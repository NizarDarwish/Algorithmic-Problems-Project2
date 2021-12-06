#include "./LSH_Folder/LSH.hpp"
#include "./Hypercube/hypercube.hpp"

using namespace std;

LSH *Lsh;
Hypercube *Hpb;

int main(int argc, char *argv[]) {

    store_data(argc, argv);

    read_file(Lsh->queries_data, Lsh->query_file);

    LSH_Insert_Points_To_Buckets(Lsh);

    // // Lsh->print_buckets();

    int queries = Lsh->queries_data.size();
    int N = Lsh->get_N();

    ofstream Output;
    Output.open (Lsh->output_file, ofstream::out | ofstream::trunc);

    vector<pair<long double, int>> ANN_result;
    vector<double>                 NNB_result;
    vector<int>                    SBR_result;

    for (int query = 0; query < queries; query++) {
        // Filter the query curve
        if (Lsh->get_metric() == "continuous") {
            Lsh->queries_data[query] = Lsh->Filter_Curve(Lsh->queries_data[query]);
        }

        Output << "Query: " << query << endl;
        Output << "Algorithm: " << Lsh->get_algorithm() << endl;
        auto begin = high_resolution_clock::now();
        ANN_result = Nearest_N_search(Lsh->queries_data[query]);
        auto end = high_resolution_clock::now();
        Lsh->ANN_time = end - begin;

        begin = high_resolution_clock::now();
        NNB_result = Nearest_N_brute(Lsh->data, Lsh->queries_data[query], Lsh->get_N(), Lsh->get_metric());
        end = high_resolution_clock::now();
        Lsh->NNB_time = end - begin;

        // SBR_result = Search_by_range(Lsh->queries_data[query]);

        Output << "Approximate Nearest neighbor: ";
        if (ANN_result.size() < 1)
            Output << "Not Found" << endl << "distanceLSH: " << endl;
        else
            Output << ANN_result[0].second << endl
            << "distanceLSH: " << ANN_result[0].first << endl;
            Output << "distanceTrue: " << NNB_result[0] << endl << endl;

        Output << "tLSH: " << Lsh->ANN_time.count() << endl;
        Output << "tTrue: " << Lsh->NNB_time.count() << endl << endl;

    //     Output << "R-near neighbors:" << endl;
    //     for (auto point: SBR_result) {
    //         Output << point << endl;
    //     }
        Output << endl;
    }
    
    Output.close();

    // Print_values();

    delete(Lsh);
}