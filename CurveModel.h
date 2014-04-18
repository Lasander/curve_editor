//
//  CurveModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#ifndef __CurveEditor__CurveModel__
#define __CurveEditor__CurveModel__

#include "RangeF.h"
#include <QObject>
#include <QMultiMap>
#include <QDebug>

/** Unique identification for a point within a CurveModel */
using PointId = int32_t;

/** 
 CurveModel represents a potentially multi-dimensional curve as its control points. 
 Each point has an unique time and therefore the points can be ordered.
 
 Derived from QObject for signals and slots.
 */
class CurveModel : public QObject
{
    Q_OBJECT
    
public:
    /** A single control point within the curve */
    class Point
    {
    public:
        float time() const;
        float value(int index) const;
        QList<float> values() const;
        
        float tension() const;
        float bias() const;
        float continuity() const;

        bool is_valid() const;
        PointId id() const;
        
	private:
        // Contruct invalid point
        Point();

        // Contruct valid point, optionally with an existing id
        Point(float time, QList<float> values, PointId id = 0);
        Point(float time, QList<float> values, float tension, float bias, float continuity, PointId id = 0);
        void updateParams(float tension, float bias, float continuity);

        bool operator==(Point const& other) const;
        bool operator!=(Point const& other) const;
        
        static PointId generateId();

        // Allow curve to create points
        friend class CurveModel;
        
    private:
        bool m_is_valid;
        PointId m_id;
        float m_time;
        QList<float> m_values;
        
        float m_tension;
        float m_bias;
        float m_continuity;
    };
    
public:
    CurveModel(int dimension);
    ~CurveModel();
	   
    QList<PointId> pointIds() const;
    
	const Point point(PointId id) const;
	int numberOfPoints() const;
	int dimension() const;
	
    RangeF timeRange() const;
    
signals:
    void pointAdded(PointId id);
    void pointUpdated(PointId id);
    void pointRemoved(PointId id);
    void timeRangeChanged(RangeF range);
    
public slots:
    void addPoint(float time, QList<float> values);
    void addPoint(float time, QList<float> values, float tension, float bias, float continuity);
    void updatePoint(PointId id, float time, float value, int index);
    void updatePointParams(PointId id, float tension, float bias, float continuity);
    void removePoint(PointId id);
    
private:
    using PointContainer = QMultiMap<float, Point>;
    using Iterator = PointContainer::Iterator;
    using ConstIterator = PointContainer::ConstIterator;
    
    Iterator findPoint(PointId id);
    ConstIterator findPoint(PointId id) const;
    float limitValueToScale(float value) const;
    QList<float> limitValuesToScale(QList<float> value) const;
    
    PointContainer m_points;
	int m_dimension;
    
    RangeF m_verticalValueClamp;
    RangeF m_verticalValueScale;
};

inline float CurveModel::Point::time() const
{
    return m_time;
}
inline float CurveModel::Point::value(int index) const
{
    return m_values.value(index);
}
inline QList<float> CurveModel::Point::values() const
{
    return m_values;
}

inline float CurveModel::Point::tension() const
{
    return m_tension;
}
inline float CurveModel::Point::bias() const
{
    return m_bias;
}
inline float CurveModel::Point::continuity() const
{
    return m_continuity;
}

inline bool CurveModel::Point::is_valid() const
{
    return m_is_valid;
}

inline PointId CurveModel::Point::id() const
{
    return m_id;
}

inline bool CurveModel::Point::operator==(Point const& other) const
{
    return
    (other.is_valid() == is_valid()) &&
    (other.id() == id()) &&
    (other.time() == time()) &&
    (other.values() == values());
}

inline bool CurveModel::Point::operator!=(Point const& other) const
{
    return !operator==(other);
}


#endif /* defined(__CurveEditor__CurveModel__) */
