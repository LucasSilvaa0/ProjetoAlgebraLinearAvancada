#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include "../../../projeto1/mfloat.cpp"
using namespace std;

using mfloat = _mfloat<2, 64, 124, -124>;

using mfloat20 = _mfloat<10, 20, 20, -20>;
using mfloat15 = _mfloat<10, 15, 20, -20>;
using mfloat10 = _mfloat<10, 10, 20, -20>;
using mfloat5  = _mfloat<10, 5,  20, -20>;
using mfloat3  = _mfloat<10, 3,  20, -20>;
using mfloat2  = _mfloat<10, 2,  20, -20>;


static bool readLine(string &out);
static bool readDouble(double &out);
static vector<double> parseDoubles(const string &line);
static string toJsonArray(const vector<double> &values);
static void normalizeRows(vector<vector<double>> &matrix);


template<typename MFloat>
struct LinearSystem {
    vector<vector<MFloat>> A;
    vector<MFloat> b;
};

// Helper to extract system Ax = b from the transition matrix for steady-state calculation
template<typename MFloat>
LinearSystem<MFloat> extractLinearSystem(const vector<vector<MFloat>>& matrix) {
    int n = matrix.size();
    if (n == 0) return { {}, {} };

    vector<vector<MFloat>> P(n, vector<MFloat>(n));
    
    for (int i = 0; i < n; i++) {
        MFloat rowSum = MFloat::makeZero();
        for (int j = 0; j < n; j++) rowSum += matrix[i][j];
        
        for (int j = 0; j < n; j++) {
            P[i][j] = rowSum.isZero() ? MFloat(0) : matrix[i][j] / rowSum;
        }
    }

    vector<vector<MFloat>> A(n, vector<MFloat>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = P[j][i];
            if (i == j) A[i][j] -= MFloat(1);
        }
    }

    vector<MFloat> b(n, MFloat::makeZero());
    int lastIndex = n - 1;
    for (int j = 0; j < n; j++) A[lastIndex][j] = MFloat(1);
    b[lastIndex] = MFloat(1);

    return { A, b };
}

// Jacobi Solver
template<typename MFloat>
vector<MFloat> solveJacobi(const vector<vector<MFloat>>& A, const vector<MFloat>& b, int iterations = 100) {
    int n = A.size();
    vector<MFloat> x(n, MFloat(1) / MFloat(n));

    for (int k = 0; k < iterations; k++) {
        vector<MFloat> xNew(n);
        for (int i = 0; i < n; i++) {
            MFloat sum = MFloat::makeZero();
            for (int j = 0; j < n; j++) {
                if (i != j) sum += A[i][j] * x[j];
            }
            xNew[i] = (b[i] - sum) / A[i][i];
        }
        x = xNew;
    }
    return x;
}

// Gauss-Seidel Solver
template<typename MFloat>
vector<MFloat> solveGaussSeidel(const vector<vector<MFloat>>& A, const vector<MFloat>& b, int iterations = 100) {
    int n = A.size();
    vector<MFloat> x(n, MFloat(1) / MFloat(n));

    for (int k = 0; k < iterations; k++) {
        for (int i = 0; i < n; i++) {
            MFloat sum = MFloat::makeZero();
            for (int j = 0; j < n; j++) {
                if (i != j) sum += A[i][j] * x[j];
            }
            x[i] = (b[i] - sum) / A[i][i];
        }
    }
    return x;
}

template<typename MFloat>
vector<vector<MFloat>> parseMatrix(const vector<vector<double>>& input) {
    int n = input.size();
    vector<vector<MFloat>> matrix(n, vector<MFloat>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = MFloat(input[i][j]);
        }
    }
    return matrix;
}

template<typename MFloat>
vector<double> parseDistribution(const vector<MFloat>& dist) {
    vector<double> result(dist.size());
    for (size_t i = 0; i < dist.size(); i++) {
        result[i] = dist[i].toDouble();
    }
    return result;
}

template<typename MFloat>
void solverGeral(string solverMode, const vector<vector<double>>& transitionMatrix, vector<double> &distribution) {
    auto mTransitionMatrix = parseMatrix<MFloat>(transitionMatrix);
    auto system = extractLinearSystem(mTransitionMatrix);

    if(solverMode == "jacobi") distribution = parseDistribution(solveJacobi(system.A, system.b));
    else 
    if(solverMode == "gauss-seidel") distribution = parseDistribution(solveGaussSeidel(system.A, system.b));
    else 
    distribution = vector<double>(system.A.size(), 0.0);
    // throw invalid_argument("Invalid solver mode " + solverMode);
}

static void to_lower(string &s) {
    for (char &c : s) c = tolower(c);
}

//////////////////////////////////////

int main() {
    string operation;
    if (!readLine(operation)) return 1;
    to_lower(operation);
    // accepted modes: jacobi, gauss-seidel

    int n = 0, t = 0;
    {
        string sizeLine;
        if (!readLine(sizeLine)) return 2;
        istringstream sizeStream(sizeLine);
        sizeStream >> n >> t;
        if (n <= 0 || t < 0) return 3;
    }

    string precision = "default";
    readLine(precision);
    to_lower(precision);
    // Accepted precisions: default, mfloat20, mfloat15, mfloat10, mfloat5, mfloat3, mfloat2


    vector<vector<double>> transitionMatrix(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if (!readDouble(transitionMatrix[i][j])) return 5;
        }    
    }    

    normalizeRows(transitionMatrix);    
    vector<double> distribution(n);

    if(precision == "default" || precision == "mfloat")  solverGeral<mfloat>  (operation, transitionMatrix, distribution);
    else
    if(precision == "mfloat20") solverGeral<mfloat20>(operation, transitionMatrix, distribution);
    else 
    if(precision == "mfloat15") solverGeral<mfloat15>(operation, transitionMatrix, distribution);
    else
    if(precision == "mfloat10") solverGeral<mfloat10>(operation, transitionMatrix, distribution);
    else
    if(precision == "mfloat5")  solverGeral<mfloat5> (operation, transitionMatrix, distribution);
    else
    if(precision == "mfloat3")  solverGeral<mfloat3> (operation, transitionMatrix, distribution);
    else
    if(precision == "mfloat2")  solverGeral<mfloat2> (operation, transitionMatrix, distribution);
    else 
        distribution = vector<double>(n, 1.0/0.0);

    cout << "{\"distribution\":" << toJsonArray(distribution) << "}\n";
    return 0;
}

static bool readLine(string &out) {
    if (!getline(cin, out)) return false;
    while (!out.empty() && (out.back() == '\r' || out.back() == '\n')) out.pop_back();
    return true;
}

static bool readDouble(double &out) {
    cin >> out;
    if (cin.fail()) return false;
    return true;
}

static vector<double> parseDoubles(const string &line) {
    istringstream input(line);
    vector<double> values;
    double val;
    while (input >> val) values.push_back(val);
    return values;
}

static string toJsonArray(const vector<double> &values) {
    ostringstream out;
    out << "[";
    out << fixed << setprecision(15);
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) out << ",";
        out << values[i];
    }
    out << "]";
    return out.str();
}

static void normalizeRows(vector<vector<double>> &matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        double rowSum = 0.0;
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            rowSum += matrix[i][j];
        }

        if (rowSum == 0.0) continue;

        for (size_t j = 0; j < matrix[i].size(); ++j) {
            matrix[i][j] /= rowSum;
        }
    }
}
