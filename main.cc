#include <cstdlib> 
#include <cstdio>
#include <math.h>
#include <iostream> 
#include <iomanip> 

using namespace std;

template<typename T>
class Vec3 {
public:

    Vec3() : x(T(0)), y(T(0)), z(T(0)) {
    }

    Vec3(const T &xx) : x(xx), y(xx), z(xx) {
    }

    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {
    }
    T x, y, z;

    T length() {
        return sqrt(x * x + y * y + z * z);
    }

    T dot(const Vec3<T> &v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3<T> cross(const Vec3<T> &v) const {
        return Vec3<T>(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x);
    }

    Vec3<T> operator+(const Vec3<T> &v) const {
        return Vec3<T>(x + v.x, y + v.y, z + v.z);
    }

    Vec3<T> operator-(const Vec3<T> &v) const {
        return Vec3<T>(x - v.x, y - v.y, z - v.z);
    }

    Vec3<T> operator*(const T &r) const {
        return Vec3<T>(x * r, y * r, z * r);
    }

    Vec3<T>& normalize() {
        T len = length();
        if (len > 0) {
            T invLen = 1 / len;
            x *= invLen, y *= invLen, z *= invLen;
        }
        return *this;
    }
};

template<typename T>
class Matrix44 {
public:

    Matrix44() {
    }
    //Access Operators to be able to access the matrix directly using bracket notation

    const T* operator[](uint8_t i) const {
        return m[i];
    }

    T* operator[](uint8_t i) {
        return m[i];
    }
    //Initialize the matrix using coefficients of the identity matrix
    T m[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    Matrix44 operator*(const Matrix44& rhs) const {
        Matrix44 mult;
        for (uint8_t i = 0; i < 4; i++) {
            for (uint8_t j = 0; j < 4; j++) {
                mult[i][j] = m[i][0] * rhs[0][j] +
                        m[i][1] * rhs[1][j] +
                        m[i][2] * rhs[2][j] +
                        m[i][3] * rhs[3][j];
            }
        }
    }

    Matrix44 transpose() const {
        Matrix44 transpMap;
        for (uint8_t i = 0; i < 4; i++) {
            for (uint8_t j = 0; j < 4; j++) {
                transpMap[i][j] = m[j][i];
            }
        }
        return transpMap;
    }

    Matrix44 inverse() {
        int i, j, k;
        Matrix44 s;
        Matrix44 t(*this);

        // Forward elimination
        for (i = 0; i < 3; i++) {
            int pivot = i;

            T pivotsize = t[i][i];

            if (pivotsize < 0)
                pivotsize = -pivotsize;

            for (j = i + 1; j < 4; j++) {
                T tmp = t[j][i];

                if (tmp < 0)
                    tmp = -tmp;

                if (tmp > pivotsize) {
                    pivot = j;
                    pivotsize = tmp;
                }
            }

            if (pivotsize == 0) {
                // Cannot invert singular matrix
                return Matrix44();
            }

            if (pivot != i) {
                for (j = 0; j < 4; j++) {
                    T tmp;

                    tmp = t[i][j];
                    t[i][j] = t[pivot][j];
                    t[pivot][j] = tmp;

                    tmp = s[i][j];
                    s[i][j] = s[pivot][j];
                    s[pivot][j] = tmp;
                }
            }

            for (j = i + 1; j < 4; j++) {
                T f = t[j][i] / t[i][i];

                for (k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        // Backward substitution
        for (i = 3; i >= 0; --i) {
            T f;

            if ((f = t[i][i]) == 0) {
                // Cannot invert singular matrix
                return Matrix44();
            }

            for (j = 0; j < 4; j++) {
                t[i][j] /= f;
                s[i][j] /= f;
            }

            for (j = 0; j < i; j++) {
                f = t[j][i];

                for (k = 0; k < 4; k++) {
                    t[j][k] -= f * t[i][k];
                    s[j][k] -= f * s[i][k];
                }
            }
        }

        return s;
    }

    void multVecMatrix(const Vec3<T> &src, Vec3<T> &dst) const {
        dst.x = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0] + m[3][0];
        dst.y = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1] + m[3][1];
        dst.z = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2] + m[3][2];
        T w = src.x * m[0][3] + src.y * m[1][3] + src.z * m[2][3] + m[3][3];
        if (w != 1 && w != 0) {
            dst.x = dst.x / w;
            dst.y = dst.y / w;
            dst.z = dst.z / w;
        }
    }

    void multDirMatrix(const Vec3<T> &src, Vec3<T> &dst) const {
        dst.x = src.x * m[0][0] + src.y * m[1][0] + src.z * m[2][0];
        dst.y = src.x * m[0][1] + src.y * m[1][1] + src.z * m[2][1];
        dst.z = src.x * m[0][2] + src.y * m[1][2] + src.z * m[2][2];
    }
};
typedef Matrix44<float> Matrix44f; 
int main() {
    Matrix44f m1;
    m1[0][0] = 0.707107;
    m1[0][1] = 0;
    m1[0][2] = -0.707107;
    m1[0][3] = 0;
    m1[1][0] = -0.331295;
    m1[1][1] = 0.883452;
    m1[1][2] = -0.331295;
    m1[1][3] = 0;
    m1[2][0] = 0.624695;
    m1[2][1] = 0.468521;
    m1[2][2] = 0.624695;
    m1[2][3] = 0;
    m1[3][0] = 4.000574;
    m1[3][1] = 3.00043;
    m1[3][2] = 4.000574;
    m1[3][3] = 1;

    Matrix44f f2 = m1.inverse();
    return 0;
}