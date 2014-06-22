#include "StepCurveModel.h"
#include <assert.h>
#include <utility>
#include <QDebug>

StepCurveModel::StepCurveModel(const QString& name)
  :	CurveModelAbs(name)
{
}

StepCurveModel::~StepCurveModel()
{
}

StepCurveModel::Options StepCurveModel::options() const
{
    return m_options;
}

void StepCurveModel::setOptions(const Options& newOptions)
{
    if (newOptions.isEmpty())
    {
        qWarning() << "Trying to set empty options";
        return;
    }

    m_options = newOptions;
    int previousValue = m_options.firstKey();

    // Ensure all points fit map to a valid option, if not, change to previous value
    for (auto pid : pointIds())
    {
        const Point p = point(pid);
        if (!m_options.contains(p.value().toInt()))
            updatePoint(pid, p.time(), previousValue);
        else
            previousValue = p.value().toInt();
    }
}

StepCurveModel* StepCurveModel::getAsStepCurve()
{
    return this;
}

bool StepCurveModel::addPointInternal(PointId id, float time, QVariant value)
{
    Q_UNUSED(id);
    Q_UNUSED(time);

    bool ok;
    int intValue = value.toInt(&ok);
    if (!ok)
    {
        qWarning() << "Cannot add point to step curve" << id << " bad value:" << value;
        return false;
    }

    if (!m_options.contains(intValue))
    {
        qWarning() << "Cannot add point to step curve" << id << " unknown value:" << value;
        return false;
    }

    return true;
}

void StepCurveModel::removePointInternal(PointId id)
{
    Q_UNUSED(id)
}

QVariant StepCurveModel::limitValueToRange(const QVariant& value) const
{
    bool ok;
    float floatValue = value.toFloat(&ok);
    if (!ok)
        return m_options.firstKey();

    Options::const_iterator atOrAbove = m_options.lowerBound(floatValue);
    if (atOrAbove != m_options.begin())
    {
        Options::const_iterator below = atOrAbove - 1;
        if (abs(floatValue - below.key()) < abs(floatValue - atOrAbove.key()))
        {
            return below.key();
        }
    }

    return atOrAbove.key();
}
