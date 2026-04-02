#include <bits/stdc++.h>
using namespace std;
#include "mfloat.cpp"

typedef _mfloat<10, 5, 20, -20> mfloat;

int main()
{
    cout << fixed << setprecision(30);

    long double val1 = 10.501234560055555555;
    long double val2 = 10.501234560099999999;

    mfloat x(val1);
    mfloat y(val2);

    x.printReal();
    y.printReal();

    mfloat sum = x + y;
    sum.printReal();

    mfloat dif = x - y;
    dif.printReal();
}
