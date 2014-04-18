//
//  GraphicsItemDelegate.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef __CurveEditor__GraphicsItemDelegate__
#define __CurveEditor__GraphicsItemDelegate__

#include "GraphicsItemEvent.h"
#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QVariant;
QT_END_NAMESPACE

class GraphicsItemDelegate
{
public:
    //    virtual void beforePaint(QGraphicsItem* item, QPainter* painter, const QStyleOptionGraphicsItem* option) {}
    //    virtual void afterPaint(QGraphicsItem* item, QPainter* painter, const QStyleOptionGraphicsItem* option) {}
    virtual bool handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output);
    
    virtual void handleMousePressEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event);
    virtual void handleMouseMoveEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event);
    virtual void handleMouseReleaseEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event);
    virtual void handleContextMenuEvent(QGraphicsItem* item, QGraphicsSceneContextMenuEvent* event);
    
    virtual void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveStartEvent* event);
    virtual void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEvent* event);
    virtual void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEndEvent* event);
    virtual void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemSelectedEvent* event);
    
    virtual ~GraphicsItemDelegate();
    
protected:
    GraphicsItemDelegate();
};

#endif /* defined(__CurveEditor__GraphicsItemDelegate__) */
