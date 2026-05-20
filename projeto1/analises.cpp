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
        ld _S,  // S = S(d)
        ld _Sc, // Sc(d)
        ld _dn   // dn = S-1(Sc(d))
    ) : d(_d), S(_S), Sc(_Sc), dn(_dn) {}

    ld Direta() {
        if (S == 0)
            return (Sc == 0) ? 0 : numeric_limits<ld>::infinity();
        return abs((S - Sc) / S);
    }
    
    ld Inversa() {
        if (d == 0)
            return (dn == 0) ? 0 : numeric_limits<ld>::infinity();
        return abs((d - dn) / d);
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
    ld S;
    ld Sc;
    ld dn;
};

#endif