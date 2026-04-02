#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>

#include "mfloat.cpp"
#include "matplotlibcpp.h"

using namespace std;

namespace plt = matplotlibcpp;

typedef _mfloat<10, 12, 20, -20> mfloat;

int main()
{
    long double val1 = 10.501234569999999999;
    long double val2 = 10.501234565555555555;

    mfloat a(val1);
    mfloat b(val2);

    vector<double> x_values;
    vector<double> y_values;
    vector<double> y2_values;

    for (double i = 0; i <= 10000; i += 1)
    {
        mfloat mfloat_i(i);
        mfloat mfloat_dif(a * mfloat_i - b * mfloat_i);

        x_values.push_back(i);
        y_values.push_back(i * (val1 - val2));      // y = i*(val1 - val2)
        y2_values.push_back(mfloat_dif.toDouble()); // y = i*(a - b)
    }

    plt::named_plot("y = i*(val1 - val2)", x_values, y_values);
    plt::named_plot("y = i*(a - b)", x_values, y2_values);
    plt::legend();

    plt::show();
}