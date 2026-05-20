
template<typename T>
T abs(T x){
    if(x < T(0)) return x * T(-1);
    return x;
}


template<typename T>
T mlog_series(T x){
    T one = T(1);
    T y = (x - one) / (x + one);
    T y2 = y * y;

    T term = y;
    T sum = term;

    const int MAX_ITER = 100;

    for(int k = 3, iter = 0; iter < MAX_ITER; k += 2, iter++){
        term *= y2;
        T add = term / T(k);
        sum += add;

        if (abs(add) < T(1e-12)) break;
    }

    return sum * T(2);
}

template<typename T>
T mlog(T x){
    if(x <= T(0)) return T(0)/T(0); // NaN

    if(x < T(1)){
        return mlog(T(1) / x) * T(-1);
    }

    int k = 0;
    T base = T(2);

    while(x > T(1.5)){
        x /= base;
        k++;
    }

    static T log2 = mlog_series(base);

    return mlog_series(x) + T(k) * log2;
}