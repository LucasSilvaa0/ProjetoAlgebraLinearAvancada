#include <bits/stdc++.h>
using namespace std;
#include "mfloat.cpp"

typedef _mfloat<10, 30, 20, -20> mfloat;



void testSum(long double x = 1234.56789, long double y = 34.54321){
    cout << "\n----- TESTE SUM ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat sum = mx + my;

    cout << x << " + " << y << " = " << x+y << endl;
    cout << mx.toDouble() << " + " << my.toDouble() << " = " << sum.toDouble() << endl;
    
    cout << endl;
    cout << "Diff: " << (x+y) - sum.toDouble() << endl;
    cout << endl;

    mx.printReal();
    my.printReal();
    sum.printReal();
}

void testSub(long double x = 1234.56789, long double y = 1233.56788){
    cout << "\n----- TESTE SUB ------\n" << endl;

    mfloat mx(x);
    mfloat my(y);
    mfloat sub = mx - my;

    cout << x << " - " << y << " = " << x-y << endl;
    cout << mx.toDouble() << " - " << my.toDouble() << " = " << sub.toDouble() << endl;

    cout << endl;
    cout << "Diff: " << (x-y) - sub.toDouble() << endl;
    cout << endl;

    mx.printReal();
    my.printReal();
    sub.printReal();
}


int main(){
    cout << fixed << setprecision(30);

    testSum();
    testSub();
}
