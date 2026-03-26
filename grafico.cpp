#include "matplotlibcpp.h"
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <iostream>

namespace plt = matplotlibcpp;

int main()
{
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> y2;

    for (double i = -5; i <= 5; i += 0.1)
    {
        x.push_back(i);
        y.push_back(i * i);      // x²
        y2.push_back(i * i * i); // x³
    }

    plt::named_plot("y = x^2", x, y);
    plt::named_plot("y = x^3", x, y2);
    plt::legend();

    plt::show();
}