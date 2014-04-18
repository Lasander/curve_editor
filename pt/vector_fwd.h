#pragma once
#ifndef PT_VECTOR_FWD_H
#define PT_VECTOR_FWD_H

namespace pt {

template<typename T, int dim>
class vector;

/// 2 dimension float vector. \relatesalso vector
typedef vector<float,2> vector2f;
/// 3 dimension float vector. \relatesalso vector
typedef vector<float,3> vector3f;
/// 4 dimension float vector. \relatesalso vector
typedef vector<float,4> vector4f;

/// 2 dimension int vector. \relatesalso vector
typedef vector<int,2> vector2i;
/// 3 dimension int vector. \relatesalso vector
typedef vector<int,3> vector3i;
/// 4 dimension int vector. \relatesalso vector
typedef vector<int,4> vector4i;

/// 2 dimension double vector. \relatesalso vector
typedef vector<double,2> vector2d;
/// 3 dimension double vector. \relatesalso vector
typedef vector<double,3> vector3d;
/// 4 dimension double vector. \relatesalso vector
typedef vector<double,4> vector4d;

} // namespace pt

#endif
