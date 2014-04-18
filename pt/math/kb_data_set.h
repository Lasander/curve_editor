#ifndef PT_MATH_KB_DATA_SET_H
#define PT_MATH_KB_DATA_SET_H

#include "data_set.h"
//#include "pt/boost/assert.h"

namespace pt { namespace math {

struct kochanek_bartels_parameters
{
    kochanek_bartels_parameters(float tension_, float bias_, float continuity_)
    :   tension(tension_)
    ,   bias(bias_)
    ,   continuity(continuity_)
    {
    }
    float tension;
    float bias;
    float continuity;
};

template<typename T>
class kb_data_set
{
public: // type definitions
    class point
    {
    public:
        point();
        point(float time, T value);
        point(float time, T value, kochanek_bartels_parameters const& parameters);
        point(float time, T value, kochanek_bartels_parameters const& parameters, T starting_tangent, T ending_tangent);
        
        float time() const
        {
            return m_time;
        }
        T value() const
        {
            return m_value;
        }
        T starting_tangent() const
        {
            return m_starting_tangent;
        }
        T ending_tangent() const
        {
            return m_ending_tangent;
        }
        void set_starting_tangent(T value)
        {
            m_starting_tangent = value;
        }
        void set_ending_tangent(T value)
        {
            m_ending_tangent = value;
        }
        kochanek_bartels_parameters const& parameters() const
        {
            return m_parameters;
        }
    private: // data members
        float m_time;
        T m_value;
        kochanek_bartels_parameters m_parameters;
        T m_starting_tangent;
        T m_ending_tangent;
    };
private:
    typedef std::vector<point> container;
public:
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef std::pair<const_iterator,const_iterator> point_pair;
    typedef T result_type;

public:
    kb_data_set();
    kb_data_set(data_set<T> const& values, std::vector<kochanek_bartels_parameters> const& parameters);
    kb_data_set const& operator=(kb_data_set const& other)
    {
        m_points = other.m_points;
        return *this;
    }

    const_iterator optional_endpoint(float time) const;
    point_pair points_at(float time) const;

    iterator add(typename data_set<T>::point const& data);
    iterator add(typename data_set<T>::point const& data, kochanek_bartels_parameters const& param);
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);
    
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
    point& at(size_t index)
    {
        return m_points[index];
    }
    const_iterator get(size_t index) const
    {
        return std::next(begin(), index);
    }
    
private: // private helpers
    void update_first_point(iterator point, const_iterator next);
    void update_last_point(const_iterator prev, iterator point);
    void update_point(const_iterator prev, iterator point, const_iterator next);
    void update(iterator point);
    
	iterator add_point(point const& point);
    
private: // data members
    std::vector<point> m_points;
};

// .inl
    
template<typename T>
inline kb_data_set<T>::point::point()
:   m_time(0)
,   m_value(static_cast<T>(0))
,   m_parameters(0, 0, 0)
,   m_starting_tangent(static_cast<T>(0))
,   m_ending_tangent(static_cast<T>(0))
{
}
    
template<typename T>
inline kb_data_set<T>::point::point(float time, T value)
:   m_time(time)
,   m_value(value)
,   m_parameters(0, 0, 0)
,   m_starting_tangent(static_cast<T>(0))
,   m_ending_tangent(static_cast<T>(0))
{
}
    
template<typename T>
inline kb_data_set<T>::point::point(float time, T value,
                                    kochanek_bartels_parameters const& parameters)
:   m_time(time)
,   m_value(value)
,   m_parameters(parameters)
{
}
    
template<typename T>
inline kb_data_set<T>::point::point(float time, T value,
                                    kochanek_bartels_parameters const& parameters,
                                    T starting_tangent, T ending_tangent)
:   m_time(time)
,   m_value(value)
,   m_parameters(parameters)
,   m_starting_tangent(starting_tangent)
,   m_ending_tangent(ending_tangent)
{
}
    
inline float starting_tangent_param1(kochanek_bartels_parameters const& p)
{
    return ((1.0f - p.tension) * (1.0f + p.bias) * (1.0f - p.continuity))
        / 2.0f;
}

inline float starting_tangent_param2(kochanek_bartels_parameters const& p)
{
    return ((1.0f - p.tension) * (1.0f - p.bias) * (1.0f + p.continuity))
        / 2.0f;
}

inline float ending_tangent_param1(kochanek_bartels_parameters const& p)
{
    return ((1.0f - p.tension) * (1.0f + p.bias) * (1.0f + p.continuity))
        / 2.0f;
}

inline float ending_tangent_param2(kochanek_bartels_parameters const& p)
{
    return ((1.0f - p.tension) * (1.0f - p.bias) * (1.0f - p.continuity))
        / 2.0f;
}

template<typename T>
inline std::pair<T, T> calculate_tangents(T const& prev_value_delta,
                                          T const& next_value_delta,
                                          kochanek_bartels_parameters const& param)
{
    T starting_tangent
    	= starting_tangent_param1(param) * prev_value_delta
    	+ starting_tangent_param2(param) * next_value_delta;
    
    T ending_tangent
    	= ending_tangent_param1(param) * prev_value_delta
    	+ ending_tangent_param2(param) * next_value_delta;
    
    return std::make_pair(starting_tangent, ending_tangent);
}
    

template<typename T>
inline void kb_data_set<T>::update_first_point(typename kb_data_set<T>::iterator point,
                                               typename kb_data_set<T>::const_iterator next)
{
    // Special case for first point.
	T next_value_delta = next->value() - point->value();
    
    std::pair<T, T> tangents = calculate_tangents(next_value_delta, next_value_delta, point->parameters());
    
    // Update point
    point->set_starting_tangent(tangents.first);
    point->set_ending_tangent(tangents.second);
	return;
}
    
template<typename T>
inline void kb_data_set<T>::update_last_point(typename kb_data_set<T>::const_iterator prev,
                                              typename kb_data_set<T>::iterator point)
{
    // Special case for last point.
    T prev_value_delta = point->value() - prev->value();
    
    std::pair<T, T> tangents = calculate_tangents(prev_value_delta, prev_value_delta, point->parameters());
    
    // Update point
    point->set_starting_tangent(tangents.first);
    point->set_ending_tangent(tangents.second);
    return;
}
    
    
template<typename T>
inline void kb_data_set<T>::update_point(typename kb_data_set<T>::const_iterator prev,
                                         typename kb_data_set<T>::iterator point,
                                         typename kb_data_set<T>::const_iterator next)
{
    T prev_value_delta = point->value() - prev->value();
    T next_value_delta = next->value() - point->value();
    
    std::pair<T, T> tangents = calculate_tangents(prev_value_delta, next_value_delta, point->parameters());
    
    // Speed adjustment.
    float prev_time_delta = point->time() - prev->time();
    float next_time_delta = next->time() - point->time();
    float t_total = prev_time_delta + next_time_delta;
    float starting_coeff = (2.0f * next_time_delta) / t_total;
    float ending_coeff = (2.0f * prev_time_delta) / t_total;
    
    // Update point
    point->set_starting_tangent(tangents.first * starting_coeff);
    point->set_ending_tangent(tangents.second * ending_coeff);
    return;
}

template<typename T>
inline void kb_data_set<T>::update(typename kb_data_set<T>::iterator point)
{
    if (point == m_points.end())
        return;
    
    const_iterator next = point + 1;
    
    if (point == m_points.begin())
    {
        if (next == m_points.end())
            return; // point is the only point
        
        return update_first_point(point, next);
    }
   
    const_iterator prev = point - 1;
    
    if (next == m_points.end())
        return update_last_point(prev, point);

    return update_point(prev, point, next);
}
    
    
template<typename T>
inline typename kb_data_set<T>::point create_point(
    typename data_set<T>::point const& cur, T prev, T next,
    kochanek_bartels_parameters const& param)
{
    std::pair<T, T> tangents = calculate_tangents(prev, next, param);

    return kb_data_set<T>::point(cur.time(), cur.value(), param,
        tangents.first, tangents.second);
}

template<typename T>
inline kb_data_set<T>::kb_data_set()
{
}

template<typename T>
inline kb_data_set<T>::kb_data_set(data_set<T> const& values,
    std::vector<kochanek_bartels_parameters> const& parameters)
{
    assert(values.size() == parameters.size());

    auto data = values.begin();
    auto param = parameters.begin();
	
    // Add points (tangents not calculated)
    for (; data != values.end(); ++data, ++param)
    {
		point p(data->time(), data->value(), *param);
        iterator it = add_point(p);
        assert(it != m_points.end());
    }

    // Update points (tangents)
    for (iterator i = m_points.begin(); i != m_points.end(); ++i)
    	update(i);
}

template<typename T>
inline typename kb_data_set<T>::const_iterator
    kb_data_set<T>::optional_endpoint(float time) const
{
    assert(!m_points.empty());
    return get_optional_endpoint(time, m_points);
}

template<typename T>
inline typename kb_data_set<T>::point_pair kb_data_set<T>::points_at(
    float time) const
{
    assert(!m_points.empty());
    return get_interval(time, *this);
}

    
template<typename T>
typename kb_data_set<T>::iterator kb_data_set<T>::add(typename data_set<T>::point const& data)
{
    kochanek_bartels_parameters param(0, 0, 0);
    return add(data, param);
}
    
template<typename T>
typename kb_data_set<T>::iterator kb_data_set<T>::add(typename data_set<T>::point const& data,
                                                      kochanek_bartels_parameters const& param)
{
    point p(data.time(), data.value(), param);
   
	iterator cur = add_point(p);
    if (cur == m_points.end())
    	return cur; // failed to add point

    iterator next = cur + 1;
    if (cur == m_points.begin() && next == m_points.end())
        return cur; // cur is the only point
    
	if (cur != m_points.begin())
    	update(cur - 1);
        
    update(cur);
    
	if (next != m_points.end())
    	update(next);
    
    return cur;
}
    
template<typename T>
typename kb_data_set<T>::iterator kb_data_set<T>::add_point(point const& point)
{
    iterator i = m_points.begin();
    while(i != m_points.end() && i->time() < point.time())
        ++i;

    // no keys with same time
    if (i != m_points.end() && i->time() == point.time())
    {
//        log("Warning: Ignoring spline key with equal time.");
        return m_points.end();
    }

    return m_points.insert(i, point);
}

    
template<typename T>
typename kb_data_set<T>::iterator kb_data_set<T>::erase(kb_data_set<T>::const_iterator pos)
{
    if (pos == m_points.end())
        return m_points.end();
    
    iterator next = m_points.erase(pos);
    
    if (next != m_points.begin())
    	update(next - 1);
    
    if (next != m_points.end())
    	update(next);
    
	return next;
}

template<typename T>
typename kb_data_set<T>::iterator kb_data_set<T>::erase(kb_data_set<T>::const_iterator first,
                                                        kb_data_set<T>::const_iterator last)
{
    if (first == m_points.end())
        return m_points.end();
    
    iterator next = m_points.erase(first, last);
    
    if (next != m_points.begin())
        update(next - 1);
    
    if (next != m_points.end())
        update(next);
    
    return next;
}
    
}} // namespace pt::math

#endif
