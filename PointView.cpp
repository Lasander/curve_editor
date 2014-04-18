//
//  PointView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#include "PointView.h"
#include "PointItem.h"
#include "GraphicsItemEvent.h"
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsScene>
#include <QObject>
#include <QDebug>
#include <assert.h>
#include <QGraphicsSceneMouseEvent>

PointView::PointView(CurveModel::Point const& pointModel, int index, QGraphicsItem* parent)
:   TransformationNode(parent),
	m_pointModel(pointModel),
	m_index(index)
{
    assert(m_pointModel.is_valid());
    
    m_item = new PointItem(this, this);
    m_item->setAcceptedMouseButtons(Qt::LeftButton);
    
    m_text = new QGraphicsSimpleTextItem(this);
    QFont smaller(m_text->font());
    smaller.setPointSize(8);
    m_text->setFont(smaller);
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    m_text->setVisible(false);
    
    setPoint(pointModel);
}

PointView::~PointView() {}

void PointView::setPoint(CurveModel::Point const& pointModel)
{
    m_pointModel = pointModel;
    QPointF pos(m_pointModel.time(), m_pointModel.value(m_index));
    qDebug() << "PointView::setPoint"  << m_pointModel.id() << pos;
    setPos(pos);
    m_text->setText(QString("(%1,%2)").arg(QString::number(m_pointModel.time(), 'f', 2), QString::number(m_pointModel.value(m_index), 'f', 2)));
}

const CurveModel::Point PointView::point() const
{
	return m_pointModel;
}

bool PointView::isSelected() const
{
    return m_item->isSelected();
}

void PointView::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveStartEvent* event)
{
    Q_UNUSED(item);
    
    qDebug() << "PointView::GraphicsItemMoveStartEvent" << m_pointModel.id();
    m_offset = scenePos() - event->data()->scenePos();
}
void PointView::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEvent* event)
{
    Q_UNUSED(item);
    
    qDebug() << "PointView::GraphicsItemMoveEvent" << m_pointModel.id();
    QPointF scenePos = event->data()->scenePos() + m_offset;
    QPointF pos = parentItem()->mapFromScene(scenePos);
    emit pointPositionChanged(m_pointModel.id(), pos.x(), pos.y(), m_index);
}
void PointView::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEndEvent* event)
{
    Q_UNUSED(item);
    Q_UNUSED(event);
    
    qDebug() << "PointView::GraphicsItemMoveEndEvent" << m_pointModel.id();
}

void PointView::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemSelectedEvent* event)
{
    Q_UNUSED(item);
    
    qDebug() << "PointView::GraphicsItemSelectedEvent"  << m_pointModel.id() << event->data();
    
    m_text->setVisible(event->data());
}

bool PointView::handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output)
{
//    qDebug() << "PointView::handleItemChange" << change;
    return GraphicsItemDelegate::handleItemChange(item, change, value, output);
}
