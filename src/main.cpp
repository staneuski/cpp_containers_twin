#include "matrix/matrix.h"

#include <cassert>
#include <experimental/iterator>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
void PrintMatrix(int ny, int nx, const T* m) {
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) std::cout << m[x + nx * y] << ' ';
        std::cout << '\n';
    }
}

int main() {
    using namespace std;
    using namespace cstl;

    const Shape s = {2, 3};
    const vector<int> f = {0, 1, 2, 3, 4, 5};

    Matrix<int> m(s);
    PrintMatrix(s.rows, s.cols, f.data());
    m.Transpose(f.begin(), f.end());
    m[0][0] = -1;

    PrintMatrix(m.GetShape().rows, m.GetShape().cols, m.GetData()); // -1, 3, 1, 4, 2, 5
    for (size_t i = 0; i < m.T().GetShape().rows; ++i) {
        cout << m.T()[i][0] << endl; // -1 1
    }

    return 0;
}