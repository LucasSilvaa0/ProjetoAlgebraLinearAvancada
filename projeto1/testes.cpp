#include <bits/stdc++.h>
using namespace std;
#include "mfloat.cpp"
const double toleratedErro = 1e-15;

const int MANTISSA = 30;
typedef _mfloat<10, MANTISSA, 20, -20> mfloat;

double testSum(long double x = 1234.56789, long double y = 34.54321){
    cout << "\n----- TESTE SUM ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat sum = mx + my;
    long double expected = x + y;

    cout << x << " + " << y << " = " << expected << endl;
    cout << mx.toDouble() << " + " << my.toDouble() << " = " << sum.toDouble() << endl;
    
    auto error = expected - sum.toDouble();
    cout << "\n" << "error: " << error << " or " << mfloat(error) << "\n\n";
    
    cout << mx << " + " << my << " = " << sum << endl;
    return error;
}

double testSub(long double x = 1234.56789, long double y = 1233.56788){
    cout << "\n----- TESTE SUB ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat sub = mx - my;
    long double expected = x - y;

    cout << x << " - " << y << " = " << expected << endl;
    cout << mx.toDouble() << " - " << my.toDouble() << " = " << sub.toDouble() << endl;
    
    auto error = expected - sub.toDouble();
    cout << "\n" << "error: " << error << " or " << mfloat(error) << "\n\n";
    
    cout << mx << " + " << my << " = " << sub << endl;
    return error;
}

double testMul(long double x = 14.59, long double y = 2.000001){
    cout << "\n----- TESTE MUL ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat mul = mx * my;
    long double expected = x * y;

    cout << x << " * " << y << " = " << expected << endl;
    cout << mx.toDouble() << " * " << my.toDouble() << " = " << mul.toDouble() << endl;
    
    auto error = expected - mul.toDouble();
    cout << "\n" << "error: " << error << " or " << mfloat(error) << "\n\n";
    
    cout << mx << " * " << my << " = " << mul << endl;
    return error;
}

double testDiv(long double x = 1.5, long double y = 50120232.0){
    cout << "\n----- TESTE DIV ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat div = mx / my;
    long double expected = x / y;

    cout << x << " / " << y << " = " << expected << endl;
    cout << mx.toDouble() << " / " << my.toDouble() << " = " << div.toDouble() << endl;
    
    auto error = expected - div.toDouble();
    cout << "\n" << "error: " << error << " or " << mfloat(error) << "\n\n";
    
    cout << mx << " / " << my << " = " << div << endl;
    return error;
}

int main(){
    cout << fixed << setprecision(MANTISSA);

    testSum();
    testSub();
    testMul();
    testDiv();
}
