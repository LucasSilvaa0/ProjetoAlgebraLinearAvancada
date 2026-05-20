#ifndef MFLOATHEADER
#define MFLOATHEADER
#include <array>
#include <bits/stdc++.h>
using namespace std;
#define ld long double

/*
 * _mfloat<BASE, MANT, EXPMAX, EXPMIN>
 * ------------------------------------
 * Implementa um numero de ponto flutuante "manual" com base parametrizavel.
 *
 * Representacao interna:
 *   valor ~= sign * (mant[0].mant[1]mant[2]...) * BASE^exp
 *   - BASE   : base numerica (ex.: 10)
 *   - MANT   : quantidade de digitos da mantissa
 *   - EXPMAX : maior expoente permitido
 *   - EXPMIN : menor expoente permitido
 *
 * Convencoes importantes neste codigo:
 *   - mant[i] guarda um "digito" em base BASE, idealmente 0 <= mant[i] < BASE.
 *   - fix() normaliza removendo zeros a esquerda da mantissa.
 *   - shiftL/shiftR deslocam mantissa e compensam exp para manter escala.
 *   - extend() duplica a precisao temporariamente para reduzir perda em operacoes.
 *
 * Observacoes:
 *   - O campo sign existe, mas o codigo ainda nao trata completamente sinal em
 *     todas as operacoes.
 *   - operadores -, *, / estao como esqueleto.
 *   - estrategia de arredondamento ainda nao foi finalizada.
 *
 */



template <const int BASE, const int MANT, int EXPMAX, int EXPMIN>
struct _mfloat
{
    typedef _mfloat<BASE, MANT, EXPMAX, EXPMIN> mfloat;

    _mfloat() { mant.fill(0); }
    _mfloat(array<int, MANT> mant, int exp) : mant(mant), exp(exp) {}
    _mfloat(ld x){
         // Extrai sinal e trabalha com modulo para preencher mantissa.
        if(x < 0) sign = -1, x *= -1;
        else sign = +1;

        // m1 . m2 m3 * BASE^exp  |  mi < BASE

        exp = 0;
        // Ajusta escala para que a parte inteira fique em faixa valida da base.
        while (exp+1 > EXPMIN && ((int)x % BASE == 0 && (int)x / BASE == 0))
            exp--, x *= BASE;
        while (exp-1 < EXPMAX && (int)x / BASE >= 1)
            exp++, x /= BASE;

        if(isnan(x)) { *this = makeNan(); return; }
        if(exp > EXPMAX || isinf(x)) { *this = makeInf(sign); return; }
        if(exp < EXPMIN) { *this = makeZero(sign); return; }

        // "Quebra" x em digitos da mantissa na base escolhida.
        for (auto &m : mant){
            int digi = (int)x;
            m = digi;
            x = (x - digi) * BASE;
        }

        // arredonda?
    }

    mfloat operator+ (mfloat m) const {
        if(this->isNan() || m.isNan()) return makeNan();                                       // x + nan = nan
        if(this->isInf() && m.isInf() && this->sign == m.sign) return makeInf(this->sign);     // +inf + +inf = +inf, -inf + -inf = -inf 
        if(this->isInf() && m.isInf()) return makeNan();                                       // +inf + -inf = nan
        if(this->isInf() || m.isInf()) return this->isInf() ? *this : m;                       // x + inf = inf

        if(this->sign != m.sign){ // x + (-y) = x - y
            m.sign *= -1;
            return *this - m;
        }
        // 1) Aumenta a mantissa para ganhar "folga" durante soma.
        auto a = this->extend();
        auto b = m.extend();

        
        // 2) Alinha os expoentes para somar digito a digito.
        a.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        b.shiftL(MANT - 1); // 0 . m1 m2 m3 0 0
        (a.exp < b.exp ? a : b).shiftR(abs(a.exp - b.exp)); //a.exp = b.exp
        
        // 3) Soma com carry.
        auto sum = a;
        int carry = 0;
        for (int i = a.mant.size() - 1; i >= 0; i--)
        {
            sum.mant[i] = a.mant[i] + b.mant[i] + carry;
            carry = sum.mant[i] / BASE;
            sum.mant[i] %= BASE;
        }

        assert(carry == 0);

        // 4) Normaliza resultado (remove zeros iniciais da mantissa).
        sum.fix();

        // 5) Volta para formato com MANT digitos.
        mfloat ans;
        ans.exp = sum.exp;
        ans.sign = sum.sign;

        for(int i=0; i<ans.mant.size(); i++)
            ans.mant[i] = sum.mant[i];

        // arredonda?

        if (ans.exp > EXPMAX) return makeInf(ans.sign);
        if (ans.exp < EXPMIN) return makeZero(ans.sign);
        return ans;
    }

    mfloat operator- (mfloat m) const {
        if(this->isNan() || m.isNan()) return makeNan();                                    // x - nan = nan
        if(this->isInf() && m.isInf() && this->sign == m.sign) return makeNan();            // inf - inf = nan
        if(this->isInf() && m.isInf()) return makeInf(this->sign);                          // +inf - -inf = +inf, -inf - +inf = -inf
        if(this->isInf() || m.isInf()) return this->isInf() ? *this : makeInf(-m.sign);     // x - +inf = -inf

        if(this->sign != m.sign){ // x - (-y) = x + y
            m.sign *= -1;
            return *this + m;
        }
        // 1) Aumenta a mantissa para ganhar "folga" durante subtracao.
        auto a = this->extend();
        auto b = m.extend();

        // 2) Alinha os expoentes para subtrair digito a digito.
        a.shiftL(MANT - 1);                                 // 0 . m1 m2 m3 0 0
        b.shiftL(MANT - 1);                                 // 0 . m1 m2 m3 0 0
        (a.exp < b.exp ? a : b).shiftR(abs(a.exp - b.exp)); // a.exp = b.exp
        
        if(a < b) swap(a, b);

        // 4) Subtracao com emprestimo.
        auto dif = a;
        int borrow = 0;
        for(int i = a.mant.size()-1; i >= 0; i--)
        {
            int cur = a.mant[i] - (b.mant[i] + borrow);
            borrow = 0;
            if(cur < 0)
            {
                borrow = (abs(cur) + BASE - 1) / BASE; // teto da divisão é o necessário pra ficar >= 0
                cur = (BASE + cur % BASE) % BASE;
            }
            dif.mant[i] = cur;
        }

        assert(borrow == 0);
        
        // 5) Normaliza resultado (remove zeros iniciais da mantissa).
        dif.fix();
        
        // 6) Volta para formato com MANT digitos.
        mfloat ans;
        ans.exp = dif.exp;
        ans.sign = a.sign;

        for (int i = 0; i < ans.mant.size(); i++)
            ans.mant[i] = dif.mant[i];

        // arredonda?

        if (ans.exp > EXPMAX) return makeInf(ans.sign);
        if (ans.exp < EXPMIN) return makeZero(ans.sign);
        return ans;
    }

    mfloat operator*(const mfloat m) const {
        if(this->isNan() || m.isNan()) return makeNan();                                          // x * nan = nan
        if(this->isInf() && m.isZero() || this->isZero() && m.isInf()) return makeNan();          // 0 * inf = nan
        if(this->isInf() || m.isInf()) return makeInf((this->sign < 0) ^ (m.sign < 0) ? -1 : +1); // inf * inf = inf, sinal pode mudar

        // 1) Aumenta a mantissa para reduzir perda durante produto.
        auto a = this->extend();
        auto b = m.extend();

        // 2) Mantem a mesma convencao usada em + e - (0 . m1 m2 ...).
        a.shiftL(MANT - 1);
        b.shiftL(MANT - 1);

        const int N = (int)a.mant.size(); // N = 2 * MANT

        // 2) Produto das mantissas por convolucao (estilo multiplicacao longa).
        vector<long long> raw(2 * N, 0);

        for (int i = N - 1; i >= 0; i--)
            for (int j = N - 1; j >= 0; j--)
                raw[i + j + 1] += 1LL * a.mant[i] * b.mant[j];

        for (int i = 2 * N - 1; i > 0; i--)
        {
            raw[i - 1] += raw[i] / BASE;
            raw[i] %= BASE;
        }

        // 4) Localiza primeiro digito significativo do produto.
        int first = 0;
        while (first < 2 * N && raw[first] == 0)
            first++;
        if (first == 2 * N) return makeZero( ((this->sign < 0) ^ (m.sign < 0)) ? -1 : +1);
        
        // 5) Monta resultado intermediario com 2*MANT digitos e expoente ajustado.
        _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> prod;
        prod.mant.fill(0);
        for (int i = 0; i < N; i++)
        {
            int idx = first + i;
            if (idx < 2 * N)
                prod.mant[i] = (int)raw[idx];
        }

        // Ajuste de escala apos truncar os digitos menos significativos.
        prod.exp = a.exp + b.exp - first + 1;
        prod.sign = ((this->sign < 0) ^ (m.sign < 0)) ? -1 : +1;
        prod.fix();
        
        // 6) Volta para formato com MANT digitos.
        mfloat ans;
        ans.exp = prod.exp;
        ans.sign = prod.sign;
        for (int i = 0; i < MANT; i++)
            ans.mant[i] = prod.mant[i];

        // arredonda?

        if (ans.exp > EXPMAX) return makeInf(ans.sign);
        if (ans.exp < EXPMIN) return makeZero(ans.sign);
        return ans;
    }

    mfloat operator/ (const mfloat m) const {
        if (this->isZero() && m.isZero()) return makeNan();                                // 0 / 0 = nan
        if (this->isNan() || m.isNan())   return makeNan();                                // x / nan = nan
        if (this->isInf() && m.isInf())   return makeNan();                                // inf / inf = nan
        if (this->isInf() && m.isZero())  return *this;                                    // inf / 0 = inf
        if (this->isInf()) return makeInf(((this->sign < 0) ^ (m.sign < 0)) ? -1 : 1);     // inf / x = inf
        if (m.isZero())    return makeInf(this->sign);                                     // x / 0 = inf
        if (m.isInf())     return makeZero();                                              // x / inf = 0

        // 1) Aumenta a mantissa para reduzir perda.
        auto a = this->extend();
        auto b = m.extend();
        const int N = (int)a.mant.size(); // N = 2 * MANT

        a.fix(), b.fix();
        auto &am = a.mant, &bm = b.mant;


        // 2) divisão clássica 
        vector<int> raw(N, 0);
        for(int i = 0; i < N; i++){
            int div = 0;

            while(am >= bm && div+1 < BASE){
                div++;
                for(int j = N-1; j >= 0; j--){
                    am[j] -= bm[j];
                    if(am[j] < 0){
                        am[j] += BASE;
                        if(j) am[j-1]--;
                    }
                }
            }

            raw[i] = div;
            for(int j = N-1; j > 0; j--) bm[j] = bm[j-1];
            bm[0] = 0;
        }

        // 5) Monta resultado intermediario
        _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> quoc;
        for (int i = 0; i < N; i++) quoc.mant[i] = raw[i];

        quoc.exp = a.exp - b.exp;
        quoc.sign = ((this->sign < 0) ^ (m.sign < 0)) ? -1 : +1;
        quoc.fix();

        // 6) Volta para formato com MANT digitos.
        mfloat ans;
        ans.exp = quoc.exp;
        ans.sign = quoc.sign;
        for (int i = 0; i < MANT; i++) ans.mant[i] = quoc.mant[i];

        // arredonda?

        if (ans.exp > EXPMAX) return makeInf(ans.sign);
        if (ans.exp < EXPMIN) return makeZero(ans.sign);
        return ans;
    }

    mfloat& operator+= (const mfloat m){ return *this = *this + m; }
    mfloat& operator-= (const mfloat m){ return *this = *this - m; }
    mfloat& operator*= (const mfloat m){ return *this = *this * m; }
    mfloat& operator/= (const mfloat m){ return *this = *this / m; }

    bool operator==(const mfloat m) const { return this->sign == m.sign && this->exp == m.exp && this->mant == m.mant; }
    bool operator< (const mfloat m) const { return tie(this->sign, this->exp, this->mant) < tie(m.sign, m.exp, m.mant); }
    bool operator<=(const mfloat m) const { return  (*this <  m) || (*this == m); }
    bool operator> (const mfloat m) const { return !(*this <= m); }
    bool operator>=(const mfloat m) const { return !(*this <  m); }
    bool operator!=(const mfloat m) const { return !(*this == m); }

    ld toDouble() const{
        if(this->isInf()) return sign > 0 ? numeric_limits<ld>::infinity() : -numeric_limits<ld>::infinity();
        if(this->isNan()) return numeric_limits<ld>::quiet_NaN();
        ld ans = 0;

        for(int i=(int)mant.size()-1; i>=0; i--) ans = ans / BASE + mant[i];

        for(int i=exp; i > 0; i--) ans *= BASE;
        for(int i=exp; i < 0; i++) ans /= BASE;

        if(sign < 0) ans *= -1;

        return ans;
    }

    void printReal() const { cout << (*this) << endl; } //depreciado, use cout << mx << endl;

    friend ostream& operator<<(ostream& os, const mfloat& p){
        if(p.isNan()) return os << "nan";
        if(p.sign < 0) os << "-";
        if(p.isInf()) return os << "inf";
        
        os << p.mant[0] << ".";
        
        for (int i = 1; i < MANT; i++) os << (BASE > 10 ? " " : "") << p.mant[i];

        return os << " * " << BASE << "^" << p.exp;
    }

    array<int, MANT> mant;
    int exp = 0;
    short sign = 0;

    // Desloca mantissa em relacao ao ponto e compensa no expoente.
    void shiftL(int sh)
    {
        auto nwMant = mant;
        nwMant.fill(0);

        for (int i = 0; i < mant.size(); i++)
            if (i + sh >= 0 && i + sh < mant.size())
            {
                nwMant[i] = mant[i + sh];
            }

        mant = nwMant;
        exp -= sh;
    }
    void shiftR(int sh) { shiftL(-sh); }

    // Normaliza removendo zeros a esquerda da mantissa.
    void fix(){
        int dlt = 0;
        while (dlt < mant.size() && mant[dlt] == 0)
            dlt++;

        if (dlt == mant.size())
            exp = EXPMIN; // 0.00
        else
            shiftL(min(dlt, exp - EXPMIN));
    }

    // Retorna um mfloat com o dobro da mantissa para calculo intermediario.
    _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> extend() const {
        _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> ans;

        // 0 . 0 0 m1 m2 m3
        for (int i = 0; i < mant.size(); i++)
            ans.mant[i + MANT] = mant[i];
        ans.exp = exp + MANT;
        ans.sign = sign;

        return ans;
    }

    bool isInf() const { return abs(sign) == 2; }
    bool isNan() const { return sign == 0; }
    bool isZero() const {
        if (isInf() || isNan()) return false;
        for (auto d : mant) if (d != 0) return false;
        return true;
    }

    static mfloat makeNan(){
        mfloat x;
        x.sign = 0;
        return x;
    }
    static mfloat makeInf(short sgn){
        mfloat x;
        x.sign = (sgn < 0 ? -2 : 2);
        return x;
    }
    static mfloat makeZero(short sgn = 1){
        mfloat x;
        x.sign = sgn;
        x.mant.fill(0);
        x.exp = EXPMIN;
        return x;
    }

};

#endif