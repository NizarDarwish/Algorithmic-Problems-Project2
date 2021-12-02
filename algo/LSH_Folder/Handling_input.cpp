#include "./LSH.hpp"
#include "../Hypercube/hypercube.hpp"

using namespace std;

extern LSH *Lsh; /* LSH Object */
extern Hypercube *Hpb; /* Hypercube Object */

void store_data(int argc,char** argv){
    string input_file = "", query_file = "", output_file = "", metric = "", algorithm;
    int N = 1, R = 10000, k = 4, L = 5;
    int probes = 2, m = 10;
    double delta;

    if(argc < 7 || argc > 21){
        cout << "Error in command line arguments:" << endl;
        cout << "argc " << argc << endl;
        cout << "Program exiting due to error ..." << endl;
        exit (EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            i++;
            input_file = string(argv[i]);
        }
        else if (!strcmp(argv[i], "-q")) {
            i++;
            query_file = string(argv[i]);
        }
        else if (!strcmp(argv[i], "-o")) {
            i++;
            output_file = string(argv[i]);
        }
        else if (!strcmp(argv[i], "-k")) {
            i++;
            k = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-L")) {
            i++;
            L = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-M")) {
            i++;
            m = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-probes")) {
            i++;
            probes = atoi(argv[i]);
        }
        else if (!strcmp(argv[i], "-delta")) {
            i++;
            delta = stod(argv[i]);
        }
        else if (!strcmp(argv[i], "-algorithm")) {
            i++;
            algorithm = argv[i];
        }
        else if (!strcmp(argv[i], "-metric")) {
            i++;
            metric = argv[i];
        }
    }

    if (input_file == "" || query_file == "" || output_file == "") {
        cout << "Error: Missing files directory" << endl;
        exit (EXIT_FAILURE);
    } else if (metric != "discrete" && metric != "continuous" && algorithm == "Frechet") {
        cout << "Error: give a metric" << endl;
        exit (EXIT_FAILURE);
    } else if (algorithm == "LSH" || algorithm == "Hypercube") metric = "";

    vector<vector<double>> vec;
    read_file(vec,input_file);

    if (algorithm == "LSH" || algorithm == "Frechet")
        Lsh = new LSH(input_file, query_file, output_file, L, N, k, num_of_points(), dim_data(), vec, delta, metric, algorithm);
    else if (algorithm == "Hypercube")
        Hpb = new Hypercube(input_file,query_file, output_file,k,1000,num_of_points(),N, dim_data(),probes,vec);
    else {
        cout << "give an algorithm" << endl;
        exit (EXIT_FAILURE);
    }
}