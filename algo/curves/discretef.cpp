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
			euclidean_distance =  sqrt(pow( curve_query[i-1] - curve[j-1], 2 ));

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