#ifndef LUZHEADER
#define LUZHEADER
#include <array>
#include <bits/stdc++.h>
using namespace std;
#define ld long double

template<const int BASE, const int MANT, int EXPMAX, int EXPMIN>
struct _mfloat {
    typedef _mfloat<BASE, MANT, EXPMAX, EXPMIN> mfloat;

    _mfloat(){ mant.fill(0); }
    _mfloat(array<int, MANT> mant, int exp) : mant(mant), exp(exp) {}
    _mfloat(ld x){
        if(x < 0) sign = -1, x *= -1;
        else sign = +1;

        // m1 . m2 m3 * BASE^exp  |  mi < BASE

        exp = 0;
        while(exp > EXPMIN && ((int)x % BASE == 0 && (int)x / BASE == 0)) exp--, x *= BASE;
        while(exp < EXPMAX && (int)x / BASE >= 1) exp++, x /= BASE;
        
        for(auto &m : mant){
            int digi = (int)x;
            m = digi;
            x = (x - digi) * BASE; 
        }

        // arredonda?
    }

    mfloat operator+ (mfloat m) const {
        if(this->sign != m.sign){ // x + (-y) = x - y
            m.sign *= -1;
            return *this - m;
        }
        // aumenta a mantissa e iguala a base
        auto a = this->extend();
        auto b = m.extend();

        
        a.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        b.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        (a.exp < b.exp ? a : b).shiftR(abs(a.exp - b.exp)); //a.exp = b.exp
        // --- //
        
        auto sum = a;
        int carry = 0;
        for(int i=a.mant.size()-1; i >= 0; i--){
            sum.mant[i] = a.mant[i] + b.mant[i] + carry;
            carry = sum.mant[i] / BASE;
            sum.mant[i] %= BASE;
        }

        assert(carry == 0);

        sum.fix();

        mfloat ans;
        ans.exp = sum.exp;
        ans.sign = sum.sign;

        for(int i=0; i<ans.mant.size(); i++)
            ans.mant[i] = sum.mant[i];

        // arredonda?

        return ans;
    } 

    mfloat operator- (mfloat m) const {
        if(this->sign != m.sign){ // x - (-y) = x + y
            m.sign *= -1;
            return *this + m;
        }
        // aumenta a mantissa e iguala a base
        auto a = this->extend();
        auto b = m.extend();

        
        a.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        b.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        (a.exp < b.exp ? a : b).shiftR(abs(a.exp - b.exp)); //a.exp = b.exp
        // --- //
        
        if(a < b) swap(a, b);

        auto sub = a;
        int carry = 0;
        for(int i=a.mant.size()-1; i >= 0; i--){
            sub.mant[i] = a.mant[i] - (b.mant[i] + carry);
            carry = 0;
            if(sub.mant[i] < 0){
                int v = sub.mant[i];
                carry = (abs(v) + BASE - 1) / BASE; // teto da divisão pra ficar positivo
                sub.mant[i] = BASE + v % BASE;
            }
            carry = sub.mant[i] / BASE;
            sub.mant[i] %= BASE;
        }

        assert(carry == 0);

        sub.fix();

        mfloat ans;
        ans.exp = sub.exp;
        for(int i=0; i<ans.mant.size(); i++)
            ans.mant[i] = sub.mant[i];
        
        ans.sign = a.sign;

        // arredonda?

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

    bool operator==(const mfloat m) const { return this->sign == m.sign && this->exp == m.exp && this.mant == m.mant; }
    bool operator< (const mfloat m) const { return tie(this->sign, this->exp, this->mant) < tie(m.sign, m.exp, m.mant); }
    bool operator<=(const mfloat m) const { return  (*this <  m) || (*this == m); }
    bool operator> (const mfloat m) const { return !(*this <= m); }
    bool operator>=(const mfloat m) const { return !(*this <  m); }
    bool operator!=(const mfloat m) const { return !(*this == m); }

    ld toDouble() const{
        ld ans = 0;

        for(int i=(int)mant.size()-1; i>=0; i--) ans = ans / BASE + mant[i];

        for(int i=exp; i > 0; i--) ans *= BASE;
        for(int i=exp; i < 0; i++) ans /= BASE;

        if(sign < 0) ans *= -1;

        return ans;
    }

    void printReal() const {
        if(sign < 0) cout << "-";
        cout << mant[0] << ".";
        for(int i=1; i<mant.size(); i++) cout << (BASE > 10 ? " " : "") << mant[i];
        cout << " * " << BASE << "^" << exp << endl;
    }

    array<int, MANT> mant;
    int exp = 0;
    short sign = 0;

    void shiftL(int sh){
        auto nwMant = mant;
        nwMant.fill(0);

        for(int i=0; i<mant.size(); i++) if(i+sh >= 0 && i+sh < mant.size()){
            nwMant[i] = mant[i+sh];
        }
        
        mant = nwMant;
        exp -= sh;
    }
    void shiftR(int sh){ shiftL(-sh); }

    void fix(){

        int dlt = 0;
        while(dlt < mant.size() && mant[dlt] == 0) dlt++;

        if(dlt == mant.size()) exp = EXPMIN; // 0.00
        else shiftL(dlt);
    }

    // retorna um mfloat com o dobro da mantissa
    _mfloat<BASE, MANT*2, EXPMAX, EXPMIN> extend() const {
        _mfloat<BASE, MANT*2, EXPMAX, EXPMIN> ans;

        // 0 . 0 0 m1 m2 m3
        for(int i=0; i<mant.size(); i++) ans.mant[i+MANT] = mant[i];
        ans.exp = exp + MANT;
        ans.sign = sign;

        return ans;
    }
};

#endif