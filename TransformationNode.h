//
//  TransformationNode.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#ifndef __CurveEditor__TransformationNode__
#define __CurveEditor__TransformationNode__

#include <QGraphicsItem>

/** 
 Non-drawable graphics item used to apply transformations to drawable
 item positions (children of this item) that are ignoring transformations
 themselves.
 */
class TransformationNode : public QGraphicsItem
{
public:
    TransformationNode(QGraphicsItem* parent);
    virtual ~TransformationNode();
    
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
};

#endif /* defined(__CurveEditor__TransformationNode__) */
