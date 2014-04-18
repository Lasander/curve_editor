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

/** View for potentially multi-dimensional curve.
 Y-axis (values) are scaled so they map to range [0, 1].
 */
class CurveView :
	public QObject,
	private TransformationNode
{
    Q_OBJECT
    
public:
    CurveView(QGraphicsItem* parent, std::shared_ptr<CurveModel> model);
    ~CurveView();
    
public slots:
    void pointAdded(PointId id);
    void pointUpdated(PointId id);
    void pointRemoved(PointId id);
    
    void duplicateSelectedPoints();
    void removeSelectedPoints();
    
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
    SplineDataSet::const_iterator findSplinePoint(CurveModel::Point const& point, int index);
    
    QList<Spline*> m_splines;
	QMap<Spline*, QGraphicsPathItem*> m_curveViews;
};


#endif /* defined(__CurveEditor__CurveView__) */
