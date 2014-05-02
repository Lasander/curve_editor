//
//  CurveView.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#ifndef __CurveEditor__CurveView__
#define __CurveEditor__CurveView__

#include "TransformationNode.h"
#include "CurveModel.h"
#include <QObject>
#include "pt/math/kb_spline.h"

class PointView;

/**
 * View for potentially multi-dimensional curve.
 * Time axis (x) is unscaled in this view.
 * Valu axis (y) is scaled so the model value range [min, max] maps to range [0, 1].
 */
class CurveView :
	public QObject,
	private TransformationNode
{
    Q_OBJECT
    
public:
    /**
     * @brief Construct CurveView
     * @param parent
     * @param model
     */
    CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent);
    ~CurveView();
    
public slots:
    void pointAdded(PointId id);
    void pointUpdated(PointId id);
    void pointRemoved(PointId id);
    
    void duplicateSelectedPoints();
    void removeSelectedPoints();

    void valueRangeChanged(RangeF valueRange);
    
    void selectedChanged(bool status);

private:
    QList<PointView*> findPointView(PointId id);
    void updateCurves();
    
    // Points, for each dimension there is a separate PointView.
    std::shared_ptr<CurveModel> m_model;
    QMultiMap<PointId, PointView*> m_pointViews;
    
    // Spline
    using Spline = pt::math::kb_spline<float>;
	using SplineDataSet = pt::math::kb_data_set<float>;
    
    bool addToSpline(CurveModel::Point const& point);
    bool removeFromSpline(CurveModel::Point const& point);
    SplineDataSet::iterator findSplinePoint(CurveModel::Point const& point, int index);
    
    QList<Spline*> m_splines;
	QMap<Spline*, QGraphicsPathItem*> m_curveViews;

    void updateTransformation();
};


#endif /* defined(__CurveEditor__CurveView__) */
