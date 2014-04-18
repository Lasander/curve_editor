//
//  CurveView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#include "CurveView.h"
#include "PointView.h"
#include "ScaleView.h"
#include <QGraphicsPathItem>
#include <QDebug>
#include <QPen>
#include <assert.h>

CurveView::CurveView(QGraphicsItem* parent, std::shared_ptr<CurveModel> model)
:   TransformationNode(parent),
	m_model(model)
{
	for (int i = 0; i < model->dimension(); ++i)
    {
        Spline* spline = new Spline(SplineDataSet());
		m_splines.insert(i, spline);
        
        QGraphicsPathItem* curveItem = new QGraphicsPathItem(this);
        QPen curvePen(Qt::blue);
        curvePen.setCosmetic(true);
        curveItem->setPen(curvePen);
        
        m_curveViews[spline] = curveItem;
    }
    
    // Set transformation
    qreal maxi = 100;
    qreal mini = -100;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);
    
    connect(m_model.get(), &CurveModel::pointAdded, this, &CurveView::pointAdded);
    connect(m_model.get(), &CurveModel::pointUpdated, this, &CurveView::pointUpdated);
    connect(m_model.get(), &CurveModel::pointRemoved, this, &CurveView::pointRemoved);
    
    for (auto pid : m_model->pointIds())
    	pointAdded(pid);
}

CurveView::~CurveView()
{
	for (auto p : m_splines)
		delete p;
}

void CurveView::pointAdded(PointId id)
{
    qDebug() << "CurveView::pointAdded" << id;
    assert(m_model);
    
    const CurveModel::Point point = m_model->point(id);
    assert(point.is_valid());
    assert(findPointView(id).isEmpty());

    for (int i = 0; i < m_model->dimension(); ++i)
    {
    	PointView* pointView = new PointView(point, i, this);
        m_pointViews.insert(id, pointView);
        connect(pointView, &PointView::pointPositionChanged, m_model.get(), &CurveModel::updatePoint);
    }

    bool addOk = addToSpline(point);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointUpdated(PointId id)
{
    qDebug() << "CurveView::pointUpdated" << id;
    
    QList<PointView*> pointViews = findPointView(id);
    assert(!pointViews.isEmpty());
    const CurveModel::Point old(pointViews[0]->point());
    
    const CurveModel::Point p = m_model->point(id);
    assert(p.is_valid());

    for (int i = 0; i < m_model->dimension(); ++i)
	    pointViews[i]->setPoint(p);

	bool removeOk = removeFromSpline(old);
    assert(removeOk);
    
    bool addOk = addToSpline(p);
    assert(addOk);
    
    updateCurves();
}

void CurveView::pointRemoved(PointId id)
{
    qDebug() << "CurveView::pointRemoved" << id;
    
    QList<PointView*> pointViews = findPointView(id);
    assert(!pointViews.isEmpty());
    const CurveModel::Point removed(pointViews[0]->point());

    int numberOfRemoved = m_pointViews.remove(id);
    assert(numberOfRemoved == m_model->dimension());
    
    for (int i = 0; i < m_model->dimension(); ++i)
    	delete pointViews[i];
    
	bool removeOk = removeFromSpline(removed);
    assert(removeOk);
    
  	updateCurves();
}

void CurveView::duplicateSelectedPoints()
{
    qDebug() << "CurveView::duplicateSelectedPoints";

    QSet<PointId> toBeDuplicated;
    
    for (auto &point : m_pointViews)
    	if (point->isSelected())
            toBeDuplicated.insert(point->point().id());
    
    for (auto pid : toBeDuplicated)
    {
        const CurveModel::Point point = m_model->point(pid);
        m_model->addPoint(point.time() + 10, point.values());
    }
}

void CurveView::removeSelectedPoints()
{
    QSet<PointId> toBeRemoved;
    
    for (auto &point : m_pointViews)
    	if (point->isSelected())
            toBeRemoved.insert(point->point().id());

    for (auto pid : toBeRemoved)
        m_model->removePoint(pid);
}

QList<PointView*> CurveView::findPointView(PointId id)
{
    return m_pointViews.values(id);
}

void CurveView::updateCurves()
{
    for (auto spline : m_splines)
    {
        auto cur = spline->data().begin();
        if (cur == spline->data().end())
        {
            // No spline data
            m_curveViews[spline]->setPath(QPainterPath());
            continue;
        }
        
        constexpr int STEPS_PER_INTERVAL = 20;
        
        QPainterPath path;
        path.moveTo(QPointF(cur->time(), cur->value()));
        
        auto next = cur + 1;
        while (next != spline->data().end())
        {
            const float startTime = cur->time();
            const float endTime = next->time();
            
            const float time_diff = endTime - startTime;
            const float step = time_diff / STEPS_PER_INTERVAL;
            
            // Start from next step after start as first point is handled either
            // before the loop (moveTo) or by the previous interval
            float time = startTime + step;
            
            for (int i = 0; i < STEPS_PER_INTERVAL; ++i)
            {
                path.lineTo(QPointF(time, spline->value_at(time)));
                time += step;
            }
            
            ++cur;
            ++next;
        }
        
        m_curveViews[spline]->setPath(path);
    }
}

bool CurveView::addToSpline(CurveModel::Point const& point)
{
    if (!point.is_valid())
        return false;
    
    pt::math::kochanek_bartels_parameters params(point.tension(), point.bias(), point.continuity());
    
	for (int i = 0; i < m_model->dimension(); ++i)
    {
        pt::math::data_set<float>::point p(point.time(), point.value(i));
        auto point_iter = m_splines[i]->data().add(p, params);
        if (point_iter == m_splines[i]->data().end())
            return false;
    }
    
    return true;
}

bool CurveView::removeFromSpline(CurveModel::Point const& point)
{
    if (!point.is_valid())
        return false;
    
	for (int i = 0; i < m_model->dimension(); ++i)
    {
        auto point_iter = findSplinePoint(point, i);
        if (point_iter == m_splines[i]->data().end())
            return false;
        
        m_splines[i]->data().erase(point_iter);
    }
    
    return true;
}

CurveView::SplineDataSet::const_iterator CurveView::findSplinePoint(CurveModel::Point const& point, int index)
{
    auto points = m_splines[index]->data().points_at(point.time());
    auto cur = points.first;
    auto next = points.second;
    if (next != m_splines[index]->data().end())
        return m_splines[index]->data().end();
    
    return cur;
}
