#include "./kMeans++/Cluster.hpp"

Cluster *cluster;
LSH *Lsh;

vector<string> ids;

int main(int argc, char *argv[]) {
    cluster->data = store_Cluster_data(argc, argv);
    cluster->kMeanspp_Initialization();
    if(cluster->get_method() == "Classic" || cluster->get_method() == "Lloyd" )
        cluster->Lloyd_method();
    else 
        cluster->reverse_assignment();

    cout << "cluster ok" << endl;
    if (cluster->get_silhouette()) cluster->Silhouette();
    cluster->output();
    cluster->print();
    if (Lsh) delete(Lsh);
    delete(cluster);
    return 0;
}