#include "src/curve.hpp"
#include "src/frechet.hpp"

#include <iostream>

int main()
{
    // Create first point
    Point point1_curve1(1);
    point1_curve1.set(0, 5);

    // Create second point
    Point point2_curve1(1);
    point2_curve1.set(0, 50);

     // Create second point
    Point point3_curve1(1);
    point3_curve1.set(0, 80);


    // Create first point
    Point point1_curve2(1);
    point1_curve2.set(0, 40);

    // Create second point
    Point point2_curve2(1);
    point2_curve2.set(0, 50);



    // first curve
    Curve curve1(1, "a");
    curve1.push_back(point1_curve1);
    curve1.push_back(point2_curve1);
    curve1.push_back(point3_curve1);

    // second curve
    Curve curve2(1, "b");
    curve2.push_back(point1_curve2);
    curve2.push_back(point2_curve2);

    // continuous frechet distance
    double dist = Frechet::Continuous::distance(curve1, curve2).value;
    std::cout << dist << std::endl;
}