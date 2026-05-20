#pragma once
using namespace std;
#include <bits/stdc++.h>

template<typename T>
T msqrt(T x){
    if(x < T(0)) return T(0)/T(0);
    T ans = x;
    
    for(int i=0; i<50; i++){
        ans = (ans + x/ans) / T(2);
    }

    return ans;
}