//
//  GraphicsItem.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#include "GraphicsItem.h"
#include "GraphicsItemEvent.h"
#include "GraphicsItemDelegate.h"
#include <QGraphicsScene>

GraphicsItem::GraphicsItem(QGraphicsItem* parent, GraphicsItemDelegate* delegate)
:	QGraphicsItem(parent),
	m_delegate(delegate)
{
	// By default selectable and ignoring transformations
    setFlags(flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIgnoresTransformations);
}

GraphicsItem::~GraphicsItem()
{
}

void GraphicsItem::setDelegate(GraphicsItemDelegate* delegate)
{
    m_delegate = delegate;
}

template<class EventType>
bool delegateEvent(GraphicsItemDelegate* delegate, QEvent* event, QGraphicsItem* item)
{
    if (!delegate || !event || !item)
        return false;
    
    if (event->type() == EventType::type())
    {
    	delegate->handleGraphicsItemEvent(item, static_cast<EventType*>(event));
        return true;
    }
    
    return false;
}

bool GraphicsItem::sceneEvent(QEvent* event)
{
    bool delegated = false;
    delegated = delegated || delegateEvent<GraphicsItemMoveStartEvent>(m_delegate, event, this);
    delegated = delegated || delegateEvent<GraphicsItemMoveEvent>(m_delegate, event, this);
    delegated = delegated || delegateEvent<GraphicsItemMoveEndEvent>(m_delegate, event, this);
    delegated = delegated || delegateEvent<GraphicsItemSelectedEvent>(m_delegate, event, this);
    
    return QGraphicsItem::sceneEvent(event);
}

QVariant GraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant output;
    if (m_delegate && m_delegate->handleItemChange(this, change, value, output))
    {
        return output;
    }
    
    return QGraphicsItem::itemChange(change, value);
}

void GraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    
    if (m_delegate)
        m_delegate->handleMousePressEvent(this, event);
}

void GraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
    
    if (m_delegate)
        m_delegate->handleMouseMoveEvent(this, event);
}

void GraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    
    if (m_delegate)
        m_delegate->handleMouseReleaseEvent(this, event);
}

void GraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem::contextMenuEvent(event);
    
    if (m_delegate)
        m_delegate->handleContextMenuEvent(this, event);
}

