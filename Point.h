#ifndef POINT_H
#define POINT_H

#include "PointId.h"
#include <QVariant>

class CurveModelAbs;

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
    friend class CurveModelAbs;

    /** @brief Construct invalid point */
    Point();

    /** @brief Construct valid point, optionally with an existing id. If no valid id is given a valid id will be generated. */
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

#endif // POINT_H
