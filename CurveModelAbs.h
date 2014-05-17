#ifndef CURVEMODELABS_H
#define CURVEMODELABS_H

#include "RangeF.h"
#include "Point.h"
#include <QObject>
#include <QMultiMap>
#include <QVariant>

/** Common implementation for curve models. */
class CurveModelAbs : public QObject
{
    Q_OBJECT

protected:
    /**
     * @brief Protected constuctor for CurveModelAbs. Instantions only through derived classes.
     * @param name Curve name
     */
    CurveModelAbs(const QString& name);

public:
    ~CurveModelAbs();

    /** @return Curve name. */
    const QString& name() const;

    /** @return True if curve is selected. */
    bool isSelected() const;

    /** @return Curve time range [start, end]. */
    RangeF timeRange() const;

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

signals:
    /** @brief A new point was added. */
    void pointAdded(PointId id);
    /** @brief Data for an existing point was modified. */
    void pointUpdated(PointId id);
    /** @brief Point was selected. */
    void pointSelected(PointId id);
    /** @brief Point was deselected. */
    void pointDeselected(PointId id);
    /** @brief An existing point was removed. */
    void pointRemoved(PointId id);

    /** @brief Curve was selected or deselected. */
    void selectedChanged(bool status);

    /** @brief Curve time range changed. */
    void timeRangeChanged(RangeF newRange);

public slots:
    /**
     * @brief Set curve name
     * @param name New name
     */
    void setName(QString name);

    /**
     * @brief Set curve selection status
     * @param status True for selected, false for deselected.
     */
    void setSelected(bool status);

    /**
     * @brief Set curve time range (x-axis)
     * @param newRange New time range
     */
    void setTimeRange(RangeF newRange);

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

protected:
    /**
     * @brief Chance for implementation classes to perform internal operations for adding a new point.
     * @param id New point id
     * @param time New point time
     * @param value New point value
     * @return Success of the internal add operation. If false is returned a point will not be added.
     */
    virtual bool addPointInternal(PointId id, float time, QVariant value);
    /**
     * @brief Chance for derived classes to perform internal operations for removing a new point. The point will be removed in any case.
     * @param id Point id to be removed
     */
    virtual void removePointInternal(PointId id);

    /**
     * @brief Chance for derived classes to limit value range.
     * @param value Suggested value
     * @return Possibly limited value
     */
    virtual QVariant limitValueToRange(const QVariant& value) const;

private:
    using PointContainer = QMultiMap<float, Point>;
    PointContainer::Iterator findPoint(PointId id);
    PointContainer::ConstIterator findPoint(PointId id) const;

    void forcePointsToTimeRange(RangeF newRange);
    float limitTimeToRange(float time) const;

private:
    QString m_name;
    bool m_selected;
    RangeF m_timeRange;

    PointContainer m_points;
};

#endif // CURVEMODELABS_H
