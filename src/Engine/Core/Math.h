#pragma once
#include <iomanip>
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <cassert>
#include <random>
#include <Windows.h>
#include <cmath>

#include "Engine/Core/Logger.h"

#define PI            3.142f
#define TORADINS(deg) (deg / 180) * PI

namespace Lgt {
struct Matrix {
private:
    size_t      _rows     = 0;
    size_t      _cols     = 0;
    float*      _elements = nullptr;
    const char* _title    = nullptr;

public:
    Matrix(size_t rows, size_t cols);

    float&       at(size_t i, size_t j);
    void         print(bool full = true) const;
    const float& at(size_t i, size_t j) const;
    void         fill(float val);
    float*       data() { return _elements; }

    inline Matrix operator*(const Matrix& other) const {
        LGT_ASSERT_MSG(_cols == other._rows, "Matrix multiplication dimension mismatch");
        Matrix result(_rows, other._cols);
        for (size_t i = 0; i < _rows; ++i) {
            for (size_t j = 0; j < other._cols; ++j) {
                result.at(i, j) = 0;
                for (size_t k = 0; k < _cols; ++k)
                    result.at(i, j) += at(i, k) * other.at(k, j);
            }
        }
        return result;
    }

    inline Matrix operator+(const Matrix& other) const {
        LGT_ASSERT_MSG(_rows == other._rows && _cols == other._cols, "Matrix addition dimension mismatch");
        Matrix result(_rows, _cols);
        for (size_t i = 0; i < _rows; ++i) {
            for (size_t j = 0; j < _cols; ++j) {
                result.at(i, j) = at(i, j) + other.at(i, j);
            }
        }
        return result;
    }

    static inline Matrix Random(size_t rows, size_t cols) {
        std::random_device                    rd;
        std::mt19937                          eng(rd());
        std::uniform_real_distribution<float> distr(0.0f, 1.0f);

        Matrix result(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result.at(i, j) = distr(eng);
            }
        }
        return result;
    }

    static inline Matrix Zero(size_t rows, size_t cols) {
        Matrix result(rows, cols);
        result.fill(0.0f);
        return result;
    }

    static inline Matrix Identity(size_t rows, size_t cols) {
        LGT_ASSERT_MSG(rows == cols, "Identity matrix must be square");
        Matrix result(rows, cols);
        result.fill(0.0f);
        for (size_t i = 0; i < rows; ++i)
            result.at(i, i) = 1.0f;
        return result;
    }

    static inline Matrix Perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane) {
        Matrix result = Matrix::Zero(4, 4);

        float tanHalfFov = tanf(fovRadians * 0.5f);

        return result;
    }

    inline size_t      rows() const { return _rows; }
    inline size_t      cols() const { return _cols; }
    inline const char* name() const { return _title; }

    inline Matrix& setName(const char* title) {
        _title = title;
        return *this;
    }
};
} // namespace Lgt