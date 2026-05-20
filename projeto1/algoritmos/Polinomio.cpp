#pragma once
using namespace std;
#include <bits/stdc++.h>

template<typename T>
T polynomiun(T x, vector<int> poli){
    T ans = 0;

    for(int i=0; i<poli.size(); i++){
        T term = poli[i];
        for(int j=0; j<i; j++) term *= x;
        ans += term;
    }

    return ans;
}