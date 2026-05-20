#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <iomanip>

#include "mfloat.cpp"
#include "analises.cpp"
#include "matplotlibcpp.h"
#define ld long double

using namespace std;

namespace plt = matplotlibcpp;

typedef _mfloat<2, 20, 20, -20> mfloat;

int main()
{
    ld val1 = 10.50199999999999;
    ld val2 = 10.50100000000000;

    mfloat a(val1);
    mfloat b(val2);

    vector<ld> x_values;
    vector<ld> y_values;
    vector<ld> y2_values;
    vector<ld> analise_direta_values;
    vector<ld> analise_inversa_values;

    for (ld i = 1; i <= 50000; i += 1)
    {
        ld d = i;
        ld S = pow(d * (val1 - val2), 3); // S(d) = (d*(a - b))^3

        mfloat mfloat_i(i);
        mfloat mfloat_dif(a * mfloat_i - b * mfloat_i);
        mfloat_dif *= mfloat_dif * mfloat_dif;

        ld Sc = mfloat_dif.toDouble(); // Sc(d) = (d*(a - b))^3 usando mfloat

        ld dn = pow(Sc, 1.0/3.0) / (val1 - val2); // S-1(Sc(d)) = (Sc(d)^(1/3)) / (a - b)

        Analise analise(d, S, Sc, dn);

        x_values.push_back(d);
        y_values.push_back(S);   // y = (d*(val1 - val2))^3
        y2_values.push_back(Sc); // y = (d*(val1 - val2))^3
        analise_direta_values.push_back(analise.Direta());
        analise_inversa_values.push_back(analise.Inversa());
    }

    plt::figure(1);
    plt::named_plot("y = (d*(val1 - val2))^3", x_values, y_values);
    plt::named_plot("y = (d*(a - b))^3", x_values, y2_values);
    plt::title("Função: S vs Sc");
    plt::legend();

    plt::figure(2);
    plt::named_plot("y = | (S - Sc) /  S |", x_values, analise_direta_values);
    plt::named_plot("y = | (d - dn) / d |", x_values, analise_inversa_values);
    plt::title("Erros: Direta vs Inversa");
    plt::legend();
    plt::show();
}