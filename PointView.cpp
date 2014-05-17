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
#include <cmath>

PointView::PointView(CurveModel::Point const& pointModel, int index, QGraphicsItem* parent)
:   TransformationNode(parent),
	m_pointModel(pointModel),
    m_index(index),
    m_gridRect(QRectF(0, 0, 2, 0))
{
    assert(m_pointModel.isValid());
    
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

void PointView::setSnapGrid(QRectF gridRect)
{
    m_gridRect = gridRect;
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

    // Snap to grid
    if (!m_gridRect.isNull())
    {
        const QPointF halfRect(m_gridRect.width() / 2.0, m_gridRect.width() / 2.0);

        QPointF inGridPos = pos + halfRect - m_gridRect.bottomLeft();
        inGridPos.setX(m_gridRect.width() > 0.0 ? std::fmod(inGridPos.x(), m_gridRect.width()) - halfRect.x() : 0.0);
        inGridPos.setY(0.0);
        // TODO:Vertical snapping doesn't work yet
        //inGridPos.setY(m_gridRect.height() > 0.0 ? std::fmod(inGridPos.y(), m_gridRect.height()) - halfRect.y() : 0.0);

        qDebug() << "Pos:" << pos << "inGridPos:" << inGridPos << "Grid:" << m_gridRect;

        pos -= inGridPos;
    }


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
    
    bool isSelected = event->data();
    m_text->setVisible(isSelected);
    emit pointSelectedChanged(m_pointModel.id(), m_index, isSelected);
}

bool PointView::handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output)
{
//    qDebug() << "PointView::handleItemChange" << change;
    return GraphicsItemDelegate::handleItemChange(item, change, value, output);
}
