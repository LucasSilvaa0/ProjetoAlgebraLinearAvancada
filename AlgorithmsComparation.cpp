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
    vector<ld> y_values16;

    ld L = 0, R = 1;
    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        y_values.push_back(msqrt_bs(x));

        y_values2.push_back(msqrt_bs<mfloat2>(x).toDouble()); 
        y_values4.push_back(msqrt_bs<mfloat4>(x).toDouble());
        y_values6.push_back(msqrt_bs<mfloat6>(x).toDouble());
        y_values8.push_back(msqrt_bs<mfloat8>(x).toDouble());
        y_values16.push_back(msqrt_bs<mfloat16>(x).toDouble());
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
    vector<ld> y_values16;

    // log só definido pra x > 0
    ld L = 0.001, R = 5;

    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        // versão double
        y_values.push_back(log(x));

        // versões mfloat
        y_values2.push_back(mlog<mfloat2>(x).toDouble()); 
        y_values4.push_back(mlog<mfloat4>(x).toDouble());
        y_values6.push_back(mlog<mfloat6>(x).toDouble());
        y_values8.push_back(mlog<mfloat8>(x).toDouble());
        y_values16.push_back(mlog<mfloat16>(x).toDouble());
    }
    
    plt::figure(fig);
    plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::named_plot("mantissa=16", x_values, y_values16);
    plt::title("Função: Log");
    plt::legend();
}

template<typename T>
ld direta(ld x){ return abs(sqrt(x) - msqrt<T>(x).toDouble()) / abs(sqrt(x)); }

template<typename T>
ld inversa(ld x){ 
    ld y = sqrt(x);
    ld ym = msqrt<T>(x).toDouble();
    ld xm = ym*ym;

    return abs(xm - x) / x;
}

void validate(int fig){
  vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;
    
    vector<ld> y_values2inversa;
    vector<ld> y_values4inversa;
    vector<ld> y_values6inversa;
    vector<ld> y_values8inversa;;

    ld L = 0, R = 16;
    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        // y_values.push_back(direta<ld>(x));

        y_values2.push_back(direta<mfloat2>(x)); 
        y_values4.push_back(direta<mfloat4>(x));
        y_values6.push_back(direta<mfloat6>(x));
        y_values8.push_back(direta<mfloat8>(x));
        
        y_values2inversa.push_back(inversa<mfloat2>(x));
        y_values4inversa.push_back(inversa<mfloat4>(x));
        y_values6inversa.push_back(inversa<mfloat6>(x));
        y_values8inversa.push_back(inversa<mfloat8>(x));
    }

    plt::figure(fig);
    // plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2 direto", x_values, y_values2);
    plt::named_plot("mantissa=2 inverso", x_values, y_values2inversa);
    
    plt::named_plot("mantissa=4 direto", x_values, y_values4);
    plt::named_plot("mantissa=4 inverso", x_values, y_values4inversa);

    // plt::named_plot("mantissa=6 direto", x_values, y_values6);
    // plt::named_plot("mantissa=6 inverso", x_values, y_values6inversa);
    
    plt::named_plot("mantissa=8 direto", x_values, y_values8);
    plt::named_plot("mantissa=8 inverso", x_values, y_values8inversa);

    plt::title("Analise Sqrt direto x inverso");
    plt::legend();
}



template<typename T>
ld diretalog(ld x){ return abs(log(x) - mlog<T>(x).toDouble()); }

void validate2(int fig){
  vector<ld> x_values;

    vector<ld> y_values;
    vector<ld> y_values2;
    vector<ld> y_values4;
    vector<ld> y_values6;
    vector<ld> y_values8;

    ld L = 0, R = 1;
    for (ld x = L; x <= R; x += (R-L)/1000){
        
        x_values.push_back(x);

        // y_values.push_back(diretalog<ld>(x));

        y_values2.push_back(diretalog<mfloat2>(x)); 
        y_values4.push_back(diretalog<mfloat4>(x));
        y_values6.push_back(diretalog<mfloat6>(x));
        y_values8.push_back(diretalog<mfloat8>(x));
    }

    plt::figure(fig);
    // plt::named_plot("double", x_values, y_values);
    plt::named_plot("mantissa=2", x_values, y_values2);
    plt::named_plot("mantissa=4", x_values, y_values4);
    plt::named_plot("mantissa=6", x_values, y_values6);
    plt::named_plot("mantissa=8", x_values, y_values8);
    plt::title("Analise Log");
    plt::legend();
}

int main(){
    msqrt(1);
    msqrt_bs(2);
    mloggrafico(3);

    validate(4);
    validate2(5);

    plt::show();
}
