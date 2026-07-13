

#include <iomanip>
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <cassert>
#include <random>
#include <Windows.h>

#include "Math.h"

namespace Lgt {

Matrix::Matrix(size_t rows, size_t cols) {
    _rows     = rows;
    _cols     = cols;
    _elements = (float*)malloc(rows * cols * sizeof(float));
}

float& Matrix::at(size_t i, size_t j) {
    LGT_ASSERT_MSG(i < _rows, "Row index out of bounds");
    LGT_ASSERT_MSG(j < _cols, "Column index out of bounds");
    return _elements[i * _cols + j];
}

const float& Matrix::at(size_t i, size_t j) const {
    LGT_ASSERT(i < _rows);
    LGT_ASSERT(j < _cols);
    return _elements[i * _cols + j];
}

void Matrix::fill(float val) {
    for (size_t i = 0; i < _rows; ++i) {
        for (size_t j = 0; j < _cols; ++j) {
            at(i, j) = val;
        }
    }
}

void Matrix::print(bool full) const {
    if (_title)
        std::cout << "Matrix " << _title << " (" << _rows << "x" << _cols << ")\n";
    else
        std::cout << "Matrix (" << _rows << "x" << _cols << ")\n";

    if (!full) {
        std::cout << "  (truncated output)\n\n";
        return;
    }

    for (size_t i = 0; i < _rows; ++i) {
        for (size_t j = 0; j < _cols; ++j) {
            float val = at(i, j);
            if (std::isnan(val) || std::abs(val) > 1e6f)
                printf("[%1.2e] ", val);
            else
                printf("%1.4f ", val);
        }
        printf("\n");
    }
    printf("\n");
}

} // namespace Lgt