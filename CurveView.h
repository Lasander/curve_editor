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
#include <memory>

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
     * @param model Curve model
     * @param parent Parent item
     */
    CurveView(std::shared_ptr<CurveModel> model, QGraphicsItem* parent);
    ~CurveView();

signals:
    /**
     * @brief Curve snap grid has changed
     * @param gridRect New snap grid
     */
    void snapGridChanged(QRectF gridRect);
    
public slots:
    /**
     * @brief Set curve snap grid
     * @param gridRect New snap grid
     */
    void setSnapGrid(QRectF gridRect);

    /**
     * @brief Set snap-to-grid
     * @param snapToGrid True if curve should snap to define snap grid.
     */
    void setSnapToGrid(bool snapToGrid);

    /** @brief Add new points corresponding to all selected points in the curve. */
    void duplicateSelectedPoints();

    /** @brief Remove all selected points in the curve. */
    void removeSelectedPoints();


    void highlightCurve(bool highlight);

private slots:
    void pointAdded(PointId id);
    void pointUpdated(PointId id);
    void pointRemoved(PointId id);

    void valueRangeChanged(RangeF valueRange);
    
    void selectedChanged(bool status);

private:
    QList<PointView*> findPointView(PointId id);
    void updateCurves();
    void updateTransformation();

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

    QRectF m_snapGridRect;
    bool m_snapToGrid;

    bool m_highlightCurve;
};


#endif /* defined(__CurveEditor__CurveView__) */
