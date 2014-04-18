//
//  TransformationNode.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#include "TransformationNode.h"

TransformationNode::TransformationNode(QGraphicsItem* parent)
:	QGraphicsItem(parent)
{
    // Nothing to draw in a TransformationNode
    setFlags(QGraphicsItem::ItemHasNoContents);
}

TransformationNode::~TransformationNode() {}

QRectF TransformationNode::boundingRect() const
{
    return QRect(0,0,0,0);
}

QPainterPath TransformationNode::shape() const
{
    return QPainterPath();
}

void TransformationNode::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
}