//
//  CurveModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#ifndef CURVEMODEL_H
#define CURVEMODEL_H

#include "PointId.h"
#include "RangeF.h"
#include "CurveModelAbs.h"
#include <QObject>
#include <QMultiMap>
#include <QDebug>

/**
 * CurveModel represents a curve as its control points.
 * Derived from QObject for signals and slots.
 */
class CurveModel : public CurveModelAbs
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
        float value() const;
        
        float tension() const;
        float bias() const;
        float continuity() const;

        bool isValid() const;
        PointId id() const;
        
	private:
        /** @brief Construct invalid point */
        Point();

        /** @brief Construct valid point, optionally with an existing id */
        Point(float time, float value, bool isSelected, PointId id = PointId::invalidId());
        Point(float time, float value, float tension, float bias, float continuity, bool isSelected, PointId id = PointId::invalidId());
        void updateParams(float tension, float bias, float continuity);

        bool operator==(Point const& other) const;
        bool operator!=(Point const& other) const;

        bool isSelected() const;
        void setSelected(bool isSelected);
        
        // Allow curve to create points
        friend class CurveModel;
        
    private:
        bool m_isValid;
        bool m_isSelected;
        PointId m_id;
        float m_time;
        float m_value;
        
        float m_tension;
        float m_bias;
        float m_continuity;
    };
    
public:
    /**
     * @brief Construct a CurveModel
     * @param name Curve name
     */
    CurveModel(const QString& name);

    ~CurveModel();

    /** @return A list of point ids. */
    QList<PointId> pointIds() const;

    /**
     * @brief Retrieve next point id from the given one.
     * @param id Original point
     * @return Point id for the next point or invalid if none found
     */
    PointId nextPointId(PointId id) const;
    
    /**
     * @return Copy of a point with the given id. In case an invalid or
     * non-existent id is given, a invalid point is returned.
     */
    const Point point(PointId id) const;

    /** @return The number of point in the curve. */
    int numberOfPoints() const;

    /** @return Curve value range [min, max]. */
    RangeF valueRange() const;

signals:
    /** @brief Curve value range changed. */
    void valueRangeChanged(RangeF newRange);

public slots:
    /**
     * @brief Add new point with given attributes.
     *
     * @param time Point time
     * @param value Point value
     * @par Tension, bias and continuity parameters default to 0.
     */
    void addPoint(float time, float value);
    /**
     * @brief Add new point with given attributes.
     *
     * @param time Point time
     * @param value Point value
     * @param tension Tension parameter at the point
     * @param bias Bias parameter at the point
     * @param continuity Continuity parameter at the point
     */
    void addPoint(float time, float value, float tension, float bias, float continuity);

    /**
     * @brief Update point time and/or value for one dimension of an existing point.
     *
     * @param id Point id
     * @param time New time
     * @param value New value
     *
     * @par No modifications are made if the id is invalid
     */
    void updatePoint(PointId id, float time, float value);
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
     * @brief Point selected state changed.
     * @param id Point id
     * @param isSelected True if point is selected
     */
    void pointSelectedChanged(PointId id, bool isSelected);

    /**
     * @brief Remove a point.
     * @param id Point id.
     *
     * @par No modifications are made if the id is invalid
     */
    void removePoint(PointId id);

    /**
     * @brief Set curve value range (y-axis)
     * @param newRange New value range
     */
    void setValueRange(RangeF newRange);

private:
    using PointContainer = QMultiMap<float, Point>;
    using Iterator = PointContainer::Iterator;
    using ConstIterator = PointContainer::ConstIterator;

    virtual void forcePointsToTimeRange(RangeF newRange);
    
    Iterator findPoint(PointId id);
    ConstIterator findPoint(PointId id) const;
    float limitTimeToRange(float time) const;
    float limitValueToScale(float value) const;
    
    PointContainer m_points;
    RangeF m_valueRange;
};

inline float CurveModel::Point::time() const
{
    return m_time;
}
inline float CurveModel::Point::value() const
{
    return m_value;
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
inline bool CurveModel::Point::isValid() const
{
    return m_isValid;
}
inline PointId CurveModel::Point::id() const
{
    return m_id;
}
inline bool CurveModel::Point::operator==(Point const& other) const
{
    return
    (other.isValid() == isValid()) &&
    (other.id() == id()) &&
    (other.time() == time()) &&
    (other.value() == value()) &&
    (other.tension() == tension()) &&
    (other.bias() == bias()) &&
    (other.continuity() == continuity());
}

inline bool CurveModel::Point::operator!=(Point const& other) const
{
    return !operator==(other);
}


#endif /* CURVEMODEL_H */
