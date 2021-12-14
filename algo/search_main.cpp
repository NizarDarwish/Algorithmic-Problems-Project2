#include "./LSH_Folder/LSH.hpp"
#include "./Hypercube/hypercube.hpp"
#include "./LSH_Folder/Handling_input.hpp"

using namespace std;

LSH *Lsh = NULL;
Hypercube *Hpb = NULL;

int main(int argc, char *argv[]) {

    string algorithm = store_data(argc, argv);

    if (Lsh != NULL) read_file(Lsh->queries_data, Lsh->query_file);
    else read_file(Hpb->query_data, Hpb->query_file);

    if (Lsh != NULL) LSH_Insert_Points_To_Buckets(Lsh);

    // // Lsh->print_buckets();

    int queries;
    if (Lsh != NULL) queries = Lsh->queries_data.size();
    else queries = Hpb->query_data.size();

    vector<string> ids, q_ids;
    string input, output, query;
    if (Lsh != NULL) {
        input = Lsh->input_file;
        output = Lsh->output_file;
        query = Lsh->query_file;
    } else {
        input = Hpb->input_file;
        output = Hpb->output_file;
        query = Hpb->query_file;
    }
    read_ids(ids, input);
    read_ids(q_ids, query);

    ofstream Output;
    Output.open (output, ofstream::out | ofstream::trunc);

    vector<pair<long double, int>> ANN_result, NNB_result;

    double tApproximate = 0.0;
    double tTrue = 0.0;
    long double max_dis = -1;

    if (Lsh != NULL) {
        if (Lsh->get_metric() == "continuous") {
            algorithm = "LSH_Frechet_Continuous";
        } else if (Lsh->get_metric() == "discrete") {
            algorithm = "LSH_Frechet_Discrete";
        } else {
            algorithm = "LSH_Vector";
        }
    }

    for (int query = 0; query < queries; query++) {
        // Filter the query curve
        if (Lsh != NULL && Lsh->get_metric() == "continuous") {
            Lsh->queries_data[query] = Filter_Curve(Lsh->queries_data[query]);
        }

        Output << "Query: " << q_ids[query] << endl;
        Output << "Algorithm: " << algorithm << endl;
        auto begin = high_resolution_clock::now();
        if (Lsh != NULL)
            ANN_result = Nearest_N_search(Lsh->queries_data[query]);
        else
            Hpb->nNeighbor(Hpb->query_data[query], 1, ANN_result);
        auto end = high_resolution_clock::now();
        if (Lsh != NULL) Lsh->ANN_time = end - begin;
        else Hpb->ANN_time = end - begin;

        begin = high_resolution_clock::now();
        if (Lsh != NULL)
            NNB_result = Nearest_N_brute(Lsh->data, Lsh->queries_data[query], 1, Lsh->get_metric());
        else
            NNB_result = Nearest_N_brute(Hpb->get_data(), Hpb->query_data[query], 1, "");
        end = high_resolution_clock::now();
        if (Lsh != NULL) Lsh->NNB_time = end - begin;
        else Hpb->NNB_time = end - begin;

        Output << "Approximate Nearest neighbor: ";
        if (ANN_result.size() < 1) Output << "Not Found";
        else Output << ids[ANN_result[0].second];

        Output << endl;

        Output << "True Nearest neighbor: " << ids[NNB_result[0].second] << endl;
        Output << "distanceApproximate: ";
        if (ANN_result.size() < 1) Output << endl;
        else Output << ANN_result[0].first << endl;
        Output << "distanceTrue: " << NNB_result[0].first << endl << endl;

        if (ANN_result.size() != 0 && max_dis < ANN_result[0].first / NNB_result[0].first)
            max_dis = ANN_result[0].first / NNB_result[0].first;

        if (Lsh != NULL) {
            tApproximate += Lsh->ANN_time.count();
            tTrue += Lsh->NNB_time.count();
        } else {
            tApproximate += Hpb->ANN_time.count();
            tTrue += Hpb->NNB_time.count();
        }

        Output << endl;
    }

    Output << "tApproximateAverage: " << double(tApproximate/double(1000*queries)) << " seconds" << endl;
    Output << "tTrueAverage: " << double(tTrue/double(1000*queries)) << " seconds" << endl;
    Output << "MAF: " << max_dis << endl;
    
    Output.close();

    // Print_values();

    if (Lsh != NULL) delete Lsh;
    else delete Hpb;
}