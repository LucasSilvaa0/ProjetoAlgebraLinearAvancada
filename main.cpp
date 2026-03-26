#include <bits/stdc++.h>
using namespace std;
#include "mfloat.cpp"

typedef _mfloat<10, 30, 20, -20> mfloat;

int main(){
    cout << fixed << setprecision(30);

    long double ori = 10.501234560000000000;

    mfloat x(ori);

    cout << x.toDouble() << endl;
    cout << mfloat(x.toDouble()).toDouble() << endl;
    cout << ori << endl;
    cout << x.toDouble() - ori << endl;
    x.printReal();

}
