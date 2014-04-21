//
//  TransformationNode.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

//#define DEBUG_BOUNDING_RECT

#include "TransformationNode.h"
#include <QPen>
#include <QPainter>

TransformationNode::TransformationNode(QGraphicsItem* parent)
:	QGraphicsItem(parent)
{
#ifndef DEBUG_BOUNDING_RECT
    // Nothing to draw in a TransformationNode
    setFlags(QGraphicsItem::ItemHasNoContents);
#endif
}

TransformationNode::~TransformationNode() {}

QRectF TransformationNode::boundingRect() const
{
    return QRect();
}

QPainterPath TransformationNode::shape() const
{
    return QPainterPath();
}

#ifdef DEBUG_BOUNDING_RECT
void TransformationNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QPen pen(Qt::red);
    pen.setCosmetic(true);

    painter->setPen(pen);
    painter->drawRect(this->childrenBoundingRect());
}
#else
void TransformationNode::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget* )
{
    // Nothing to draw.
}
#endif
