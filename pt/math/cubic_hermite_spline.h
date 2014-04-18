#ifndef PT_MATH_CUBIC_HERMITE_SPLINE_H
#define PT_MATH_CUBIC_HERMITE_SPLINE_H

namespace pt { namespace math {

template<typename DataSet>
class cubic_hermite_spline
{
public:
    typedef typename DataSet::result_type result_type;

public:
    cubic_hermite_spline(DataSet const& data);
    cubic_hermite_spline const& operator=(cubic_hermite_spline const& other)
    {
        m_data = other.m_data;
        return *this;
    }
    result_type value_at(float time) const;
    DataSet& data()
    {
    	return m_data;
    }
    const DataSet& data() const
    {
    	return m_data;
    }
    
private: // data members
    DataSet m_data;
};

}} // namespace pt::math

#include "cubic_hermite_spline.inl"

#endif
