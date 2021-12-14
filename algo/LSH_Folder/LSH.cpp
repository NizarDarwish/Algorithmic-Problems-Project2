#include "LSH.hpp"

using namespace std;

extern LSH *Lsh; /* LSH Object */

int W;

/* Initialize the variables used by the hash function */
LSH::LSH(string input, string query, string output, int L_,int N_,int k_, long long int n, int dim, vector<vector<double>> Data, double d, string method, double max)
        :input_file(input), query_file(query), output_file(output), L(L_), N(N_), k(k_), points_num(n), dimension(dim), delta(d), metric(method), max_value(max)
    {
        data = Data;
        W = Calculate_w();

        // Number of buckets per hash table
        hashtable_size = n/4;
        Hash_Funs = new Euclidean_Hash_Function[L];
         //Declaration of hash tables...
        hashtables = new Bucket**[L];

        // Initialize vector for shifting
        random_device generator;
	    uniform_real_distribution<double> dis(0.0, (double) d);

        for(int i=0;i<L;i++) {
            Hash_Funs[i] = Euclidean_Hash_Function(k, dim);
            hashtables[i] = new Bucket*[hashtable_size];
            for(int j=0;j<hashtable_size;j++)   hashtables[i][j] = NULL;


            double random = dis(generator);
            shift.push_back(random);
        }

        // Every hash function uses a vector r length k

        int j=0;

        do {
            int r_value = rand();
            r.push_back(r_value);
            j++;
        }while(j < k);
    }

LSH::~LSH() {
    // Deallocate Data
    if (data.size() > 0) {
        auto it_D = data.begin();
        it_D++;
        data.erase(it_D, data.end());
    }

    if (hashtables) {
        for(int i = 0; i < L; i++) {
            for (int j = 0; j < hashtable_size; j++)
                delete hashtables[i][j];

            delete [] hashtables[i];
        }
        delete [] hashtables;
    }

    if (Hash_Funs) delete [] Hash_Funs;
}

void LSH::print_buckets() {
    for(int j=0; j < this->L; j++) {
        for(long int i=0; i < this->hashtable_size - 1100; i++){
            int counter = 0;
            if (hashtables[j][i] != NULL) {
                cout << "Table " << j << " in Bucket " << i << endl;
                for (auto point: hashtables[j][i]->points) {
                    if (counter == 5) break;
                    cout << "Item id: " << point.first.first << endl;
                    cout << "Hash id: " << point.first.second << endl;
                    cout << "Hash value: " << point.second << endl;
                    counter++;
                }
                cout << endl;
            }
        }
    }
}


/* Mod function that handles negative values */
long long int mod(long long int value, long int Mod) {
    if ((value % Mod) < 0) 
        return (unsigned int) (value % Mod + Mod);
    else
        return (unsigned int) (value % Mod);
}

vector<double> LSH::Grid(int hashtable, vector<double> item) {
    // xi' = floor((x-t)/δ + 1/2)*δ + t
    // pi = floor((item[i] - t)/delta + 1/2) * delta

    double padding_value = this->max_value * 2;

    vector<double> P;
    if (this->get_metric() == "discrete") {
        // Calculate G vector
        // Snap
        vector<pair<double, double>> p;
        for (int dim = 0; dim < this->get_dimension(); dim++) {
            double value = floor((item[dim] - shift[hashtable])/this->delta + 1/2) * this->delta;
            double time = floor((dim - shift[hashtable])/this->delta + 1/2) * this->delta;
            p.push_back(make_pair(time + shift[hashtable], value + shift[hashtable]));
        }
        for (auto it = p.begin(); it != p.end(); ++it) {
            if ((it + 1) == p.end()) break;
            if (it->first == (it + 1)->first && it->second == (it + 1)->second) {
                p.erase(it);
            }
        }

        // Unpack pairs
        for (auto it = p.begin(); it != p.end(); ++it) {
            P.push_back(it->first);
            P.push_back(it->second);
        }

        // Padding to 2d
        for (int i = P.size(); i < 2*this->get_dimension(); i++) {
            P.push_back(padding_value);
        }
    } else if (this->get_metric() == "continuous") {
        vector<double> p;
        for (int dim = 0; dim < this->get_dimension(); dim++) {
            double value = floor(item[dim]/this->delta) * this->delta;
            p.push_back(value);
        }

        for (auto it = p.begin(); it != p.end() && (it + 1) != p.end() && (it + 2) != p.end(); ++it) {
            if (min(it[0], (it + 2)[0]) <= (it + 1)[0] && (it + 1)[0] <= max(it[0], (it + 2)[0])) {
                p.erase((it + 1));
            }
        }

        P = p;

        // Padding to d
        for (int i = P.size(); i < this->get_dimension(); i++) {
            P.push_back(padding_value);
        }
    }

    return P;
}

// Return the hash value for a specific query in a table
vector<long long int> LSH::Specific_Hash_Value(int g, vector<double> item) {
    int L = this->get_L();
    int k = this->get_k();
    long long int ID = -1;

    Euclidean_Hash_Function Hash_Fun = this->get_hash_functions()[g];

    long long int hash_value = 0;
    for (int h = 0; h < k; h++) {
        int sum = 0;
        vector <double> v = Hash_Fun.get_vector_v()[h];
        vector <double> t = Hash_Fun.get_vector_t();

        /* The inner product is calculated by multiplying all the coordinates of 2 vectors and summing them */
        for (int dim = 0; dim < this->get_dimension(); dim++) {
            sum += item[dim] * v[dim];
        }

        sum += t[h];
        sum = floor(sum / (double) this->get_w());
        hash_value += sum * r[h];
        hash_value = mod(hash_value, M);
    }

    // Storing the ID before second mod
    ID = hash_value;

    return {mod(hash_value, this->get_hashtablesize()), ID};
}

/*
* w is defined by calculating the average euclidean distance between 10% of the input data
* if the input is small and therefore 5% is less than zero then take half the points into consideration
*/
int LSH::Calculate_w() {
    long double sum = 0;
    long int subpoints;
    subpoints = this->points_num * 5/100;
    if (subpoints == 0) subpoints = this->points_num/2;
    for (int point = 0; point < subpoints - 1; point++) {
        for (int second_point = point; second_point < subpoints; second_point++) {
            sum += euclidean_dis(this->data[point], this->data[second_point]);
        }
        sum /= (subpoints - point);
    }
    sum /= 2;
    set_w(sum);
    return this->get_w();
}

Euclidean_Hash_Function::Euclidean_Hash_Function(int k, int dim) {
    // Initialize the vectors used for hashing
    v.resize(k, vector<double>(dim));

    for(int i=0; i < k; i++) {
        v[i].clear();

		for(int j=0; j < dim; j++){
			v[i].push_back(Normal_distribution());
		}
    }

    srand(time(0));

    // Initialize vector t
    // Inspired from https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
    t.clear();

	random_device generator;
	uniform_real_distribution<float> dis(0.0, (float) W);

	for(int i=0; i<k; i++){ 	// For every hash function
		float random = dis(generator);
		t.push_back(random);
	}
}

Euclidean_Hash_Function::~Euclidean_Hash_Function() {
    auto it_v = v.begin();
    it_v++;
    v.erase(it_v, v.end());

    auto it_t = t.begin();
    it_t++;
    t.erase(it_t, t.end());
}

/*
* To find the N-Nearest Items to the query
** go the each table
** find the bucket the query belongs to
** find the N nearest points to the query (with euclidean distance)
** do the same for next table
** whilst doing that compare the nearest items you found in the tables with each other and keep the N-closest
*/
vector<pair<long double, int>> Nearest_N_search(vector<double> query) {
    long double d = M; // Minimum distance
    long int b = -1; // Closest item so far

    int L = Lsh->get_L();
    int N = Lsh->get_N();

    vector<pair<long double, int>> near_items;

    Bucket *** buckets = Lsh->get_hashtables();


    for (int g = 0; g < L; g++) {

        vector<double> Gquery = query;
        if (Lsh->get_metric() == "discrete" || Lsh->get_metric() == "continuous") Gquery = Lsh->Grid(g, query);

        // Get the bucket the query belongs to
        vector<long long int> hash_value = Lsh->Specific_Hash_Value(g, Gquery);

        // if bucket is empty then skip it
        if (buckets[g][hash_value[0]] == NULL) continue;

        // For each point calculate L2 distance to find the nearest
        for (auto Points: buckets[g][hash_value[0]]->points) {
            // index = Item_id of point in bucket
            int index = Points.first.first;
            long double euc_dist;
            if (Lsh->get_metric() == "discrete")
                euc_dist = discreteFrechetDistance(Lsh->data[index], query);
            else if (Lsh->get_metric() == "continuous")
                euc_dist = Continuous_Frechet(Lsh->data[index], query);
            else
                euc_dist = euclidean_dis(Lsh->data[index], query);

            if (euc_dist < d) {
                b = index;
                if (none_of(near_items.begin(), near_items.end(), [b](pair<long double, int> item) { return b == item.second; })) {
                    if (near_items.size() >= N) {
                        near_items.pop_back();
                    }

                    near_items.push_back(make_pair(euc_dist, b));
                    sort(near_items.begin(), near_items.end());
                    d = near_items.back().first;
                }
            }
        }
    }

    return near_items;
}

/*
* To find the Nearest Items to the query within a range
** go the each table
** find the bucket the query belongs to
** calculate euclidean distance
** if the distance is within the range then the Item is near (store the point)
*** if the iterations reach 100*L then stop the search
*/
vector<int> Search_by_range(vector<double> query) {
    long int iterations = 0; // When it reaches 100L stop
    int L = Lsh->get_L();
    int k = Lsh->get_k();
    int R = Lsh->get_R();

    vector<int> near_items;

    Bucket *** buckets = Lsh->get_hashtables();

    for (int g = 0; g < L; g++) {
        vector<long long int> hash_value = Lsh->Specific_Hash_Value(g, query);

        if (buckets[g][hash_value[0]] == NULL) continue;
        for (auto Points: buckets[g][hash_value[0]]->points) {
            iterations++;
            int index = Points.first.first;
            long double euc_dist = euclidean_dis(Lsh->data[index], query);
            // index += 1; // In input file the index starts from 1

            if (euc_dist <= R) {
                if (none_of(near_items.begin(), near_items.end(), [index](int item) { return index == item; })) {
                    near_items.insert(near_items.begin(), index);
                }
            }
            if (iterations >= 100*L) break;
        }
    }

    return near_items;
}

// This function is a duplicate of the above
// the only difference is that it stores and returns the distance as well
vector<std::pair<long double,int>> LSH::Search_by_range2(vector<double> query,long int R_custom) {
    long int iterations = 0; // When it reaches 100L stop
    int L = Lsh->get_L();
    int k = Lsh->get_k();

    vector<std::pair<long double,int>> near_items;

    Bucket *** buckets = Lsh->get_hashtables();

    vector<double> Gquery = query;

    for (int g = 0; g < L; g++) {
        if (Lsh->get_metric() == "discrete" || Lsh->get_metric() == "continuous") Gquery = Lsh->Grid(g, query);

        vector<long long int> hash_value;
        hash_value = Lsh->Specific_Hash_Value(g, Gquery);

        if (buckets[g][hash_value[0]] == NULL)
            continue;

        for (auto Points: buckets[g][hash_value[0]]->points) {
            iterations++;
            int index = Points.first.first;

            long double euc_dist;
            if (Lsh->get_metric() == "discrete")
                euc_dist = discreteFrechetDistance(Lsh->data[index], query);
            else
                euc_dist = euclidean_dis(Lsh->data[index], query);

            if (euc_dist <= R_custom) {
                if (none_of(near_items.begin(), near_items.end(), [index](std::pair<long double, int> item) { return index == item.second; })) {
                    near_items.insert(near_items.begin(),std::pair<long double, int>(euc_dist,index));
                }
            }
            if (iterations >= 100*L) break;
        }
    }

    return near_items;
}
