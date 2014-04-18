//
//  PointItem.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef __CurveEditor__PointItem__
#define __CurveEditor__PointItem__

#include "GraphicsItem.h"
class GraphicsItemDelegate;

class PointItem : public GraphicsItem
{
public:
	PointItem(QGraphicsItem* parent, GraphicsItemDelegate* delegate);
    ~PointItem();
    
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
};


#endif /* defined(__CurveEditor__PointItem__) */
