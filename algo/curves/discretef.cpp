#include "discretef.hpp"

long double discreteFrechetDistance( vector<double> curve_query, vector<double> curve){
	int curve_points_num,query_points_num,dimension,i,j,k,l;
    double max, min, distance_DFD,sum;
	long double euclidean_distance;
    
    query_points_num = curve_query.size();
	curve_points_num = curve.size();

    //cerate dynamic array of distances
	long double **c = new long double*[query_points_num+1];
	for(int i = 0; i <=query_points_num; ++i)
    	c[i] = new long double[curve_points_num+1];
	
	// Discrete Frechet Distance
	for( i=1; i<query_points_num+1; i++ )
	{
		for( j=1; j<curve_points_num+1; j++ )
		{
			// Euclidean distance/norm
			euclidean_distance =  sqrt(pow(i - j, 2) + pow( curve_query[i-1] - curve[j-1], 2 ));

			//choose path based on theory
			if( i == 1 && j == 1 )
				c[i][j] = euclidean_distance;
			else if( i == 1 ){
				if( euclidean_distance <= c[1][j-1] )
                    c[i][j] = c[1][j-1];
				else
					c[i][j] = euclidean_distance;
			}
			else if( j == 1 )
			{
				if( euclidean_distance <= c[i-1][1] )
                    c[i][j] = c[i-1][1];
				else
                    c[i][j] = euclidean_distance;
			}
			else
			{
                min = std::numeric_limits<double>::infinity();
				max = euclidean_distance;

				if( c[i-1][j] < min )
					min = c[i-1][j];

                if( c[i-1][j-1] < min )
					min = c[i-1][j-1];

				if( c[i][j-1] < min )
					min = c[i][j-1];

				if( euclidean_distance > min )
					max = euclidean_distance;
				else
					max = min;

				c[i][j] = max;
			}
		}
	}

	distance_DFD = c[query_points_num][curve_points_num];

	for(int i = 0; i <=query_points_num; ++i)
		delete [] c[i];

	delete [] c;

	return distance_DFD;
}

long double** create_DFD_Table( vector<double> curve_query, vector<double> curve){
	int curve_points_num,query_points_num,dimension,i,j,k,l;
    double max, min, distance_DFD,sum;
    long double euclidean_distance;

    query_points_num = curve_query.size();

    curve_points_num = curve.size();

	long double **c = new long double*[query_points_num+1];
	for(int i = 0; i <=query_points_num; ++i)
    	c[i] = new long double[curve_points_num+1];
	
	// Discrete Frechet Distance
	for( i=1; i<query_points_num+1; i++ )
	{
		for( j=1; j<curve_points_num+1; j++ )
		{
			// Euclidean distance norm 
			euclidean_distance =  sqrt(pow( curve_query[i-1] - curve[j-1], 2 ));

			if( i == 1 && j == 1 )
				c[i][j] = euclidean_distance;
			else if( i == 1 ){
				if( euclidean_distance <= c[1][j-1] )
                    c[i][j] = c[1][j-1];
				else
					c[i][j] = euclidean_distance;
			}
			else if( j == 1 )
			{
				if( euclidean_distance <= c[i-1][1] )
                    c[i][j] = c[i-1][1];
				else
                    c[i][j] = euclidean_distance;
			}
			else
			{
                min = std::numeric_limits<double>::infinity();
				max = euclidean_distance;

				if( c[i-1][j] < min )
					min = c[i-1][j];

                if( c[i-1][j-1] < min )
					min = c[i-1][j-1];

				if( c[i][j-1] < min )
					min = c[i][j-1];

				if( euclidean_distance > min )
					max = euclidean_distance;
				else
					max = min;

				c[i][j] = max;
			}
		}
	}

	return c;
}

int min3_index(double a, double b, double c){
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

vector<pair<double,double>> optimal_Traversal_Computation(vector<double> P, vector<double> Q) {
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

    for (int n = 0; n <= P.size(); n++)
        delete[] L[n];
    delete[] L;

    return traversal;
}

vector<double> mean_Discrete_Frechet_Curve(vector<double> P, vector<double> Q) {
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
	return mean_curve;
}

vector<double> create_mean_curve_tree(vector<int> cluster, vector<double> center, vector<vector<double>> data, int dim) {

    //if cluster is empty
    if (cluster.size() == 0 || cluster.size() == 1)
        return center;


    vector<vector<double>> curr_node;

    // Start with original curves as leaves

    std::vector<int> indexes;
    for (int i = 0; i < cluster.size(); i++) {
        indexes.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    shuffle(indexes.begin(), indexes.end(), g);
    
    for (int i = 0 ; i < cluster.size() ; i++){
        curr_node.push_back(data[cluster[indexes[i]]]);
    }
    indexes.clear();

    vector<vector<double>> nextNodes;

    while(curr_node.size()>1){
    // Take curves in pairs
        //step 2 because we have binary tree
        for (int i = 0 ; i < curr_node.size() - 1; i+=2) {
            vector<double> mean_curve = mean_Discrete_Frechet_Curve(curr_node[i], curr_node[i+1]);
            while (mean_curve.size() > size_t (dim)) {
                mean_curve.erase(mean_curve.end() - 1, mean_curve.end());
                if (mean_curve.size() == size_t (dim)) break;
                mean_curve.erase(mean_curve.begin(), mean_curve.begin() + 1);
            }

            nextNodes.push_back(mean_curve);
            mean_curve.clear();
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
    vector<double> curve = curr_node[0];
    curr_node.clear();
    return curve;
}