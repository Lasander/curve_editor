//
//  ScaleView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#include "ScaleView.h"
#include <QPen>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

class ScaleLineView : private TransformationNode
{
public:
	ScaleLineView(const RangeF& timeRange, QGraphicsItem* parent);
    ~ScaleLineView();
    
    void setPos(float pos);
    void setText(const QString& str);
    void setTimeRange(const RangeF& timeRange);
    
private:
    QGraphicsTextItem* m_text;
    QGraphicsLineItem* m_line;
};

ScaleLineView::ScaleLineView(const RangeF& timeRange, QGraphicsItem* parent)
: 	TransformationNode(parent),
	m_text(new QGraphicsTextItem(this)),
	m_line(new QGraphicsLineItem(timeRange.min, 0, timeRange.max, 0, this))
{
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    
    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    m_line->setPen(pen);
}

ScaleLineView::~ScaleLineView()
{}

void ScaleLineView::setPos(float pos)
{
    TransformationNode::setPos(0, pos);
}

void ScaleLineView::setText(const QString& str)
{
    m_text->setPlainText(str);
}

void ScaleLineView::setTimeRange(const RangeF& timeRange)
{
	m_line->setLine(qMin(0.0f, timeRange.min), 0, timeRange.max, 0);
}

///////////////////////////////////////
///////////////////////////////////////

ScaleView::ScaleView(const QList<float>& scaleLines, QGraphicsItem* parent) :
	TransformationNode(parent),
	m_valueScale(0, 1),
	m_timeScale(0, 1),
	m_scaleLineValues(scaleLines)
{
    // Set transformation to scale y-axis to [0, 1] range
    qreal maxi = 100;
    qreal mini = -100;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);
    
	for (auto scalePos : m_scaleLineValues)
    {
        ScaleLineView* scaleLine = new ScaleLineView(m_timeScale, this);
		scaleLine->setPos(scalePos);
		scaleLine->setText(QString("%1").arg(scalePos));
        
        m_scaleLines.push_back(scaleLine);
    }
}

ScaleView::~ScaleView()
{
}

void ScaleView::setValueScale(RangeF valueScale)
{
	m_valueScale = valueScale;
}

void ScaleView::setTimeScale(RangeF timeScale)
{
    m_timeScale = timeScale;
    
    for (auto& scale : m_scaleLines)
        scale->setTimeRange(m_timeScale);
}


