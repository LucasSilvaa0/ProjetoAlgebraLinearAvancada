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
#define ld long double
namespace plt = matplotlibcpp;

using namespace std;
typedef _mfloat<2, 2, 20, -20> mfloat2;
typedef _mfloat<2, 4, 20, -20> mfloat4;
typedef _mfloat<2, 6, 20, -20> mfloat6;
typedef _mfloat<2, 8, 20, -20> mfloat8;
typedef _mfloat<2, 16, 20, -20> mfloat16;


template<typename T, typename Func>
ld direta(ld x, Func func){ 
    ld ref = func(x);                 
    ld approx = func(T(x)).toDouble(); 
    return abs(ref - approx) / abs(ref);
}

template<typename T, typename Func, typename InvFunc>
ld inversa(ld x, Func func, InvFunc inv_func){ 
    ld y_approx = func(T(x)).toDouble();
    ld x_reconstructed = inv_func(y_approx);
    return abs(x_reconstructed - x) / abs(x);
}

template<typename Func, typename InvFunc>
void validate(int fig, Func func, InvFunc inv_func, string name=""){

    vector<ld> x_values;

    vector<ld> y2, y4, y6, y8;
    vector<ld> y2inv, y4inv, y6inv, y8inv;

    ld L = 0, R = 16;
    int N = 1000;

    for (int i = 0; i <= N; i++){
        ld x = L + (R - L) * i / N;

        x_values.push_back(x);

        y2.push_back(direta<mfloat2>(x, func));
        y4.push_back(direta<mfloat4>(x, func));
        y6.push_back(direta<mfloat6>(x, func));
        y8.push_back(direta<mfloat8>(x, func));

        y2inv.push_back(inversa<mfloat2>(x, func, inv_func));
        y4inv.push_back(inversa<mfloat4>(x, func, inv_func));
        y6inv.push_back(inversa<mfloat6>(x, func, inv_func));
        y8inv.push_back(inversa<mfloat8>(x, func, inv_func));
    }

    plt::figure(fig);

    plt::named_plot("mantissa=2 direto", x_values, y2);
    plt::named_plot("mantissa=2 inverso", x_values, y2inv);

    plt::named_plot("mantissa=4 direto", x_values, y4);
    plt::named_plot("mantissa=4 inverso", x_values, y4inv);

    plt::named_plot("mantissa=8 direto", x_values, y8);
    plt::named_plot("mantissa=8 inverso", x_values, y8inv);

    plt::title("Analise direta x inversa " + name);
    plt::legend();
}


int main(){
    
    validate(
        1,
        [](auto x){ return msqrt(x); },
        [](ld y){ return y * y; },
        "Sqrt"
    );

    validate(
        2,
        [](auto x){ return msqrt_bs(x); },
        [](ld y){ return y * y; },
        "Sqrt Binary Search"
    );

    
    validate(
        3,
        [](auto x){ return mlog(x); },
        [](ld y){ return exp(y); },
        "Log e"
    );

    plt::show();
}
