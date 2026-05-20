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

template<typename T>
T msqrt_bs(T x){
    T l = T(0), r = max(T(1), x);

    for(int i=0; i<50; i++){

        T m = (l + r) / T(2);
        if(m*m > x) r = m;
        else l = m;
    
    }

    return l;
}