#include "Cluster.hpp"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;

extern Cluster *cluster;
extern LSH *Lsh;

Cluster::Cluster(string input, string config, string out, bool comp, string method, string assign, bool sil)
                :input_file(input), config_file(config), output_file(out), complete(comp), Method(method),
                 assignment(assign), silhouette(sil) {

        read_config(config); // Read configuration file
        this->num_of_Items = num_of_points();
    }

Cluster::~Cluster() {
    // Deallocate all memory
    if (s.size() > 0) {
        auto it = s.begin();
        it++;
        s.erase(it, s.end());
    }

    if (Lloyd.size() > 0) {
        auto it_L = Lloyd.begin();
        it_L++;
        Lloyd.erase(it_L, Lloyd.end());
    }

    if (reverse_centroids.size() > 0) {
        auto it_R = reverse_centroids.begin();
        it_R++;
        reverse_centroids.erase(it_R, reverse_centroids.end());
    }

    if (data.size() > 0) {
        auto it_D = data.begin();
        it_D++;
        data.erase(it_D, data.end());
    }

    if (centroids.size() > 0) centroids.clear();
}

void Cluster::read_config(string config_file) {
    ifstream Config_File(config_file);

    if (Config_File.is_open()) {
        string Line;
        while(getline(Config_File, Line)) {
            stringstream s;
            s << Line;
            string name;
            s >> name;
            string var;
            s << Line;
            s >> var;
            name.pop_back();
            if (name == "number_of_clusters") this->number_of_clusters = stoi(var);
            else if (name == "number_of_vector_hash_tables") this->L = stoi(var);
            else if (name == "number_of_vector_hash_functions") this->k = stoi(var);
            else if (name == "max_number_M_hypercube") this->max_number_M_hypercube = stoi(var);
            else if (name == "number_of_hypercube_dimensions") this->number_of_hypercube_dimensions = stoi(var);
            else if (name == "number_of_probes") this->number_of_probes = stoi(var);
        }
        
    }
}

/*
* Initializes the centroids (centroids are part of the input data)
* The first centroid is uniformly picked
* The rest of the centroids are picked based on probability (Probability = distance/sum of distances)
* distance = the distance of the point relative to its closest centroid
*/
void Cluster::kMeanspp_Initialization() {
    int t = 1, input_items = this->num_of_Items;

    auto begin = high_resolution_clock::now();

    // Pick a random input index
    srand(time(0));
    random_device generator;
	uniform_int_distribution<int> dis(0, input_items);

	int random_index = dis(generator);

    this->centroids.push_back(random_index);

    while(t != this->number_of_clusters) {
        long double sum_min_dists = 0.0;
        vector<pair<int, long double>> prob;
        for (int point = 0; point < input_items; point++) {
            // If point is not a centroid
            if (none_of(this->centroids.begin(), this->centroids.end(), [point](int centroid) { return point == centroid; })) {
                long double dis = (long double) NUM;
                int potential_centroid = -1;
                // Find the closest centroid to the point
                for (auto centroid: this->centroids) {
                    long double point_dist;
                    point_dist = euclidean_dis(this->data[point], this->data[centroid]);

                    if (point_dist < dis) {
                        dis = point_dist;
                        potential_centroid = point;
                    }
                }

                sum_min_dists += dis;

                // Store the min distance and the point
                prob.push_back(make_pair(potential_centroid, dis));
            }
        }

        long double highest_prob = -1;
        int next_centroid = -1;
        // The point with the highest probability with be the next centroid
        // Probability = distance/sum of distances
        for (auto item: prob) {
            long double prob = item.second/sum_min_dists;
            if (prob > highest_prob) {
                highest_prob = prob;
                next_centroid = item.first;
            }
        }

        this->centroids.push_back(next_centroid);

        t++;

        // Delete vector
        auto it = prob.begin();
        it++;
        prob.erase(it, prob.end());
    }

    auto end = high_resolution_clock::now();
    duration<double, std::milli> time = end - begin;

    std::cout << "kMeans++ TIME: " << time.count() << endl;
}

/*
* Find new centroid witch is the mean of all point in the cluster
*/
vector<double> Cluster::Calculate_Mean(vector<int> near_points) {
    long int T = near_points.size();
    int size = this->data[0].size();
    vector<double> centroid(size, 0.0);
    for (auto point: near_points) {
        for(int i = 0; i < size; i++) {
            centroid[i] += this->data[point][i];
        }
    }

    for (int i = 0; i < size; i++) {
            centroid[i] = centroid[i] / (T > 0 ? T : 1);
    }

    return centroid;
}

// This function compares all new clusters to their previous
// if the clusters have not changed much then return false else return true
bool Cluster::Compare(vector<vector<double>> previous_clusters) {
    for (int centroid = 0; centroid < number_of_clusters; centroid++) {
        double sum_of_diff_points = 0.0;
        int size = previous_clusters[centroid].size();
        if (size == 0) return true;
        for (int point = 0; point < size; point++) {
            if (Lloyd[centroid].first[point] != previous_clusters[centroid][point]) {
                sum_of_diff_points++;
            }
        }

        // If 10% of the cluster has changed return true
        double percentage = sum_of_diff_points/(double) (size - 1);
        if (percentage >= 0.01) return true;
    }

    return false;
}

/*
* Assign each point to its closest centroid
* Update the centroids (Mean of cluster)
* Do this until the difference between the new centroids and the previous is less than 1% (function Compare)
*/
void Cluster::Lloyd_method() {

    // Store initial kMeans++ centroids
    vector<int> empty_vec;
    empty_vec.clear();
    for (auto centroid: centroids) {
        Lloyd.push_back(make_pair(this->data[centroid], empty_vec));
    }

    auto begin = high_resolution_clock::now();

    vector<double> empty;
    empty.clear();
    vector<vector<double>> previous_clusters(number_of_clusters, empty);
    // Do this until there is almost no difference to the centroids
    while (Compare(previous_clusters)) {
        // Assign each point to its centroid
        for (int centroid = 0; centroid < number_of_clusters; centroid++) {
            Lloyd[centroid].second.clear();
        }
        for (int point = 0; point < num_of_Items; point++) {
            long double dis = (long double) NUM;
            int point_centroid = -1;
            if (none_of(this->centroids.begin(), this->centroids.end(), [point](int centroid) { return point == centroid; })) {
                for (int centroid = 0; centroid < number_of_clusters; centroid++) {
                    long double point_dist = euclidean_dis(this->data[point], Lloyd[centroid].first);
                    if (point_dist < dis) {
                        dis = point_dist;
                        point_centroid = centroid;
                    }
                }

                Lloyd[point_centroid].second.push_back(point);
            }
        }

        // Store previous clusters before Updating them
        // Update centroids
        for (int centroid = 0; centroid < number_of_clusters; centroid++) {
            previous_clusters[centroid].clear();
            previous_clusters[centroid] = Lloyd[centroid].first;
            Lloyd[centroid].first = Calculate_Mean(Lloyd[centroid].second);
        }

        centroids.clear();
    }

    auto end = high_resolution_clock::now();
    Cluster_time = end - begin;

    auto it = previous_clusters.begin();
    it++;
    previous_clusters.erase(it, previous_clusters.end());
}

void Cluster::Silhouette() {
    vector<long double> sil(number_of_clusters, 0);
    auto begin = high_resolution_clock::now();
    // The first vector is the centroid and the second is a vector of indexes
    vector<pair<vector<double>, vector<int>>> temp;
    if(cluster->get_method() == "Classic" || cluster->get_method() == "Lloyd" ){
        temp = this->Lloyd;
    }else{
        temp = reverse_centroids;
    }

    unordered_map<string, long double> dists;

    for (int cluster = 0; cluster < number_of_clusters; cluster++) {

        int size_of_cluster = temp[cluster].second.size();
        vector<int> Points = temp[cluster].second;
        for (auto i: Points) {
            // Calculate average distance
            long double a = 0.0;
            for (auto j: Points) {
                if (i != j) {
                    unordered_map<string, long double>::iterator it = dists.find(to_string(i)+to_string(j));
                    if (it != dists.end()) {
                        a += it->second;
                    }
                    else {
                        unordered_map<string, long double>::iterator it = dists.find(to_string(j)+to_string(i));
                        if (it != dists.end()) {
                            a += it->second;
                        } else {
                            long double euc_dis = euclidean_dis(this->data[i], this->data[j]);
                            a += euc_dis;
                            dists[to_string(i)+to_string(j)] = euc_dis;
                        }
                    }
                }
            }

            a /= size_of_cluster-1 > 0 ? (size_of_cluster-1) : 1; // average distance of i to objects in same cluster

            // Find Second closest centroid
            long double dist = NUM;
            int sec_cluster = -1;
            for (int second_cluster = 0; second_cluster < number_of_clusters; second_cluster++) {
                if (cluster != second_cluster) {
                    long double point_dist = euclidean_dis(this->data[i], temp[second_cluster].first);
                    if (dist > point_dist) {
                        sec_cluster = second_cluster;
                        dist = point_dist;
                    }
                }
            }

            // Calculate average distance of second cluster
            long double b = 0.0;
            for (auto j: temp[sec_cluster].second) {
                if (i != j) {
                    unordered_map<string, long double>::iterator it = dists.find(to_string(i)+to_string(j));
                    if (it != dists.end()) {
                        b += it->second;
                    }
                    else {
                        unordered_map<string, long double>::iterator it = dists.find(to_string(j)+to_string(i));
                        if (it != dists.end()) {
                            b += it->second;
                        } else {
                            long double euc_dis = euclidean_dis(this->data[i], this->data[j]);
                            b += euc_dis;
                            dists[to_string(i)+to_string(j)] = euc_dis;
                        }
                    }
                }
            }

            b /= (temp[sec_cluster].second.size()-1) > 0 ? (temp[sec_cluster].second.size()-1) : 1;

            // Calculate Silhouette
            sil[cluster] += (b - a) / (b > a ? b : a);
            // cout << a << " " << b << " " << sil[cluster] << endl;
        }

        sil[cluster] /= size_of_cluster;
    }

    this->s = sil;

    auto end = high_resolution_clock::now();
    duration<double, std::milli> time = end - begin;
    
    std::cout << "Silhouette TIME: " << time.count() << endl;

    // Deallocate Memory
    auto it = dists.begin();
    it++;
    dists.erase(it, dists.end());

    auto it_v = temp.begin();
    it_v++;
    temp.erase(it_v, temp.end());

    auto it_s = sil.begin();
    it_s++;
    sil.erase(it_s, sil.end());
}


// compute how many unassigned vectors we've got, for the reverse assignement method
int Cluster::unassigned_count(){
	int count = 0;
	for(int i = 0; i < num_of_Items; i++){
		if (assigned_centroid.at(i) == -1) 
			count++;
	}
	return count;
}


//compute the nearest center for a given vector

int Cluster::nearest_centroid(vector<double> vec) {
	long int min_distance =  4294967291;
	int nearest_centroid = -1;
	// compute the distances to all the centroids
	for (int i = 0; i < number_of_clusters; i++) {
		long int temp_distance = euclidean_dis(vec, this->reverse_centroids[i].first);

		// set it as min
		if (temp_distance < min_distance) {
			min_distance = temp_distance;
			nearest_centroid = i;
		}
	}
	assert(nearest_centroid != -1);

	return nearest_centroid;
};

// Compute the minimum of the distances of the centroids. Needed for the initialization of the radius in reverse assignment
long int  Cluster::min_distance_between_centroids(){

	// initialize the minimum distance
	long int min_distance = 4294967291;

	// brute force all the distances in order to find the smallest

	for(int i = 0; i < number_of_clusters; i++){
		for(int j = 0; j < number_of_clusters; j++){
			if (i != j) {
				long int temp_distance = euclidean_dis(this->reverse_centroids[i].first, this->reverse_centroids[j].first);
				if (temp_distance < min_distance)
					min_distance = temp_distance;
			}
		}
	} 
	return min_distance;
}


// This function computes the distance between the new and the previous centroids
// if the distance is less than 0.1 to at least half the centroids then return false else return true
bool Cluster::Compare1(vector<pair<vector<double>, vector<int>>> previous_clusters) {
    int sum_of_diff_centroids = 0;
    for (int centroid = 0; centroid < number_of_clusters; centroid++) {
        int size = previous_clusters[centroid].first.size();
        if (size == 0) return true;
        long double dist = euclidean_dis(previous_clusters[centroid].first, reverse_centroids[centroid].first);
        if (dist < 0.1)
            sum_of_diff_centroids++;
    }

    if (sum_of_diff_centroids >= number_of_clusters/2) return false;
    else return true;
}

// Check if the clusters changed
bool Cluster::Check(vector<pair<vector<double>, vector<int>>> previous_clusters) {
    int sum_of_diff_points = 0;
    for (int centroid = 0; centroid < number_of_clusters; centroid++) {
        if (previous_clusters[centroid].second.size() != reverse_centroids[centroid].second.size()) {
            return true;
        }

        if (reverse_centroids[centroid].second.size() == 0) sum_of_diff_points++;
    }
    
    if (sum_of_diff_points == number_of_clusters) return true;

    return false;
}

int Cluster::reverse_assignment(void) {

    auto begin = high_resolution_clock::now();
    
    this->reverse_centroids.reserve(number_of_clusters);

    string method, metric = "";
    if (Method == "LSH_Frechet") {
        method = "Frechet";
        metric = "discrete";
    }
    else if (Method == "LSH") {
        method = "Mean_Vector";
    }
      
    if(Method=="LSH"||Method=="LSH_Frechet"){
        //we dont care about query file and  N here
        Lsh = new LSH(input_file, "", output_file, this->L, 1, this->k, this->num_of_Items, dim_data(), this->data, 0.0, metric, method);
        LSH_Insert_Points_To_Buckets(Lsh);
    }else if(Method=="Hypercube"){
        hypercube_ptr = new Hypercube(input_file, "", output_file, this->number_of_hypercube_dimensions, this->max_number_M_hypercube,this->num_of_Items,5,dim_data() , this->number_of_probes, this->data);
    }

    vector<int> empty_vec;
    empty_vec.clear();
    for (auto centroid: centroids) {
        reverse_centroids.push_back(make_pair(this->data[centroid], empty_vec));
    }
    centroids.clear();

	// initial radius
	long int radius = min_distance_between_centroids()/2;

    // keep a vector of the new assignments
	vector<int> assigned_new(num_of_Items, -1);

    assigned_centroid = assigned_new;

	// keep track of the changes
	int changes = 0;

	// set a threshold in order to break the loop
	int unassigned_prev = 4294967291;

	// keep track of unassinged points
	int unassinged = 4294967291 - 1;

    vector<double> empty;
    empty.clear();
    vector<pair<vector<double>, vector<int>>> previous_clusters(number_of_clusters, {empty, empty_vec});

	// break the loop when all the balls contain no new vectors
	while(Compare1(previous_clusters)) {

        radius = min_distance_between_centroids()/2;
        for(int centroid = 0; centroid < number_of_clusters; centroid++) this->reverse_centroids[centroid].second.clear();

        // do a range search query for every centroid
        assigned_centroid.clear();
        assigned_centroid = assigned_new;

        while(Check(previous_clusters)) {
            for(int i = 0; i < number_of_clusters; i++) {
                previous_clusters[i].second = reverse_centroids[i].second;
                // this->reverse_centroids[i].second.clear();
                vector<pair<long double, int>> near_items;
                list<int> neighbors;
                list<double> neighbors_dists;
                // the type of range search depends on what the user wants
                if (Method == "LSH"){
                    near_items = Lsh->Search_by_range2(this->reverse_centroids[i].first,radius);
                }
                else if (Method == "Hypercube"){
                    //near items have format (long double,int) -> distance, position in data vectors
                    hypercube_ptr->RNeighbors(this->reverse_centroids[i].first, radius,neighbors,neighbors_dists);

                    auto itA = neighbors_dists.begin();
                    auto itB = neighbors.begin();	
                    while(itA != neighbors_dists.end() || itB != neighbors.end()){
                        near_items.push_back(make_pair(*itA,*itB));
                        ++itA;
                        ++itB;
                    }
                    neighbors.clear();
                    neighbors_dists.clear();
                }else if (Method == "LSH_Frechet"){
                    near_items = Lsh->Search_by_range2(this->reverse_centroids[i].first,radius);
                }

                for (auto iter = near_items.begin(); iter != near_items.end(); iter++){

                    // get the current vector
                    int current_vector = iter->second;
                    if(assigned_centroid.at(current_vector) != -1) {
                        // check if its distance from the current centroid, is less than the previous' one
                        int assigned_prev = assigned_centroid.at(current_vector); 
                        int prev_distance = euclidean_dis(data.at(current_vector), this->reverse_centroids[assigned_prev].first);
                        int new_distance = iter->first;

                        // if it is, it is closest to the current centroid, thus change the asssigned value in the temp vector
                        if (new_distance < prev_distance){
                            this->reverse_centroids[assigned_prev].second.erase(remove(this->reverse_centroids[assigned_prev].second.begin(), this->reverse_centroids[assigned_prev].second.end(), current_vector), this->reverse_centroids[assigned_prev].second.end());
                            this->reverse_centroids[i].second.push_back(current_vector); 
                            assigned_centroid.at(current_vector) = i;
                        }
                    }// if it has been already assigned
                    else if(assigned_centroid.at(current_vector) == -1){
                        this->reverse_centroids[i].second.push_back(current_vector); 
                        // temporarly assign it to this centroid
                        assigned_centroid.at(current_vector) = i;
                    }
                }
            }
            cout << "ok6" << endl;
            // update the unassigned vectors count
            unassigned_prev = unassinged;
            unassinged = unassigned_count();

            // update the radius
            radius *= 2;
        }

        // Assign the rest of the points to their closest centroid
        for (int i = 0; i < num_of_Items; i++) {
            
            // for each one not tracked, use direct assignment
            if (assigned_centroid.at(i) == -1){
                assigned_centroid.at(i) = nearest_centroid(data.at(i));
                reverse_centroids[assigned_centroid.at(i)].second.push_back(i);
            }
        }

        cout << "ok5" << endl;
        if (this->assignment == "Mean_Vector") {
            //Update centroids
            for (int centroid = 0; centroid < number_of_clusters; centroid++) {
                previous_clusters[centroid].first = reverse_centroids[centroid].first;
                if(this->reverse_centroids[centroid].second.size() != 0)
                    reverse_centroids[centroid].first = Calculate_Mean(reverse_centroids[centroid].second);
            }
        }else if (this->assignment == "Mean_Frechet") {
            cout << "ok4" << endl;
            for (int centroid = 0; centroid < number_of_clusters; centroid++) {
                previous_clusters[centroid].first = reverse_centroids[centroid].first;
                if(this->reverse_centroids[centroid].second.size() != 0){
                    reverse_centroids[centroid].first = create_mean_curve_tree(this->reverse_centroids[centroid].second, this->reverse_centroids[centroid].first);
                }
            }
        }
        cout << "ok1" << endl;
	}
    
	auto end = high_resolution_clock::now();
    Cluster_time = end - begin;
};

int Cluster::min3_index(double a, double b, double c){
	int index;
	double curr_min;
	if (a <= b) {
		index = 0;
		curr_min = a;
	} else {
		index = 1;
		curr_min = b;
	}
	if (c <= curr_min) {
		index = 2;
		curr_min = c;
	}
	return index;
}

vector<pair<double,double>> Cluster::optimal_Traversal_Computation(vector<double> P, vector<double> Q) {
    cout << "ok3" << endl;
    long double** L = create_DFD_Table(P, Q);
    //long double **L = this->array_C;

    int i = P.size();
    int j = Q.size();

    vector<pair<double,double>> traversal;

    // put at the end of the vector, so we will read it in reverse afterwards
    traversal.push_back(make_pair(P[j],Q[i]));

    while (i != 0 && j != 0) {
        int min_index = min3_index(L[i-1][j], L[i][j-1], L[i-1][j-1]);
        if (min_index == 0)
            i--;
        else if (min_index == 1)
            j--;
        else if (min_index == 2) {
            i--;
            j--;
        }
        traversal.push_back(make_pair(P[i], Q[j]));
    }

    // Corner cases
    if (i != 0 && j == 0)
        while (i != 0) {
            i--;
            traversal.push_back(make_pair(P[i], Q[j]));
        }
    if (i == 0 && j != 0)
        while (j != 0) {
            j--;
            traversal.push_back(make_pair(P[i], Q[j]));
        }

    for (int n = 0; n < P.size(); n++)
        delete[] L[n];
    delete[] L;

    return traversal;
}

vector<double> Cluster::mean_Discrete_Frechet_Curve(vector<double> P, vector<double> Q) {
	vector<pair<double,double>> traversal = optimal_Traversal_Computation(P, Q);
	int points_num = traversal.size();
	vector<double> mean_curve;


	// start from end (see comment in optimal_Traversal_Computation)
	for (int i = 0 ; i <points_num; i++) {
		pair<double,double> pair_var = traversal[i];
		mean_curve.push_back((pair_var.first + pair_var.second) / 2);
		//mean_points[points_num-1-i].print();
	}

    traversal.clear();
    cout << "ok2" << endl;
	return mean_curve;
}

void Cluster::postOrderPrint(TreeNode* node) {
	if (node->left!= NULL) {
	    std::cout << "LEFT" << endl;
		postOrderPrint(node->left);
	}
	if (node->right!=NULL) {
		std::cout << "RIGHT" << endl;
		postOrderPrint(node->right);
	}
	std::cout << "ROOT" << endl;
	//node->curve->CurvePrint();
}

vector<double> Cluster::create_mean_curve_tree(vector<int> cluster, vector<double> center) {

    //if cluster is empty
    if (cluster.size() == 0 || cluster.size() == 1)
        return center;

    vector<TreeNode*> curr_node;

    // Start with original curves as leaves
    for (int i = 0 ; i < cluster.size() ; i++){
        curr_node.push_back(new TreeNode( &this->data[cluster[i]] ));
    }

    vector<TreeNode*> nextNodes;

    while(curr_node.size()>1){
    // Take curves in pairs
        //step 2 because we have binary tree
        for (int i = 0 ; i < curr_node.size() - 1; i+=2) {
            vector<double> P = *(curr_node[i]->curve_id);
            vector<double> Q = *(curr_node[i+1]->curve_id);
            vector<double> mean_curve = mean_Discrete_Frechet_Curve(P, Q);
            TreeNode* mean_node = new TreeNode(&mean_curve, curr_node[i], curr_node[i+1]);
            nextNodes.push_back(mean_node);
        }

        // if size is odd, there is an extra node at the end
        if (curr_node.size() % 2 == 1)
            nextNodes.push_back(curr_node[curr_node.size()-1]);

        curr_node = nextNodes;
        nextNodes.clear();

    }

    //postOrderPrint(curr_node[0]);
    //curr_node[0]->curve->CurvePrint();

    //update centroid - return root total node
    return *(curr_node[0]->curve_id);
}

void Cluster::print() {
    cout << "number_of_clusters: " << number_of_clusters << endl;
    cout << "number_of_vector_hash_tables: " << L << endl;
    cout << "number_of_vector_hash_functions: " << k << endl;
    cout << "max_number_M_hypercube: " << max_number_M_hypercube << endl;
    cout << "number_of_hypercube_dimensions: " << number_of_hypercube_dimensions << endl;
    cout << "number_of_probes: " << number_of_probes << endl;
}

void Cluster::output() {
    vector<pair<vector<double>, vector<int>>> temp;
    if(cluster->get_method() == "Classic" || cluster->get_method() == "Lloyd" ){
        temp = this->Lloyd;
    }else{
        temp = reverse_centroids;
    }
    
    ofstream Output;
    Output.open (this->output_file, ofstream::out | ofstream::trunc);
    Output << "Algorithm: ";
    if (this->Method == "Classic" || this->Method == "Lloyd") Output << "Lloyds";
    else if (this->Method == "LSH") Output << "Range Search LSH";
    else if (this->Method == "Hypercube") Output << "Range Search Hypercube";
    Output << endl;

    int counter = 1;
    for (auto centroid: temp) {
        bool first = true;
        Output << "CLUSTER-" << counter << " {size: " << centroid.second.size() << ", centroid: ";
        for (auto points: centroid.first) {
            if (first) {
                first = false;
                continue;
            }
            Output << points << ", ";
        }

        if (this->complete) {
            Output << "Items: ";
            for (auto points: centroid.second) {
                Output << points << ", ";
            }
        }
        Output << "}" << endl;
        counter++;
    }

    Output << "cluster_time: " << this->Cluster_time.count() << endl;
    Output << "Silhouette: [";

    long double stotal = 0.0;
    for (auto cluster: this->s) {
        Output << cluster << ",";
        stotal += cluster;
    }

    Output << stotal/this->number_of_clusters << "]" << endl;

    Output.close();
}
