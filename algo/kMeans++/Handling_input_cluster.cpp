#include "./Cluster.hpp"

using namespace std;

extern Cluster *cluster;

vector<vector<double>> store_Cluster_data(int argc,char** argv){
    string input_file = "", configuration_file = "", output_file = "";
    int N = 1, R = 10000, k = 4, L = 5;
    string Method = "", update = "";
    bool complete = false;
    bool silhouette = false;

    if(argc < 9 || argc > 13){
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
        else if (!strcmp(argv[i], "-c")) {
            i++;
            configuration_file = string(argv[i]);
        }
        else if (!strcmp(argv[i], "-o")) {
            i++;
            output_file = string(argv[i]);
        }
        else if(strcmp(argv[i], "-complete")==0){
            complete = true;
        }
        else if(strcmp(argv[i], "-assignment")==0){
            i++;
            Method = string(argv[i]);
        }
        else if(strcmp(argv[i], "-update")==0){
            i++;
            update = string(argv[i]);
        }
        else if(strcmp(argv[i], "-silhouette")==0){
            silhouette = true;
        }
    }

    if (input_file == "" || configuration_file == "" || output_file == "" || Method == "" || update == "") {
        cout << "Error: Missing files directory" << endl;
        exit (EXIT_FAILURE);
    } else if ((Method == "LSH_Frechet" && update == "Mean_Vector") ||
                ((Method == "LSH" || Method == "Hypercube") && update == "Mean_Frechet")) {
        cout << "Assignment with wrong update" << endl;
        exit (EXIT_FAILURE);
    }
        
    vector<vector<double>> vec;
    read_file(vec,input_file);

    cluster = new Cluster(input_file, configuration_file, output_file, complete, Method, update, silhouette);

    return vec;
}