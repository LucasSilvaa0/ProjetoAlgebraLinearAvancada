#ifndef ANALISES
#define ANALISES
#include <array>
#include <bits/stdc++.h>
using namespace std;
#define ld long double

class Analise
{
public:
    Analise(
        ld _d,   // d
        ld _Sd,  // Sd = S(d)
        ld _Scd, // Sc(d)
        ld _dn   // dn = S-1(Sc(d))
    ) : d(_d), Sd(_Sd), Scd(_Scd), dn(_dn) {}

    ld Direta() {
        return abs(Sd - Scd);
    }
    
    ld Inversa() {
        return abs(dn - Sd);
    }

    void MelhorAnalise() {
        ld direta = Direta();
        ld inversa = Inversa();
        
        if (direta < inversa)
            cout << "Melhor análise: Direta." << endl;
        else if (inversa < direta)
            cout << "Melhor análise: Inversa." << endl;
        else
            cout << "Análises iguais." << endl;
    }

    ld d;
    ld Sd;
    ld Scd;
    ld dn;
};

#endif