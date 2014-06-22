#ifndef STEPCURVEMODEL_H
#define STEPCURVEMODEL_H

#include "PointId.h"
#include "RangeF.h"
#include "CurveModelAbs.h"
#include <QObject>
#include <QMap>

/** StepCurveModel represents a step curve as its control points. */
class StepCurveModel : public CurveModelAbs
{
    Q_OBJECT

public:
    /**
     * @brief Construct a StepCurveModel
     * @param name Curve name
     */
    StepCurveModel(const QString& name);

    ~StepCurveModel();

    /** Options type */
    using Options = QMap<int, QString>;

    /** @return Possible value options */
    Options options() const;

signals:
    /** @brief Possible value options changed. */
    void optionsChanged(const Options& newOptions);

public slots:
    /**
     * @brief Set curve value options
     * @param newOptions New value options
     */
    void setOptions(const Options& newOptions);

protected:
    virtual StepCurveModel* getAsStepCurve() override;

private:
    /** @see CurveModelAbs::addPointInternal */
    virtual bool addPointInternal(PointId id, float time, QVariant value) override;
    /** @see CurveModelAbs::removePointInternal */
    virtual void removePointInternal(PointId id) override;

    /** @see CurveModelAbs::limitValueToRange */
    virtual QVariant limitValueToRange(const QVariant& value) const override;

    Options m_options;
};

#endif // STEPCURVEMODEL_H
