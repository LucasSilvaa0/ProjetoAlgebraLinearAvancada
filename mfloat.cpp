#ifndef LUZHEADER
#define LUZHEADER
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




        // Reconstrucao da mantissa por Horner reverso.
        for (int i = (int)mant.size() - 1; i >= 0; i--)
            ans = ans / BASE + mant[i];

        // Aplica o expoente armazenado.
        for (int i = exp; i > 0; i--)
            ans *= BASE;
        for (int i = exp; i < 0; i++)
            ans /= BASE;

        return ans;
    }

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
        while (exp > EXPMIN && ((int)x % BASE == 0 && (int)x / BASE == 0))
            exp--, x *= BASE;
        while (exp < EXPMAX && (int)x / BASE >= 1)
            exp++, x /= BASE;

        // "Quebra" x em digitos da mantissa na base escolhida.
        for (auto &m : mant){
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

        return ans;
    }

    mfloat operator- (mfloat m) const {
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

        return ans;
    }

    mfloat operator*(const mfloat m) const
    {
        // 1) Aumenta a mantissa para reduzir perda durante produto.
        auto a = this->extend();
        auto b = m.extend();

        // 2) Mantem a mesma convencao usada em + e - (0 . m1 m2 ...).
        a.shiftL(MANT - 1);
        b.shiftL(MANT - 1);

        const int N = (int)a.mant.size(); // N = 2 * MANT

        // 3) Produto das mantissas por convolucao (estilo multiplicacao longa).
        vector<long long> raw(2 * N, 0);
        for (int i = N - 1; i >= 0; i--)
        {
            for (int j = N - 1; j >= 0; j--)
            {
                raw[i + j + 1] += 1LL * a.mant[i] * b.mant[j];
            }
        }

        for (int i = 2 * N - 1; i > 0; i--)
        {
            raw[i - 1] += raw[i] / BASE;
            raw[i] %= BASE;
        }

        // 4) Localiza primeiro digito significativo do produto.
        int first = 0;
        while (first < 2 * N && raw[first] == 0)
            first++;

        mfloat ans;
        if (first == 2 * N)
        {
            // Produto nulo.
            ans.exp = EXPMIN;
            ans.mant.fill(0);
            ans.sign = 0;
            return ans;
        }

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
        prod.sign = ((this->sign < 0) ^ (m.sign < 0)) ? -1 : 0;
        prod.fix();

        // 6) Volta para formato com MANT digitos.
        ans.exp = prod.exp;
        ans.sign = prod.sign;
        for (int i = 0; i < MANT; i++)
            ans.mant[i] = prod.mant[i];

        // arredonda?

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
    void fix()
    {

        int dlt = 0;
        while (dlt < mant.size() && mant[dlt] == 0)
            dlt++;

        if (dlt == mant.size())
            exp = EXPMIN; // 0.00
        else
            shiftL(dlt);
    }

    // Retorna um mfloat com o dobro da mantissa para calculo intermediario.
    _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> extend() const
    {
        _mfloat<BASE, MANT * 2, EXPMAX, EXPMIN> ans;

        // 0 . 0 0 m1 m2 m3
        for (int i = 0; i < mant.size(); i++)
            ans.mant[i + MANT] = mant[i];
        ans.exp = exp + MANT;
        ans.sign = sign;

        return ans;
    }
};

#endif