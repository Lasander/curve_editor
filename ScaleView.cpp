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
    ScaleLineView(float pos, const RangeF& timeRange, QGraphicsItem* parent);
    ~ScaleLineView();
    
    void setText(const QString& str);
    void setTimeRange(const RangeF& timeRange);
    
private:
    QGraphicsTextItem* m_text;
    QGraphicsLineItem* m_line;
};

ScaleLineView::ScaleLineView(float pos, const RangeF& timeRange, QGraphicsItem* parent)
: 	TransformationNode(parent),
	m_text(new QGraphicsTextItem(this)),
	m_line(new QGraphicsLineItem(timeRange.min, 0, timeRange.max, 0, this))
{
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    
    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    m_line->setPen(pen);

    setPos(0, pos);
}

ScaleLineView::~ScaleLineView()
{}

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

ScaleView::ScaleView(int numberOfScaleLines, const RangeF& timeRange, QGraphicsItem* parent) :
	TransformationNode(parent),
	m_valueScale(0, 1),
    m_timeRange(timeRange)
{
    // Set transformation to scale y-axis to [0, 1] range
    qreal maxi = 1;
    qreal mini = 0;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);

    float scalePos = 0;
    const float scaleStep = 1.0f / static_cast<float>(numberOfScaleLines - 1);

    for (int i = 0; i < numberOfScaleLines; ++i, scalePos += scaleStep)
    {
        // TODO: No text in scale at the moment
//        ScaleLineView* scaleLine = new ScaleLineView(scalePos, m_timeRange, this);
//        scaleLine->setText(QString("%1").arg(scalePos));
        m_scaleLines.push_back(new ScaleLineView(scalePos, m_timeRange, this));
    }
}

ScaleView::~ScaleView()
{
}

void ScaleView::setValueScale(RangeF valueScale)
{
	m_valueScale = valueScale;
}

void ScaleView::setTimeRange(RangeF timeRange)
{
    m_timeRange = timeRange;
    
    for (auto& scale : m_scaleLines)
        scale->setTimeRange(m_timeRange);
}
