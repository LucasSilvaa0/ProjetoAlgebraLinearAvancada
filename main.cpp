#include <bits/stdc++.h>
using namespace std;
#include "mfloat.cpp"

typedef _mfloat<70, 2, 20, -20> mfloat;

int main(){
    cout << fixed << setprecision(20);

    double ori = 10.501234560000;

    mfloat x(ori);

    cout << x.toDouble() << endl;
    cout << mfloat(x.toDouble()).toDouble() << endl;
    cout << ori << endl;
    cout << x.toDouble() - ori << endl;
    x.printReal();

}
