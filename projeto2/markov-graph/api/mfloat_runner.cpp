#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "../../../projeto1/mfloat.cpp"

using mfloat = _mfloat<10, 1, 2, -2>;

static bool readLine(std::string &out) {
    if (!std::getline(std::cin, out)) return false;
    while (!out.empty() && (out.back() == '\r' || out.back() == '\n')) out.pop_back();
    return true;
}

static std::vector<double> parseDoubles(const std::string &line) {
    std::istringstream input(line);
    std::vector<double> values;
    double val;
    while (input >> val) values.push_back(val);
    return values;
}

static std::string toJsonArray(const std::vector<double> &values) {
    std::ostringstream out;
    out << "[";
    out << std::fixed << std::setprecision(15);
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) out << ",";
        out << values[i];
    }
    out << "]";
    return out.str();
}

int main() {
    std::string operation;
    if (!readLine(operation)) return 1;

    std::string sizeLine;
    if (!readLine(sizeLine)) return 2;
    std::istringstream sizeStream(sizeLine);
    int n = 0;
    int t = 0;
    sizeStream >> n >> t;
    if (n <= 0 || t < 0) return 3;

    std::string initialMode;
    if (!readLine(initialMode)) return 4;

    std::vector<double> initialVec;
    int initialIndex = -1;
    if (initialMode == "index") {
        std::string indexLine;
        if (!readLine(indexLine)) return 5;
        initialIndex = std::stoi(indexLine);
    } else if (initialMode == "vector") {
        std::string vectorLine;
        if (!readLine(vectorLine)) return 6;
        initialVec = parseDoubles(vectorLine);
        if ((int)initialVec.size() != n) return 7;
    }

    std::string matrixLine;
    if (!readLine(matrixLine)) return 8;
    std::vector<double> matrixValues = parseDoubles(matrixLine);
    if ((int)matrixValues.size() != n * n) return 9;

    std::vector<mfloat> matrix;
    matrix.reserve(n * n);
    for (double value : matrixValues) matrix.emplace_back(value);

    auto normalizeRow = [&](int rowIndex) {
        mfloat sum = mfloat::makeZero();
        for (int j = 0; j < n; ++j) {
            sum += matrix[rowIndex * n + j];
        }
        for (int j = 0; j < n; ++j) {
            if (sum.isZero())
                matrix[rowIndex * n + j] = mfloat::makeZero();
            else
                matrix[rowIndex * n + j] /= sum;
        }
    };

    for (int i = 0; i < n; ++i) normalizeRow(i);

    std::vector<mfloat> pi(n, mfloat::makeZero());
    if (operation == "distribution") {
        if (initialMode == "index") {
            if (initialIndex < 0 || initialIndex >= n) return 10;
            pi[initialIndex] = mfloat(1.0L);
        } else if (initialMode == "vector") {
            mfloat sum = mfloat::makeZero();
            for (int i = 0; i < n; ++i) {
                pi[i] = mfloat(initialVec[i]);
                sum += pi[i];
            }
            if (!sum.isZero()) {
                for (int i = 0; i < n; ++i) pi[i] /= sum;
            }
        } else {
            for (int i = 0; i < n; ++i) pi[i] = mfloat(1.0L / n);
        }
    } else if (operation == "steady") {
        for (int i = 0; i < n; ++i) pi[i] = mfloat(1.0L / n);
    } else {
        return 11;
    }

    int maxIterations = operation == "steady" ? t : t;
    for (int step = 0; step < maxIterations; ++step) {
        std::vector<mfloat> nextPi(n, mfloat::makeZero());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                nextPi[j] += pi[i] * matrix[i * n + j];
            }
        }
        mfloat sum = mfloat::makeZero();
        for (int i = 0; i < n; ++i) sum += nextPi[i];
        if (!sum.isZero()) {
            for (int i = 0; i < n; ++i) nextPi[i] /= sum;
        }
        pi.swap(nextPi);
    }

    if (operation == "steady") {
        // One extra iteration to use the same steady-state formula semantics.
    }

    std::vector<double> distribution(n);
    for (int i = 0; i < n; ++i) distribution[i] = static_cast<double>(pi[i].toDouble());

    std::cout << "{\"distribution\":" << toJsonArray(distribution) << "}\n";
    return 0;
}
