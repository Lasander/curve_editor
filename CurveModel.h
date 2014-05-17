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

class CurveModel;

/**
 * @brief A single control point within the curve.
 *
 * Points are created, owned and controlled by a CurveModel instance.
 * Others can refer to the point using its id.
 */
class Point
{
public:
    /** @return Point time */
    float time() const;
    /** @return Point value */
    QVariant value() const;

    /** @return True if point is valid */
    bool isValid() const;
    /** @return Point id */
    PointId id() const;

    /** @return True if point is selected */
    bool isSelected() const;

private:
    /** For now allow only CurveModel to create/modify */
    friend class CurveModel;

    /** @brief Construct invalid point */
    Point();

    /** @brief Construct valid point, optionally with an existing id */
    Point(float time, QVariant value, bool isSelected, PointId id = PointId::invalidId());

    /**
     * @brief Set point selected state.
     * @param isSelected True for selected.
     */
    void setSelected(bool isSelected);

    /** Compare two points */
    bool operator==(const Point& other) const;

private:
    bool m_isValid;
    bool m_isSelected;
    PointId m_id;
    float m_time;
    QVariant m_value;
};

inline Point::Point()
:	m_isValid(false), m_isSelected(false), m_id(PointId::invalidId()), m_time(0), m_value()
{
}
inline Point::Point(float time, QVariant value, bool isSelected, PointId id)
:	m_isValid(true), m_isSelected(isSelected), m_id(id.isValid() ? id : PointId::generateId()), m_time(time), m_value(value)
{
}
inline float Point::time() const
{
    return m_time;
}
inline QVariant Point::value() const
{
    return m_value;
}
inline bool Point::isValid() const
{
    return m_isValid;
}
inline PointId Point::id() const
{
    return m_id;
}
inline bool Point::isSelected() const
{
    return m_isSelected;
}
inline void Point::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}
inline bool Point::operator==(const Point& other) const
{
    return
        (m_isValid == other.m_isValid) &&
        (m_isSelected == other.m_isSelected) &&
        (m_id == other.m_id) &&
        (m_time == other.m_time) &&
        (m_value == other.m_value);
}

/**
 * CurveModel represents a curve as its control points.
 * Derived from QObject for signals and slots.
 */
class CurveModel : public CurveModelAbs
{
    Q_OBJECT
    
public:
    /** Kochanek-Bartels parameters for a single curve point */
    class KbParams
    {
    public:
        /** @return tension parameter */
        float tension() const;
        /** @return bias parameter */
        float bias() const;
        /** @return continuity parameter */
        float continuity() const;

    private:
        KbParams(float tension = 0.0f, float bias = 0.0f, float continuity = 0.0f);
        void update(float tension, float bias, float continuity);

        bool operator==(KbParams const& other) const;
        bool operator!=(KbParams const& other) const;

        // Allow curve to create parameters
        friend class CurveModel;

    private:
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

    /**
     * @return Copy of a point params with the given id. In case an invalid or
     * non-existent id is given default params are returned.
     */
    const KbParams params(PointId id) const;

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
    void addPoint(float time, QVariant value);
    /**
     * @brief Update point time and/or value for one dimension of an existing point.
     *
     * @param id Point id
     * @param time New time
     * @param value New value
     *
     * @par No modifications are made if the id is invalid
     */
    void updatePoint(PointId id, float time, QVariant value);
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
    using ParamContainer = QMap<PointId, KbParams>;

    virtual void forcePointsToTimeRange(RangeF newRange);
    
    PointContainer::Iterator findPoint(PointId id);
    PointContainer::ConstIterator findPoint(PointId id) const;
    float limitTimeToRange(float time) const;
    QVariant limitValueToScale(const QVariant& value) const;

    PointContainer m_points;
    ParamContainer m_params;
    RangeF m_valueRange;
};

inline CurveModel::KbParams::KbParams(float tension, float bias, float continuity)
  : m_tension(tension), m_bias(bias), m_continuity(continuity)
{
}
inline float CurveModel::KbParams::tension() const
{
    return m_tension;
}
inline float CurveModel::KbParams::bias() const
{
    return m_bias;
}
inline float CurveModel::KbParams::continuity() const
{
    return m_continuity;
}
inline void CurveModel::KbParams::update(float tension, float bias, float continuity)
{
    m_tension = tension;
    m_bias = bias;
    m_continuity = continuity;
}
inline bool CurveModel::KbParams::operator==(KbParams const& other) const
{
    return
        (m_tension == other.m_tension) &&
        (m_bias == other.m_bias) &&
        (m_continuity == other.m_continuity);
}
inline bool CurveModel::KbParams::operator!=(KbParams const& other) const
{
    return !((*this)==other);
}

#endif /* CURVEMODEL_H */
