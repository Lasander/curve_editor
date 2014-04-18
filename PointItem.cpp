//
//  PointItem.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#include "PointItem.h"
#include <QGraphicsItem>
#include <QDebug>
#include <QPen>
#include <QPainter>

PointItem::PointItem(QGraphicsItem* parent, GraphicsItemDelegate* delegate)
:	GraphicsItem(parent, delegate)
{
}

PointItem::~PointItem() {}


QRectF PointItem::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void PointItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);
    
    QPen pen;
    pen.setCosmetic(true);
    if (isSelected()) pen.setColor(QColor(Qt::green));
    
    painter->setPen(pen);
    painter->drawRect(boundingRect());
}