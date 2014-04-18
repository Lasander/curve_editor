#ifndef PT_MATH_DATA_SET_H
#define PT_MATH_DATA_SET_H

//#include "../log.h"
#include "../vector.h"
//#include "../boost/assert.h"
#include "../std/vector.h"


namespace pt { namespace math {

/// Array of data of type T used in splines.
/// This defines they keys for all curves. Each point in time is mapped with
/// a value.
template<typename T>
class data_set
{
public: // type definitions
    /// Point in time with a value of type T.
    class point
    {
    public:
        point();
        point(float time_, T value_);
        float time() const
        {
            return m_time;
        }
        T value() const
        {
            return m_value;
        }
    private: // data members
        float m_time;
        T m_value;
    };
private:
    typedef std::vector<point> container;
public:
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef std::pair<const_iterator,const_iterator> point_pair;
    typedef T result_type;

public:
    iterator add(float time, T value);
    iterator add(point const& point);

    void reserve(size_t size)
    {
        m_points.reserve(size);
    }

    const_iterator begin() const
    {
        return m_points.begin();
    }
    iterator begin()
    {
        return m_points.begin();
    }
    const_iterator end() const
    {
        return m_points.end();
    }
    iterator end()
    {
        return m_points.end();
    }
    size_t size() const
    {
        return m_points.size();
    }
    point const& at(size_t index) const
    {
        return m_points[index];
    }
    //point& at(size_t index)
    //{
    //    return m_points[index];
    //}

private: // data members
    std::vector<point> m_points;
};

template<class DataSet>
typename DataSet::const_iterator get_optional_endpoint(float time,
    DataSet const& data);

template<class DataSet>
typename DataSet::point_pair get_interval(float time, DataSet const& data);

}} // namespace pt::math

#include "data_set.inl"

#endif
