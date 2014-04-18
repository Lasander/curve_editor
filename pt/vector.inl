#ifndef PT_VECTOR_INL
#define PT_VECTOR_INL

namespace pt {

// vector_base

template<typename T, int dim>
inline T const& vector_base<T,dim>::operator[](size_t index) const
{
    assert(index < dim);
    return m_value[index];
}

template<typename T, int dim>
inline T& vector_base<T,dim>::operator[](size_t index)
{
    assert(index < dim);
    return m_value[index];
}

template<typename T, int dim>
inline vector_base<T,dim> const& vector_base<T,dim>::operator+=(
    vector_base const& other)
{
    T const* source = other.data();

    for (size_t i = 0; i < dim; ++i)
        m_value[i] += source[i];

    return (*this);
}

template<typename T, int dim>
inline vector_base<T,dim> const& vector_base<T,dim>::operator-=(
    vector_base const& other)
{
    T const* source = other.data();

    for (size_t i = 0; i < dim; ++i)
        m_value[i] -= source[i];

    return (*this);
}

template<typename T, int dim>
inline vector_base<T,dim> const& vector_base<T,dim>::operator*=(T other)
{
    for (size_t i = 0; i < dim; ++i)
        m_value[i] *= other;

    return (*this);
}

template<typename T, int dim>
inline vector_base<T,dim> const& vector_base<T,dim>::operator/=(T other)
{
    for (size_t i = 0; i < dim; ++i)
        m_value[i] /= other;

    return (*this);
}

// vector

// vector<T,2>

template<typename T>
vector<T,2>::vector()
{
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
}

template<typename T>
vector<T,2>::vector(enum vector_base<T,2>::uninitialized)
{
}

template<typename T>
vector<T,2>::vector(vector<T,2> const& other)
{
    ((*this))[0] = other[0];
    ((*this))[1] = other[1];
}

template<typename T>
vector<T,2>::vector(T val1, T val2)
{
    ((*this))[0] = val1;
    ((*this))[1] = val2;
}

// vector<T,3>

template<typename T>
vector<T,3>::vector()
{
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
}

template<typename T>
vector<T,3>::vector(enum vector_base<T,3>::uninitialized)
{
}

template<typename T>
vector<T,3>::vector(vector<T,2> const& other)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = static_cast<T>(0);
}

template<typename T>
vector<T,3>::vector(vector<T,3> const& other)
{
    T const* source = other.data();
    T* destination = this->data();
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

template<typename T>
vector<T,3>::vector(vector<T,2> const& other, T val3)
{
    ((*this))[0] = other[0];
    ((*this))[1] = other[1];
    ((*this))[2] = val3;
}

template<typename T>
vector<T,3>::vector(T val1, T val2, T val3)
{
    (*this)[0] = val1;
    (*this)[1] = val2;
    (*this)[2] = val3;
}

// vector<T, 4>

template<typename T>
vector<T,4>::vector()
{
    (*this)[0] = static_cast<T>(0);
    (*this)[1] = static_cast<T>(0);
    (*this)[2] = static_cast<T>(0);
    (*this)[3] = static_cast<T>(0);
}

template<typename T>
vector<T,4>::vector(enum vector_base<T,4>::uninitialized)
{
}

template<typename T>
vector<T,4>::vector(vector<T,2> const& other)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = static_cast<T>(0);
    (*this)[3] = static_cast<T>(0);
}

template<typename T>
vector<T,4>::vector(vector<T,3> const& other)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = other[2];
    (*this)[3] = static_cast<T>(0);
}

template<typename T>
vector<T,4>::vector(vector<T,4> const& other)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = other[2];
    (*this)[3] = other[3];
}

template<typename T>
vector<T,4>::vector(vector<T,2> const& other, T val3, T val4)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = val3;
    (*this)[3] = val4;
}

template<typename T>
vector<T,4>::vector(vector<T,3> const& other, T val4)
{
    (*this)[0] = other[0];
    (*this)[1] = other[1];
    (*this)[2] = other[2];
    (*this)[3] = val4;
}

template<typename T>
vector<T,4>::vector(T val1, T val2, T val3, T val4)
{
    (*this)[0] = val1;
    (*this)[1] = val2;
    (*this)[2] = val3;
    (*this)[3] = val4;
}

// global templates

template<typename T, int dim>
inline T dot(vector<T,dim> const& a, vector<T,dim> const& b)
{
	T result = static_cast<T>(0);

    T const* a_ptr = a.data();
    T const* b_ptr = b.data();

	for (size_t i = 0; i < dim; ++i)
		result += a_ptr[i] * b_ptr[i];

	return result;
}

template<typename T>
inline vector<T,3> cross(vector<T,2> const& a, vector<T,2> const& b)
{
	T det_ab = a[0] * b[1] - b[0] * a[1]; // determinant for 2x2
	return vector3f(det_ab, 0, 0);
}

template<typename T>
inline vector<T,3> cross(vector<T,3> const& a, vector<T,3> const& b)
{
    return vector<T,3>(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]);
}

template<typename T, int dim>
inline vector<T,dim> operator*(vector<T,dim> const& v, T const& scalar)
{
    vector<T,dim> result(vector<T,dim>::uninitialized);

    T const* v_ptr = v.data();
    T* result_ptr = result.data();

	for (size_t i = 0; i < dim; ++i)
        result_ptr[i] = v_ptr[i] * scalar;

    return result;
}

template<typename T, int dim>
inline vector<T,dim> operator*(T const& scalar, vector<T,dim> const& v)
{
    return v * scalar;
}

template<typename T, int dim, typename U>
inline vector<U,dim> operator/(vector<T,dim> const& v, U const& scalar)
{
    vector<U,dim> result(vector<U,dim>::uninitialized);

	for (size_t i = 0; i < dim; ++i)
        result[i] = static_cast<U>(v[i]) / scalar;

    return result;
}

//template<typename T, int dim>
//inline vector<T,dim> operator/(vector<T,dim> const& v, T const& scalar)
//{
//    vector<T,dim> result(vector<T,dim>::uninitialized);
//
//	for (size_t i = 0; i < dim; ++i)
//        result[i] = v[i] / scalar;
//
//    return result;
//}

template<typename T, int dim>
inline vector<T,dim> operator-(vector<T,dim> const& v)
{
    vector<T,dim> result(vector<T,dim>::uninitialized);

	for (size_t i = 0; i < dim; ++i)
        result[i] = -v[i];

    return result;
}

template<typename T, int dim, typename U>
vector<T,dim> operator+(vector<T,dim> const& a, vector<U,dim> const& b)
{
    vector<T,dim> result(vector<T,dim>::uninitialized);

	for (size_t i = 0; i < dim; ++i)
        result[i] = a[i] + static_cast<T>(b[i]);

    return result;
}

template<typename T, int dim, typename U>
vector<T,dim> operator-(vector<T,dim> const& a, vector<U,dim> const& b)
{
    vector<T,dim> result(vector<T,dim>::uninitialized);

	for (size_t i = 0; i < dim; ++i)
        result[i] = a[i] - static_cast<T>(b[i]);

    return result;
}

template<typename T, int dim>
inline bool operator==(vector<T,dim> const& a, vector<T,dim> const& b)
{
    for (size_t i = 0; i < dim; ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

template<typename T, int dim>
inline bool operator!=(vector<T,dim> const& a, vector<T,dim> const& b)
{
    return !(a == b);
}

template<typename T, int dim>
inline T distance(vector<T,dim> const& a, vector<T,dim> const& b)
{
    return length(a - b);
}

template<typename T, int dim>
inline T squared_distance(vector<T,dim> const& a, vector<T,dim> const& b)
{
    return squared_length(a - b);
}

template<typename T, int dim>
inline T squared_length(vector<T,dim> const& v)
{
    T result = 0;

    for (size_t i = 0; i < dim; ++i)
        result += v[i] * v[i];

    return result;
}

template<typename T, int dim>
inline T length(vector<T,dim> const& v)
{
    return ::sqrt(squared_length(v));
}

template<typename T, int dim>
inline vector<T,dim> abs(vector<T,dim> const& v)
{
    vector<T,dim> result(vector<T,dim>::uninitialized);
    // pretty useless? abs all axis to positive
    for (size_t i = 0; i < dim; ++i)
        result[i] = ::abs(v[i]);
    return result;
}

template<typename T, int dim>
inline size_t dominant_axis(vector<T,dim> const& v)
{
    size_t axis = 0;
    for (size_t i = 1; i < dim; ++i)
    {
        if (abs(v[i]) > ::abs(v[axis]))
            axis = i;
    }
    return axis;
}

template<typename T, int dim>
inline size_t minimum_axis(vector<T,dim> const& v)
{
    size_t axis = 0;
    for (size_t i = 1; i < dim; ++i)
    {
        if (abs(v[i]) < ::abs(v[axis]))
            axis = i;
    }
    return axis;
}

template<typename T, int dim>
inline vector<T, dim> normalize(vector<T,dim> const& v)
{
	T const len = length(v);
    return v / len;
}

template<typename T, int dim>
inline vector<T,dim> project(vector<T,dim> const& u, vector<T,dim> const& v)
{
    return dot(u,v) / dot(v,v) * v;
}

template<typename T, int dim>
inline vector<T,dim> denormalize(vector<T,dim> const& v)
{
    vector<T,dim> result(v);
    denormalize_in_place(result);
    return result;
}

template<typename T, int dim>
inline void denormalize_in_place(vector<T,dim>& v)
{
    for (size_t i = 0; i < dim; ++i)
        v[i] = denormalize(v[i]);
}

template<typename U, typename T, int dim>
vector<U,dim> static_vector_cast(vector<T,dim> const& v)
{
    vector<U,dim> result(vector<U,dim>::uninitialized);
    for (size_t i = 0; i < dim; ++i)
        result[i] = static_cast<U>(v[i]);

    return result;
}

} // namespace pt

#endif
