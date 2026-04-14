

/*
    
    x = mantissa*(base ^ exp)
    log(x) = log(mantissa) + exp*log(base)


    serie de taylor do log = somatorio de -x^i/i
    
*/
template<typename T>
T mlog_series(T x){

    T one = T(1);
    T y = (x - one) / (x + one);
    T y2 = y * y;

    T term = y;
    T sum = term;

    for(int k = 3; k < 100; k += 2){
        term = term * y2;
        sum += term / T(k);
    }

    return sum * T(2);
}

template<typename T>
T mlog(T x){
    if(x <= T(0)) return T(0)/T(0);

    
    int e = x.exp;
    
    T m = x;
    m.exp = 0; 

    static T log_base = mlog_series(T(BASE));

    return mlog_series(m) + T(e) * log_base;
}
