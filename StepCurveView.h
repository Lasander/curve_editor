#ifndef STEPCURVEVIEW_H
#define STEPCURVEVIEW_H

#include "CurveViewAbs.h"
#include "StepCurveModel.h"
#include <QObject>
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsPathItem;
QT_END_NAMESPACE

/**
 * View for a step curve.
 * Time axis (x) is unscaled in this view.
 * Value axis (y) is scaled so the model options map evenly to range [0, 1].
 */
class StepCurveView : public CurveViewAbs
{
    Q_OBJECT

private:
    /**
     * @brief Construct StepCurveView. Private to force two-phase initialization using create method.
     * @param model Step curve model
     * @param parent Parent item
     */
    StepCurveView(std::shared_ptr<StepCurveModel> model, QGraphicsItem* parent);

public:
    ~StepCurveView();

    /**
     * @see StepCurveView::StepCurveView
     * @return new StepCurveView
     */
    static StepCurveView* create(std::shared_ptr<StepCurveModel> model, QGraphicsItem* parent);

private slots:
    void updateOptions(StepCurveModel::Options options);

private:
    virtual bool internalAddPoint(PointId id) override;
    virtual bool internalUpdatePoint(PointId id) override;
    virtual bool internalRemovePoint(PointId id) override;

    virtual std::pair<float, QVariant> placeNewPoint(PointId id, PointId nextId) const override;

    virtual void updateCurves() override;
    void updateTransformation();

    std::shared_ptr<StepCurveModel> m_model;
    QGraphicsPathItem* m_curveView;
};

#endif // STEPCURVEVIEW_H
