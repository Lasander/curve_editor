#pragma once
#ifndef PT_VECTOR_H
#define PT_VECTOR_H

#include "vector_fwd.h"

// vector.inl includes
#include <cassert>
#include <cmath>

namespace pt {

/// Base class for vectors.
template<typename T, int dim>
class vector_base
{
public:
    /// Enum for creating uninitialized vector.
    enum uninitialized{uninitialized};

    /// Element accessor. \return element by \a index.
	T const& operator[](size_t index) const;
    /// \overload
	T& operator[](size_t index);

    /// Add \a other vector to this vector.
    vector_base const& operator+=(vector_base const& other);

    /// Subtract \a other vector from this this vector.
	vector_base const& operator-=(vector_base const& other);

    /// Multiply this vector with scalar \a value.
	vector_base const& operator*=(T value);

    /// Divide this vector by scalar \a value.
	vector_base const& operator/=(T value);

    T const* data() const
    {
        return m_value;
    }

    T* data()
    {
        return m_value;
    }

private: // data members
	T m_value[dim];
};

/// Templated vector for any dimensions and types. Use predefined type
/// definitions for most common ones.
///
/// This class is designed for mathematical vector operations. For use of
/// dynamic vector (array), use std::vector.
///
/// Implicit conversions between different types is not supported, since
/// they may introduce hard to find bugs. Use static_vector_cast() for
/// converting between different types.
///
/// \code
/// typedef vector<float,2> vector2f;
/// typedef vector<float,3> vector3f;
/// typedef vector<float,4> vector4f;
/// typedef vector<int,2> vector2i;
/// typedef vector<int,3> vector3i;
/// typedef vector<int,4> vector4i;
/// typedef vector<double,2> vector2d;
/// typedef vector<double,3> vector3d;
/// typedef vector<double,4> vector4d;
/// 
/// vector3f my_vector(1.0f, 2.0f, 3.0f);
/// vector3f unit_vector = normalize(my_vector);
/// \endcode
///
template<typename T, int dim>
class vector : public vector_base<T,dim>
{
    /// \todo general template functions.
};

/// Specialized 2d vector template.
template<typename T>
class vector<T,2> : public vector_base<T,2>
{
public:
    vector();
    vector(enum vector_base<T,2>::uninitialized);
    vector(vector<T,2> const& other);
    //vector(vector<T,3> const& other);
    //vector(vector<T,4> const& other);
	vector(T val1, T val2);
};

/// Specialized 3d vector template.
template<typename T>
class vector<T,3> : public vector_base<T,3>
{
public:
    vector();
    vector(enum vector_base<T,3>::uninitialized);
    vector(vector<T,2> const& other);
    vector(vector<T,3> const& other);
    //vector(vector<T,4> const& other);
    vector(vector<T,2> const& other, T val3);
	vector(T val1, T val2, T val3);
};

/// Specialized 4d vector template.
template<typename T>
class vector<T,4> : public vector_base<T,4>
{
public:
    vector();
    vector(enum vector_base<T,4>::uninitialized);
    vector(vector<T,2> const& other);
    vector(vector<T,3> const& other);
    vector(vector<T,4> const& other);
	vector(vector<T,2> const& other, T val3, T val4);
	vector(vector<T,3> const& other, T val4);
	vector(T val1, T val2, T val3, T val4);
};

// global templates

/// Dot product. \return dot product of vectors \a a and \a b.
/// \relatesalso vector
template<typename T, int dim>
T dot(vector<T,dim> const& a, vector<T,dim> const& b);

/// Vector cross product. \return cross product of vectors \a a and \a b.
/// \relatesalso vector
template<typename T>
vector<T,3> cross(vector<T,2> const& a, vector<T,2> const& b);

/// Vector cross product. \return cross product of vectors \a a and \a b.
/// \relatesalso vector
template<typename T, int dim>
vector<T,3> cross(vector<T,dim> const& a, vector<T,dim> const& b);

/// Vector scalar multiply. \return \a v multiplied by \a scalar.
/// \relatesalso vector
template<typename T, int dim>
vector<T,dim> operator*(vector<T,dim> const& v, T const& scalar);

/// Vector scalar multiply. \return \a v multiplied by \a scalar.
/// \relatesalso vector
template<typename T, int dim>
vector<T,dim> operator*(T const& scalar, vector<T,dim> const& v);

/// Vector scalar division. \return \a v divided by \a scalar.
/// \relatesalso vector
template<typename T, int dim, typename U>
vector<U,dim> operator/(vector<T,dim> const& v, U const& scalar);
//template<typename T, int dim>
//vector<T,dim> operator/(vector<T,dim> const& v, T const& scalar);

/// Vector negation. \return Negated vector \a v. \relatesalso vector
template<typename T, int dim>
vector<T,dim> operator-(vector<T,dim> const& v);

/// Vector addition. \return Addition of vectors \a a and \a b. Returned
/// type is same as \a a. \relatesalso vector
template<typename T, int dim, typename U>
vector<T,dim> operator+(vector<T,dim> const& a, vector<U,dim> const& b);

/// Vector subtraction. \return Result of \a a subtracted by \a b. Returned
/// type is same as \a a. \relatesalso vector
template<typename T, int dim, typename U>
vector<T,dim> operator-(vector<T,dim> const& a, vector<T,dim> const& b);

/// Equality operator. \return \c true if vector \a a is equal with
/// vector \a b. \relatesalso vector
template<typename T, int dim>
bool operator==(vector<T,dim> const& a, vector<T,dim> const& b);
/// Unequality operator. \return \c true if vector \a is not equal with
/// vector \a b. \relatesalso vector
template<typename T, int dim>
bool operator!=(vector<T,dim> const& a, vector<T,dim> const& b);

/// Distance between vectors. \return Distance between vector \a a and vector
/// \a b. \relatesalso vector
template<typename T, int dim>
T distance(vector<T,dim> const& a, vector<T,dim> const& b);

/// Squared distance between vectors. \return Squared distance between vector
/// \a a and vector \a b. \relatesalso vector
template<typename T, int dim>
T squared_distance(vector<T,dim> const& a, vector<T,dim> const& b);

/// Vector length. \return length of vector \a v. \relatesalso vector
template<typename T, int dim>
T length(vector<T,dim> const& v);

/// Vector squared length. \return length() * length() of vector \a v.
/// \relatesalso vector
template<typename T, int dim>
T squared_length(vector<T,dim> const& v);

/// Vector element absolute values. \return vector \a v with all elements of
/// abs(). \relatesalso vector
template<typename T, int dim>
vector<T,dim> abs(vector<T,dim> const& v);

/// \return an index into element of largest absolute value of \a v.
/// \relatesalso vector
template<typename T, int dim>
size_t dominant_axis(vector<T,dim> const& v);

/// \return an index into element of smallest absolute value of \a v.
/// \relatesalso vector
template<typename T, int dim>
size_t minimum_axis(vector<T,dim> const& v);

/// Vector normalization. \return an unit length vector of \a v.
/// \relatesalso vector
template<typename T, int dim>
vector<T,dim> normalize(vector<T,dim> const& v);

/// Projects the vector \a u orthogonally onto the vector \a v
/// \relatesalso vector
template<typename T, int dim>
vector<T,dim> project(vector<T,dim> const& u, vector<T,dim> const& v);

/// Sets elements near -1, 0 or 1 values to -1, 0 or 1.
/// \relatesalso vector
template<typename T, int dim>
vector<T,dim> denormalize(vector<T,dim> const& v);
/// Sets elements near -1, 0 or 1 values to -1, 0 or 1.
/// \relatesalso vector
template<typename T, int dim>
void denormalize_in_place(vector<T,dim>& v);

/// Performs static_cast<U>() for vector elements.
template<typename U, typename T, int dim>
vector<U,dim> static_vector_cast(vector<T,dim> const& v);

// math utils overloads

/// math_utils average template function overload. \relatesalso vector
//template<>
//PT_INLINE vector3f average(vector3f const& a, vector3f const& b);
///// \overload
//template<>
//PT_INLINE vector2f average(vector2f const& a, vector2f const& b);

} // namespace pt

#include "vector.inl"

#endif
