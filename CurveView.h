#ifndef CURVEVIEW_H
#define CURVEVIEW_H

#include "CurveModel.h"
#include "CurveViewAbs.h"
#include "pt/math/kb_spline.h"
#include <QObject>
#include <memory>

/**
 * A spline view for a curve model.
 * Time axis (x) is unscaled in this view.
 * Valu axis (y) is scaled so the model value range [min, max] maps to range [0, 1].
 */
class CurveView : public CurveViewAbs
{
    Q_OBJECT

private:
    /**
     * @brief Private constructor for CurveView. Private to force two-phase initialization done by static create method.
     * @param model Curve model
     * @param parent Parent item
     */
    CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent);
    
public:
    ~CurveView();

    /**
     * @see CurveView::CurveView
     * @return new CurveView
     */
    static CurveView* create(std::shared_ptr<CurveModel> model, QGraphicsItem* parent);

private slots:
    void changeValueRange(RangeF valueRange);

    virtual bool internalAddPoint(PointId id) override;
    virtual bool internalUpdatePoint(PointId id) override;
    virtual bool internalRemovePoint(PointId id) override;

private:
    virtual std::pair<float, QVariant> placeNewPoint(PointId id, PointId nextId) const override;

    virtual void updateCurves() override;
    void updateTransformation();

    std::shared_ptr<CurveModel> m_model;
    
    // Spline
    using Spline = pt::math::kb_spline<float>;
	using SplineDataSet = pt::math::kb_data_set<float>;
    
    bool addToSpline(PointId id);
    bool removeFromSpline(PointId id);
    SplineDataSet::iterator findSplinePoint(PointId id) const;
    
    Spline* m_spline;
    QGraphicsPathItem* m_curveView;
};


#endif /* CURVEVIEW_H */
