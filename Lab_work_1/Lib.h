#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <windows.h>

using namespace std;

inline void setColor(int color)
{
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE),
        color
    );
}

class Polynomial
{
protected:
    int degree;
    double* coef;

public:

    // -------------------------
    // Конструктори
    // -------------------------

    Polynomial()
    {
        degree = 0;

        coef = new double[1];
        coef[0] = 0;
    }

    Polynomial(int deg)
    {
        degree = deg;

        coef = new double[degree + 1];

        for(int i = 0; i <= degree; i++)
            coef[i] = 0;
    }

    Polynomial(const Polynomial& other)
    {
        degree = other.degree;

        coef = new double[degree + 1];

        for(int i = 0; i <= degree; i++)
            coef[i] = other.coef[i];
    }

    virtual ~Polynomial()
    {
        delete[] coef;
    }

    // -------------------------
    // Оператор присвоювання
    // -------------------------

    Polynomial& operator=(const Polynomial& other)
    {
        if(this == &other)
            return *this;

        delete[] coef;

        degree = other.degree;

        coef = new double[degree + 1];

        for(int i = 0; i <= degree; i++)
            coef[i] = other.coef[i];

        return *this;
    }

    // -------------------------
    // Індексація
    // -------------------------

    double& operator[](int index)
    {
        return coef[index];
    }

    // -------------------------
    // Псевдозмінна
    // -------------------------

    class RangeProxy
    {
    private:
        Polynomial& poly;
        int left;
        int right;

    public:

        RangeProxy
        (
            Polynomial& p,
            int l,
            int r
        )
            : poly(p),
              left(l),
              right(r)
        {
        }

        void operator=(double value)
        {
            if(left < 0)
                left = 0;

            if(right > poly.degree)
                right = poly.degree;

            for(int i = left; i <= right; i++)
                poly.coef[i] = value;
        }
    };

    RangeProxy operator()(int left,int right)
    {
        return RangeProxy(*this,left,right);
    }

    // -------------------------
    // Додавання
    // -------------------------

    Polynomial operator+(const Polynomial& p) const
    {
        int maxDeg = max(degree,p.degree);

        Polynomial result(maxDeg);

        for(int i = 0; i <= maxDeg; i++)
        {
            double a =
                (i <= degree)
                ? coef[i]
                : 0;

            double b =
                (i <= p.degree)
                ? p.coef[i]
                : 0;

            result.coef[i] = a + b;
        }

        return result;
    }

    // -------------------------
    // Множення поліномів
    // -------------------------

    Polynomial operator*(const Polynomial& p) const
    {
        Polynomial result
        (
            degree + p.degree
        );

        for(int i = 0; i <= degree; i++)
        {
            for(int j = 0; j <= p.degree; j++)
            {
                result.coef[i + j] +=
                    coef[i] * p.coef[j];
            }
        }

        return result;
    }

    // -------------------------
    // Множення на константу
    // -------------------------

    Polynomial operator*(double k) const
    {
        Polynomial result(*this);

        for(int i = 0; i <= degree; i++)
        {
            result.coef[i] *= k;
        }

        return result;
    }

    // -------------------------
    // Піднесення до степеня
    // -------------------------

    Polynomial pow(int n) const
    {
        Polynomial result(0);

        result.coef[0] = 1;

        for(int i = 0; i < n; i++)
        {
            result = result * (*this);
        }

        return result;
    }

    // -------------------------
    // Ділення
    // -------------------------

    Polynomial operator/(const Polynomial& p) const
    {
        if(p.degree == 0 &&
           p.coef[0] == 0)
        {
            throw runtime_error(
                "Division by zero polynomial"
            );
        }

        Polynomial dividend(*this);

        Polynomial quotient
        (
            max(0,
                degree - p.degree)
        );

        while(dividend.degree >= p.degree)
        {
            int power =
                dividend.degree -
                p.degree;

            double factor =
                dividend.coef[
                    dividend.degree
                ]
                /
                p.coef[p.degree];

            quotient.coef[power] =
                factor;

            for(int i = 0;
                i <= p.degree;
                i++)
            {
                dividend.coef[
                    i + power
                ]
                -= factor *
                   p.coef[i];
            }

            while(dividend.degree > 0 &&
                  abs(
                    dividend.coef[
                        dividend.degree
                    ]) < 1e-9)
            {
                dividend.degree--;
            }
        }

        return quotient;
    }

    // -------------------------
    // Зберігання
    // -------------------------

    void saveToFile(const string& filename)
    {
        ofstream fout(filename);

        if(!fout)
        {
            setColor(12);
            cout << "ERROR: file not created!" << endl;
            setColor(7);
            return;
        }

        fout << degree << endl;

        for(int i = 0; i <= degree; i++)
            fout << coef[i] << " ";

        setColor(10);
        cout << "Saved to: " << filename << endl;
        setColor(7);
    }

    // -------------------------
    // Відкриття
    // -------------------------

    void loadFromFile(const string& filename)
    {
        ifstream fin(filename);

        if(!fin)
        {
            setColor(12);
            cout << "ERROR: file not found!" << endl;
            setColor(7);
            return;
        }

        delete[] coef;

        fin >> degree;

        coef = new double[degree + 1];

        for(int i = 0; i <= degree; i++)
            fin >> coef[i];

        setColor(10);
        cout << "Loaded successfully!" << endl;
        setColor(7);
    }

    // -------------------------
    // Виведення
    // -------------------------

    friend ostream&
    operator<<
    (
        ostream& out,
        const Polynomial& p
    )
    {
        bool first = true;

        for(int i = p.degree;
            i >= 0;
            i--)
        {
            if(abs(p.coef[i]) < 1e-9)
                continue;

            if(!first)
                out << " + ";

            out << p.coef[i];

            if(i > 0)
                out << "x";

            if(i > 1)
                out << "^" << i;

            first = false;
        }

        if(first)
            out << "0";

        return out;
    }

    // -------------------------
    // Введення
    // -------------------------

    friend istream&
    operator>>
    (
        istream& in,
        Polynomial& p
    )
    {
        cout
        << "Degree: ";

        in >> p.degree;

        delete[] p.coef;

        p.coef =
            new double[
                p.degree + 1
            ];

        cout
        << "Coefficients:\n";

        for(int i = 0;
            i <= p.degree;
            i++)
        {
            cout
            << "a[" << i
            << "] = ";

            in >> p.coef[i];
        }

        return in;
    }
};

// -----------------------------------
// Наслідування
// -----------------------------------

class NamedPolynomial
    : public Polynomial
{
private:
    string name;

public:

    NamedPolynomial
    (
        string n = "",
        int deg = 0
    )
        : Polynomial(deg)
    {
        name = n;
    }

    string getName() const
    {
        return name;
    }

    void setName
    (
        const string& n
    )
    {
        name = n;
    }
};