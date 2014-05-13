#ifndef CURVEMODELABS_H
#define CURVEMODELABS_H

#include "RangeF.h"
#include <QObject>

/** Common implementation for CurveModels. Not meant to be used as an interface. */
class CurveModelAbs : public QObject
{
    Q_OBJECT

public:
    CurveModelAbs(const QString& name);
    ~CurveModelAbs();

    /** @return Curve name. */
    const QString& name() const;

    /** @return True if curve is selected. */
    bool isSelected() const;

    /** @return Curve time range [start, end]. */
    RangeF timeRange() const;

signals:
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

protected:
    virtual void forcePointsToTimeRange(RangeF newRange) = 0;

private:
    QString m_name;
    bool m_selected;
    RangeF m_timeRange;
};

#endif // CURVEMODELABS_H
