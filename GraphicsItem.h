//
//  GraphicsItem.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef __CurveEditor__GraphicsItem__
#define __CurveEditor__GraphicsItem__

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class GraphicsItemDelegate;

/** Drawable graphics items that delegate the event handling to a GraphicsItemDelegate */
class GraphicsItem : public QGraphicsItem
{
public:
    void setDelegate(GraphicsItemDelegate* delegate);
    
    ~GraphicsItem();
    
protected:
    GraphicsItem(QGraphicsItem* parent, GraphicsItemDelegate* delegate);
    
private:
    bool sceneEvent(QEvent* event) override;
    
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    
private:
    GraphicsItemDelegate* m_delegate;
};


#endif /* defined(__CurveEditor__GraphicsItem__) */
