#ifndef PT_MATH_CUBIC_HERMITE_SPLINE_H
#define PT_MATH_CUBIC_HERMITE_SPLINE_H

#include "../vector.h"

namespace pt { namespace math {

template<typename ResultType>
class spline_base
{
public:
    virtual ~spline_base() {}

public:
    virtual ResultType value_at(float time) const = 0;
};

template<typename DataSet, int C>
class cubic_hermite_spline :
    public spline_base<pt::vector<typename DataSet::result_type,C> >
{
public:
    typedef typename DataSet::result_type element_type;
    typedef pt::vector<element_type,C> result_type;

public:
    cubic_hermite_spline(DataSet const data[C]);
    result_type value_at(float time) const;

private: // data members
    DataSet m_data[C];
};

template<typename DataSet>
class cubic_hermite_spline<DataSet,1> :
    public spline_base<typename DataSet::result_type>
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
