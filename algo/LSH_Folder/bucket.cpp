#include "LSH.hpp"

void Bucket::add(long long int point_id,long int g_i,long long int hash_id)
{
    points.push_back(make_pair(make_pair(point_id, hash_id), g_i));
}

//Fill hashtables with Points (data structure initialization) for LSH.
void LSH_Insert_Points_To_Buckets(LSH* info){
    long long int points_num = info->get_pointsnum();
    int L_var = info->get_L();

    //Fill buckets of Hash_Table
    for(int i=0;i<points_num;i++){
        if (info->get_metric() == "continuous")
            info->data[i] = Filter_Curve(info->data[i]);
        for(int j=0;j<L_var;j++){
            vector<double> data;
            if (info->get_metric() == "discrete" || info->get_metric() == "continuous")
                data = info->Grid(j, info->data[i]);
            else
                data = info->data[i];

            vector<long long int> hash_values = info->Specific_Hash_Value(j, data);
            if(info->get_hashtables()[j][hash_values[0]]==NULL)
                info->get_hashtables()[j][hash_values[0]] = new Bucket();
            info->get_hashtables()[j][hash_values[0]]->add((long long int) i , hash_values[0], hash_values[1]);
        }
    }
}