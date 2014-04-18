//
//  GraphicsItemDelegate.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#include "GraphicsItemDelegate.h"
#include "GraphicsItemEvent.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>
#include <QMenu>
#include <QAction>
#include <QDebug>

GraphicsItemDelegate::GraphicsItemDelegate()
{
}

GraphicsItemDelegate::~GraphicsItemDelegate()
{
}

bool GraphicsItemDelegate::handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output)
{
    Q_UNUSED(output);
    
    QGraphicsScene* scene = item->scene();
    
    if (!scene)
        return false;
    
    switch (change)
    {
        case QGraphicsItem::ItemSelectedHasChanged:
        {
            GraphicsItemSelectedEvent itemEvent(value.toBool());
            scene->sendEvent(item, &itemEvent);
            break;
	    }
        default:
            break;
    }
    
    return false;
}

void GraphicsItemDelegate::handleMousePressEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene* scene = item->scene();
    
    if (!scene)
        return;
    
    GraphicsItemMoveStartEvent itemEvent(event);
    for (auto &selected_item : scene->selectedItems())
        scene->sendEvent(selected_item, &itemEvent);
}

void GraphicsItemDelegate::handleMouseMoveEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene* scene = item->scene();
    
    if (!scene)
        return;
    
    GraphicsItemMoveEvent itemEvent(event);
    for (auto &selected_item : scene->selectedItems())
        scene->sendEvent(selected_item, &itemEvent);
}

void GraphicsItemDelegate::handleMouseReleaseEvent(QGraphicsItem* item, QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene* scene = item->scene();
    
    if (!scene)
        return;
    
    GraphicsItemMoveEndEvent itemEvent(event);
    for (auto &selected_item : scene->selectedItems())
        scene->sendEvent(selected_item, &itemEvent);
}

void GraphicsItemDelegate::handleContextMenuEvent(QGraphicsItem* item, QGraphicsSceneContextMenuEvent* event)
{
    qDebug() << "handleContextMenuEvent";
    
    Q_UNUSED(item);
    Q_UNUSED(event);
    
//    QGraphicsScene* scene = item->scene();
//    
//    if (!scene)
//        return;
//    
//	auto selectedItems = scene->selectedItems();
//    
//    QMenu menu;
//    menu.addAction(selectedItems.size() > 1 ? "Add items" : "Add item");
//    menu.addAction(selectedItems.size() > 1 ? "Remove items" : "Remove item");
//    menu.exec(event->screenPos());
}

void GraphicsItemDelegate::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveStartEvent* event)
{
    Q_UNUSED(item);
    Q_UNUSED(event);
}
void GraphicsItemDelegate::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEvent* event)
{
    Q_UNUSED(item);
    Q_UNUSED(event);
}
void GraphicsItemDelegate::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEndEvent* event)
{
    Q_UNUSED(item);
    Q_UNUSED(event);
}
void GraphicsItemDelegate::handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemSelectedEvent* event)
{
    Q_UNUSED(item);
    Q_UNUSED(event);
}


