#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <iomanip>

#include "mfloat.cpp"
#include "analises.cpp"
#include "matplotlibcpp.h"
#include "algoritmos/sqrt.cpp"
#include "algoritmos/log.cpp"
#include "algoritmos/Polinomio.cpp"
#define ld long double
namespace plt = matplotlibcpp;

using namespace std;
typedef _mfloat<2, 2, 20, -20> mfloat2;
typedef _mfloat<2, 4, 20, -20> mfloat4;
typedef _mfloat<2, 6, 20, -20> mfloat6;
typedef _mfloat<2, 8, 20, -20> mfloat8;
typedef _mfloat<2, 16, 20, -20> mfloat16;

template<typename Func>
void plotGrafico(int fig, Func func, string name="", ld L=0, ld R=1, int N=1000){

    vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;

    for (int i = 0; i <= N; i++){
        ld x = L + (R - L) * i / N;

        x_values.push_back(x);

        // double version
        y_values.push_back(func(x));

        // custom types
        y_values2.push_back(func(mfloat2(x)).toDouble());
        y_values4.push_back(func(mfloat4(x)).toDouble());
        y_values6.push_back(func(mfloat6(x)).toDouble());
        y_values8.push_back(func(mfloat8(x)).toDouble());
    }

    plt::figure(fig);
    plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::title("Plot da funcao " + name);

    plt::legend();
}


int main(){
    
    plotGrafico(
        1,
        [](auto x){ return polynomiun(x, {1, -3, 2}); },
        "Polinomio x^2 - 3x + 2",
        -10,
        10
    );



    
    plt::show();
}
