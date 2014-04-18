//
//  Curve.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 23.12.13.
//  Copyright (c) 2013 Lasse Lopperi. All rights reserved.
//

#ifndef __CurveEditor__Curve__
#define __CurveEditor__Curve__

#include "pt/math/kb_spline.h"

template<typename T>
class Curve
{
public:
    /** Curve point handle */
    struct Point
    {
    public:
        float time() const
        {
            return m_data.time();
        }
        T value() const
        {
            return m_data.value();
        }
        bool is_valid() const
        {
            return m_is_valid;
        }
        pt::math::kochanek_bartels_parameters const& parameters() const
        {
            return m_data.parameters();
        }
        
	private:
        // Allow curve to create and update points
        friend class Curve<T>;

        Point() :
        	m_is_valid(false), m_data()
        {
        }
        
        explicit Point(typename pt::math::kb_data_set<T>::point const& data) :
        	m_is_valid(true), m_data(data)
        {
        }
        
        void update_data(typename pt::math::kb_data_set<T>::point const& data)
        {
            m_data = data;
        }
        
        bool m_is_valid;
        typename pt::math::kb_data_set<T>::point m_data;
    };

public:
	Curve();
    ~Curve();
    
    int numberOfPoints() const;
    const Point point(int index_) const;
    
    const Point add(float time_, T value_);
    const Point update(Point const& point_, float time_, T value_);
    void remove(Point const& point_);
    
	T value_at(float time_) const;
    
private:
    typename pt::math::kb_data_set<T>::const_iterator find_point(Point const& point_);
    
    pt::math::kb_spline<T, 1> m_spline;
};

template<typename T>
inline Curve<T>::Curve()
: m_spline(pt::math::kb_data_set<T>())
{
}

template<typename T>
inline Curve<T>::~Curve()
{
}

template<typename T>
inline int Curve<T>::numberOfPoints() const
{
    return static_cast<int>(m_spline.data().size());
}

template<typename T>
inline const typename Curve<T>::Point Curve<T>::point(int index_) const
{
    auto point_iter = m_spline.data().get(static_cast<size_t>(index_));
    if (point_iter == m_spline.data().end())
        return Point();
    
    return Point(*point_iter);
}

template<typename T>
inline const typename Curve<T>::Point Curve<T>::add(float time_, T value_)
{
    typename pt::math::data_set<T>::point p(time_, value_);
    auto point_iter = m_spline.data().add(p);
    if (point_iter == m_spline.data().end())
        return Point();
    
    return Point(*point_iter);
}

template<typename T>
inline const typename Curve<T>::Point Curve<T>::update(Point const& point_, float time_, T value_)
{
    if (!point_.is_valid())
        return Point();
    
    auto point_iter = find_point(point_);
    assert(point_iter != m_spline.data().end());
    
    m_spline.data().erase(point_iter);
    return add(time_, value_);
}

template<typename T>
inline void Curve<T>::remove(Point const& point_)
{
    if (!point_.is_valid())
        return;
    
    auto point_iter = find_point(point_);
    assert(point_iter != m_spline.data().end());
    
    m_spline.data().erase(point_iter);
}

template<typename T>
inline T Curve<T>::value_at(float time_) const
{
    return m_spline.value_at(time_);
}

template<typename T>
inline typename pt::math::kb_data_set<T>::const_iterator Curve<T>::find_point(Point const& point_)
{
    auto points = m_spline.data().points_at(point_.time());
    auto cur = points.first;
    auto next = points.second;
    if (next != m_spline.data().end())
        return m_spline.data().end();
    
    return cur;
}


#endif /* defined(__Curves__Curve__) */
