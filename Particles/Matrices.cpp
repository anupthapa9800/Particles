#include "Matrices.h"
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace Matrices;
using std::cout;
using std::endl;

// Helper for float comparison (required for operator==)
bool almostEqual(double a, double b, double eps = 0.0001)
{
    return std::fabs(a - b) < eps;
}

// Base Matrix Constructor
Matrix::Matrix(int _rows, int _cols) : rows(_rows), cols(_cols)
{
    // Resize the outer vector (rows)
    a.resize(rows);
    // Resize each inner vector (columns) and initialize elements to 0.0
    for (int i = 0; i < rows; ++i) {
        a[i].resize(cols, 0.0);
    }
}

// Operator Overload for Matrix Addition: c = a + b
Matrix Matrices::operator+(const Matrix& a, const Matrix& b)
{
    if (a.getRows() != b.getRows() || a.getCols() != b.getCols()) {
        throw std::runtime_error("Error: Matrix addition requires matrices of the same dimensions.");
    }

    Matrix result(a.getRows(), a.getCols());
    for (int i = 0; i < a.getRows(); ++i) {
        for (int j = 0; j < a.getCols(); ++j) {
            result(i, j) = a(i, j) + b(i, j);
        }
    }
    return result;
}

// Operator Overload for Matrix Multiplication: c = a * b
Matrix Matrices::operator*(const Matrix& a, const Matrix& b)
{
    if (a.getCols() != b.getRows()) {
        throw std::runtime_error("Error: Matrix multiplication requires matrix A's columns to equal matrix B's rows.");
    }

    Matrix result(a.getRows(), b.getCols());
    for (int i = 0; i < a.getRows(); ++i) {
        for (int j = 0; j < b.getCols(); ++j) {
            double sum = 0.0;
            for (int k = 0; k < a.getCols(); ++k) {
                sum += a(i, k) * b(k, j);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

// Operator Overload for Matrix Equality: a == b
bool Matrices::operator==(const Matrix& a, const Matrix& b)
{
    // Check dimensions first
    if (a.getRows() != b.getRows() || a.getCols() != b.getCols()) {
        return false;
    }

    // Check elements using an almost equal comparison for doubles
    for (int i = 0; i < a.getRows(); ++i) {
        for (int j = 0; j < a.getCols(); ++j) {
            if (!almostEqual(a(i, j), b(i, j))) {
                return false;
            }
        }
    }
    return true;
}

// Operator Overload for Matrix Inequality: a != b
bool Matrices::operator!=(const Matrix& a, const Matrix& b)
{
    // Simply use the equality operator
    return !(a == b);
}

// Operator Overload for Output Stream: cout << a
std::ostream& Matrices::operator<<(std::ostream& os, const Matrix& a)
{
    os << std::fixed << std::setprecision(4); // Set precision for readability

    for (int i = 0; i < a.getRows(); ++i) {
        for (int j = 0; j < a.getCols(); ++j) {
            os << std::setw(10) << a(i, j); // Set width for column spacing
        }
        os << '\n'; // Separate rows by newline
    }
    return os;
}

// Transformation Matrix Implementations

// RotationMatrix Constructor (2x2)
RotationMatrix::RotationMatrix(double theta) : Matrix(2, 2)
{
    a[0][0] = std::cos(theta);
    a[0][1] = -std::sin(theta);
    a[1][0] = std::sin(theta);
    a[1][1] = std::cos(theta);
}

// ScalingMatrix Constructor (2x2)
ScalingMatrix::ScalingMatrix(double scale) : Matrix(2, 2)
{
    a[0][0] = scale;
    a[0][1] = 0.0;
    a[1][0] = 0.0;
    a[1][1] = scale;
}

// TranslationMatrix Constructor (2 x nCols)
TranslationMatrix::TranslationMatrix(double xShift, double yShift, int nCols)
    : Matrix(2, nCols)
{
    // Row 0 = xShift, Row 1 = yShift for all columns
    for (int j = 0; j < nCols; ++j) {
        a[0][j] = xShift;
        a[1][j] = yShift;
    }
}