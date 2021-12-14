#include "../functions/functions.hpp"
#include "../curves/discretef.hpp"

#include <CUnit/CUnit.h>
#include "CUnit/Basic.h"

using namespace std;

static FILE* temp_file = NULL;


int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

// We are testing the below functions
void ContinuousFrechet();
void DiscreteFrechet();
void Curve_Filtering();
void Mean_Curve();

int main()
{
    CU_pSuite Countinuous_Suite = NULL, Discrete_Suite = NULL, Filtering_Suite = NULL,
              Mean_Curve_Suite = NULL;

    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

    // Continuous Distance
    Countinuous_Suite = CU_add_suite("Continuous_Frechet", init_suite, clean_suite);
    if (NULL == Countinuous_Suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(Countinuous_Suite, "\n\nn Testing \n\n", ContinuousFrechet))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Discrete Distance
    Discrete_Suite = CU_add_suite("Test_Continuous_Frechet", init_suite, clean_suite);
    if (NULL == Discrete_Suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(Discrete_Suite, "\n\n Testing \n\n", DiscreteFrechet))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Curve Filtering
    Filtering_Suite = CU_add_suite("Test_Continuous_Frechet", init_suite, clean_suite);
    if (NULL == Filtering_Suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(Filtering_Suite, "\n\n Testing \n\n", Curve_Filtering))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Mean Curve
    Mean_Curve_Suite = CU_add_suite("Test_Continuous_Frechet", init_suite, clean_suite);
    if (NULL == Mean_Curve_Suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(Mean_Curve_Suite, "\n\n Testing \n\n", Mean_Curve))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();// OUTPUT to the screen

    CU_cleanup_registry();
    return CU_get_error();

}

void ContinuousFrechet() {
    double distance = 0.0; 

    vector<double> curve1 {5, 5, 5};
    vector<double> curve2 {10, 10};
    distance = Continuous_Frechet(curve1, curve2);
    CU_ASSERT(5 == distance);

    vector<double> curve3 {20, 20, 20, 30};
    vector<double> curve4 {25, 40};
    distance = Continuous_Frechet(curve3, curve4);
    CU_ASSERT(10 == distance);
}

void DiscreteFrechet() {
    double distance = 0.0; 

    vector<double> curve1 {5, 5, 5};
    vector<double> curve2 {10, 10};
    distance = discreteFrechetDistance(curve1, curve2);
    CU_ASSERT(5 == distance); 


    vector<double> curve3 {20, 20, 20, 30};
    vector<double> curve4 {25, 40};
    distance = discreteFrechetDistance(curve3, curve4);
    CU_ASSERT(10 == distance);  

}

void Curve_Filtering() {
    vector<double> filtered_curve;

    vector<double> curve1 {1, 2, 3};
    vector<double> curve3 {1, 2, 3};
    filtered_curve = Filter_Curve(curve1);
    CU_ASSERT(filtered_curve == curve3);


    vector<double> curve2 {100, 101, 101, 195, 210, 200};
    vector<double> curve4 {100, 101, 195, 200};
    filtered_curve = Filter_Curve(curve2);
    CU_ASSERT(filtered_curve == curve4);
}

void Mean_Curve() {
    vector<double> curve;

    vector<double> curve2 {10.5, 90, 195, 210};
    vector<double> curve4 {100, 101, 20.5, 200};
    vector<int> cluster {1, 0};
    vector<vector<double>> data;
    data.push_back(curve2);
    data.push_back(curve4);
    vector<double> curve3 {105, 197.5, 107.75, 95.5};
    curve = create_mean_curve_tree(cluster, curve3, data, 4);
    CU_ASSERT(curve == curve3);
}
