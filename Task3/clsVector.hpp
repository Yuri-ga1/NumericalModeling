#pragma once
#include <iostream>
#include <string>
#include <type_traits>
using namespace std;

template <typename T>
class clsVector
{
public:
    T x, y, z;
    clsVector(T x_ = 0, T y_ = 0, T z_ = 0) : x(x_), y(y_), z(z_) {}

    // Constructor for converting between types
    template <typename U>
     explicit clsVector(const clsVector<U> &other)
        : x(static_cast<T>(other.x)),
          y(static_cast<T>(other.y)),
          z(static_cast<T>(other.z)) {}

    // Assignment between types
    template <typename U>
     clsVector &operator=(const clsVector<U> &other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        z = static_cast<T>(other.z);
        return *this;
    }

    T at(int i) {
        switch (i) {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                return 0;
        }
    }
    template <typename U>
    void set(int i, U val) {
        switch (i) {
            case 0:
                x = static_cast<T>(val);
                break;
            case 1:
                y = static_cast<T>(val);
                break;
            case 2:
                z = static_cast<T>(val);
                break;
            default:
                break;
        }
    }

    // Arithmetic operators
    template <typename U>
     auto operator+(const clsVector<U> &other) const
    {
        using R = decltype(x + other.x);
        return clsVector<R>(x + other.x, y + other.y, z + other.z);
    }

    template <typename U>
     auto operator-(const clsVector<U> &other) const
    {
        using R = decltype(x - other.x);
        return clsVector<R>(x - other.x, y - other.y, z - other.z);
    }

    template <typename U>
     auto operator*(const clsVector<U> &other) const
    {
        using R = decltype(x * other.x);
        return clsVector<R>(x * other.x, y * other.y, z * other.z);
    }

    template <typename Scalar>
     auto operator*(Scalar s) const
    {
        using R = decltype(x * s);
        return clsVector<R>(x * s, y * s, z * s);
    }

    template <typename Scalar>
     auto operator/(Scalar s) const
    {
        using R = decltype(x / s);
        return clsVector<R>(x / s, y / s, z / s);
    }

	template <typename U>
     auto operator/(const clsVector<U> &other) const
    {
        using R = decltype(x / other.x);
        return clsVector<R>(x / other.x, y / other.y, z / other.z);
    }


// Dot product
    template <typename U>
     auto dot(const clsVector<U> &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    friend std::ostream &operator<<(std::ostream &os, const clsVector &v)
    {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    }

// Module (norm) 
    
    T abs() const
    {
        return sqrt(x * x + y * y + z * z);
    }

// Module (norm) squared
    
    T abs2() const
    {
        return x * x + y * y + z * z;
    }
};

// Vector product is an external function
template <typename T1, typename T2>
 auto cross(const clsVector<T1> &a, const clsVector<T2> &b)
{
    using R = decltype(a.x * b.y - a.y * b.x);
    return clsVector<R>(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

// Scalar * vector
template <typename T>
 auto operator*(double s, const clsVector<T> &v)
{
    using R = decltype(s * v.x);
    return clsVector<R>(s * v.x, s * v.y, s * v.z);
}


// Dot product is an outer function
template <typename T1, typename T2>
 auto dot(const clsVector<T1> &a, const clsVector<T2> &b)
{
    using ResultT = decltype(a.x * b.x); 
    return ResultT{a.x * b.x + a.y * b.y + a.z * b.z};
}

using vec = clsVector<double>;