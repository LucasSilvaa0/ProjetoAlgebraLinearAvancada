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

void msqrt(int fig){

    vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;

    ld L = 0, R = 1;
    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        y_values.push_back(msqrt(x));

        y_values2.push_back(msqrt<mfloat2>(x).toDouble()); 
        y_values4.push_back(msqrt<mfloat4>(x).toDouble());
        y_values6.push_back(msqrt<mfloat6>(x).toDouble());
        y_values8.push_back(msqrt<mfloat8>(x).toDouble());
    }

    plt::figure(fig);
    plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::title("Função: Sqrt");
    plt::legend();

}


void msqrt_bs(int fig){

    vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;

    ld L = 0, R = 1;
    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        y_values.push_back(msqrt_bs(x));

        y_values2.push_back(msqrt_bs<mfloat2>(x).toDouble()); 
        y_values4.push_back(msqrt_bs<mfloat4>(x).toDouble());
        y_values6.push_back(msqrt_bs<mfloat6>(x).toDouble());
        y_values8.push_back(msqrt_bs<mfloat8>(x).toDouble());
    }

    plt::figure(fig);
    plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::title("Função: Sqrt com binary search");
    plt::legend();

}

void mloggrafico(int fig){

    vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;

    // log só definido pra x > 0
    ld L = 0.001, R = 1;

    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        // versão double
        y_values.push_back(log(x));

        // versões mfloat
        y_values2.push_back(mlog<mfloat2>(x).toDouble()); 
        y_values4.push_back(mlog<mfloat4>(x).toDouble());
        y_values6.push_back(mlog<mfloat6>(x).toDouble());
        y_values8.push_back(mlog<mfloat8>(x).toDouble());
    }

    plt::figure(fig);
    plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::title("Função: Log");
    plt::legend();
}


int main(){
    msqrt(1);
    msqrt_bs(2);

    plt::show();
}
