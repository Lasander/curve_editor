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
 * CurveModel represents a (potentially multi-dimensional) curve as its control points.
 * Derived from QObject for signals and slots.
 */
class CurveModel : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief A single control point within the curve.
     *
     * Points are created, owned and controlled by a CurveModel instance.
     * Others can refer to the point using its id.
     */
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
        /** \brief Construct invalid point */
        Point();

        /** @brief Construct valid point, optionally with an existing id */
        Point(float time, QList<float> values, PointId id = 0);
        Point(float time, QList<float> values, float tension, float bias, float continuity, PointId id = 0);
        void updateParams(float tension, float bias, float continuity);

        bool operator==(Point const& other) const;
        bool operator!=(Point const& other) const;
        
        /** @brief Generator for unique point id's */
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
    /**
     * @brief Construct a CurveModel
     * @param dimension Number of values per point
     * @param name Curve name
     */
    CurveModel(int dimension, const QString& name);

    ~CurveModel();

    /** @return A list of point ids. */
    QList<PointId> pointIds() const;
    
    /**
     * @return Copy of a point with the given id. In case an invalid or
     * non-existent id is given, a invalid point is returned.
     */
    const Point point(PointId id) const;

    /** @return The number of point in the curve. */
    int numberOfPoints() const;

    /** @return The number of values in each point. */
    int dimension() const;
	
    /** @return Curve time range [start, end]. */
    RangeF timeRange() const;

    /** @return Curve value range [min, max]. */
    RangeF valueRange() const;

    bool isSelected() const;

    const QString& name() const;

signals:
    /** @brief A new point was added. */
    void pointAdded(PointId id);
    /** @brief Data for an existing point was modified. */
    void pointUpdated(PointId id);
    /** @brief An existing point was removed. */
    void pointRemoved(PointId id);
    /** @brief Curve time range changed. */
    void timeRangeChanged(RangeF newRange);
    /** @brief Curve value range changed. */
    void valueRangeChanged(RangeF newRange);

    void selectedChanged(bool status);

public slots:
    /**
     * @brief Add new point with given attributes.
     *
     * @param time Point time
     * @param values Array of values

     * @par Tension, bias and continuity parameters default to 0.
     * @par No point is added if number of value doesn't match the curve dimension
     */
    void addPoint(float time, QList<float> values);
    /**
     * @brief Add new point with given attributes.
     *
     * @param time Point time
     * @param values Array of values (number of value must match the curve dimension)
     * @param tension Tension parameter at the point
     * @param bias Bias parameter at the point
     * @param continuity Continuity parameter at the point
     *
     * @par No point is added if number of value doesn't match the curve dimension
     */
    void addPoint(float time, QList<float> values, float tension, float bias, float continuity);

    /**
     * @brief Update point time and/or value for one dimension of an existing point.
     *
     * @param id Point id
     * @param time New time
     * @param value New value
     * @param index The dimension index
     *
     * @par Altering the time will naturally affect the whole point (also other value dimension)
     * @par No modifications are made if the id is invalid or if the index is not a valid value dimension
     */
    void updatePoint(PointId id, float time, float value, int index);
    /**
     * @brief Update parameters for an existing point
     *
     * @param id Point id
     * @param tension New tension
     * @param bias New bias
     * @param continuity New continuity
     *
     * @par No modifications are made if the id is invalid
     */
    void updatePointParams(PointId id, float tension, float bias, float continuity);

    /**
     * @brief Remove a point.
     * @param id Point id.
     *
     * @par No modifications are made if the id is invalid
     */
    void removePoint(PointId id);

    /**
     * @brief Set curve time range (x-axis)
     * @param newRange New time range
     */
    void setTimeRange(RangeF newRange);
    /**
     * @brief Set curve value range (y-axis)
     * @param newRange New value range
     */
    void setValueRange(RangeF newRange);
    
    void setSelected(bool status);

private:
    using PointContainer = QMultiMap<float, Point>;
    using Iterator = PointContainer::Iterator;
    using ConstIterator = PointContainer::ConstIterator;
    
    Iterator findPoint(PointId id);
    ConstIterator findPoint(PointId id) const;
    float limitTimeToRange(float time) const;
    float limitValueToScale(float value) const;
    QList<float> limitValuesToScale(QList<float> value) const;
    
    PointContainer m_points;
    const int m_dimension;
    
    RangeF m_timeRange;
    RangeF m_valueRange;

    bool m_selected;

    QString m_name;
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
