#ifndef LUZHEADER
#define LUZHEADER
#include <array>
#include <bits/stdc++.h>
using namespace std;
#define ld long double

template<const int BASE, const int MANT, int EXPMAX, int EXPMIN>
struct _mfloat {
    typedef _mfloat<MANT, BASE, EXPMAX, EXPMIN> mfloat;

    _mfloat(){ mant.fill(0); }
    _mfloat(array<int, MANT> mant, int exp) : mant(mant), exp(exp) {}
    _mfloat(ld x){
        if(x < 0) sign = -1, x = -1;

        // m1 . m2 m3 * BASE^exp  |  mi < BASE

        exp = 0;
        while(exp > EXPMIN && ((int)x % BASE == 0 && (int)x / BASE == 0)) exp--, x *= BASE;
        while(exp < EXPMAX && (int)x / BASE >= 1) exp++, x /= BASE;
        
        for(auto &m : mant){
            int digi = (int)x;
            m = digi;
            x = (x - digi) * BASE; 
        }
    }

    ld toDouble(){
        ld ans = 0;

        for(int i=(int)mant.size()-1; i>=0; i--) ans = ans / BASE + mant[i];

        for(int i=exp; i > 0; i--) ans *= BASE;
        for(int i=exp; i < 0; i++) ans /= BASE;

        return ans;
    }

    void printReal(){
        cout << mant[0] << " . ";
        for(int i=1; i<mant.size(); i++) cout << mant[i] << " ";

        cout << "  *  " << BASE << "^" << exp << endl;
    }

    mfloat operator+ (const mfloat m) const {
        mfloat ans;
        return ans;
    } 

    mfloat operator- (const mfloat m) const {
        mfloat ans;
        return ans;
    }

    mfloat operator* (const mfloat m) const {
        mfloat ans;
        return ans;
    }

    mfloat operator/ (const mfloat m) const {
        mfloat ans;
        return ans;
    }

    mfloat& operator+= (const mfloat m){ return *this = *this + m; }
    mfloat& operator-= (const mfloat m){ return *this = *this - m; }
    mfloat& operator*= (const mfloat m){ return *this = *this * m; }
    mfloat& operator/= (const mfloat m){ return *this = *this / m; }

    bool operator==(const mfloat m){ return this->sign == m.sign && this->exp == m.exp && this.mant == m.mant; }
    bool operator< (const mfloat m){ return tie(this->sign, this->exp, this->mant) < tie(m.sign, m.exp, m.mant); }
    bool operator<=(const mfloat m){ return  (*this <  m) || (*this == m); }
    bool operator> (const mfloat m){ return !(*this <= m); }
    bool operator>=(const mfloat m){ return !(*this <  m); }
    bool operator!=(const mfloat m){ return !(*this == m); }

    private:
    array<int, MANT> mant;
    int exp = 0;
    short sign = 0;

    void changeExp(int nwExp){
        
    }

    // retorna um mfloat com o dobro da mantissa
    _mfloat<MANT*2, BASE, EXPMAX, EXPMIN> extend(){
        _mfloat<MANT*2, BASE, EXPMAX, EXPMIN> ans;

        // 0 . 0 0 m1 m2 m3
        for(int i=0; i<mant.size(); i++) ans.mant[i+MANT] = mant[i];

        return ans;
    }
};

#endif