//
//  GraphicsItemEvent.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef __CurveEditor__GraphicsItemEvent__
#define __CurveEditor__GraphicsItemEvent__

#include <QEvent>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QVariant;
QT_END_NAMESPACE

enum UserEventType
{
    ItemMoveStart,
    ItemMove,
    ItemMoveEnd,

    ItemSelected,
    ItemDeselected,
};

template<UserEventType E, typename T>
class UserEvent : public QEvent
{
public:
    UserEvent(T data) : QEvent(type()), m_data(data)
    {
    }
    
    ~UserEvent() {}
    
    T data()
    {
        return m_data;
    }
    
    static QEvent::Type type()
    {
        static int generatedType = QEvent::registerEventType();
        return static_cast<QEvent::Type>(generatedType);
    }

private:
    T m_data;
};

//template<UserEventType E>
//class EmptyUserEvent : public QEvent
//{
//public:
//    EmptyUserEvent() : QEvent(type())
//    {
//    }
//    
//    ~EmptyUserEvent() {}
//    
//    static QEvent::Type type()
//    {
//        static int generatedType = QEvent::registerEventType();
//        return static_cast<QEvent::Type>(generatedType);
//    }
//};

class GraphicsItemSelectedEvent : public UserEvent<ItemSelected, bool>
{
public:
	GraphicsItemSelectedEvent(bool value) : UserEvent(value) {}
    ~GraphicsItemSelectedEvent() {}
};

class GraphicsItemMoveStartEvent : public UserEvent<ItemMoveStart, const QGraphicsSceneMouseEvent*>
{
public:
	GraphicsItemMoveStartEvent(QGraphicsSceneMouseEvent* sceneEvent) : UserEvent<ItemMoveStart, const QGraphicsSceneMouseEvent*>(sceneEvent) {}
    ~GraphicsItemMoveStartEvent()  {}
};

class GraphicsItemMoveEvent : public UserEvent<ItemMove, const QGraphicsSceneMouseEvent*>
{
public:
	GraphicsItemMoveEvent(QGraphicsSceneMouseEvent* sceneEvent) : UserEvent<ItemMove, const QGraphicsSceneMouseEvent*>(sceneEvent) {}
    ~GraphicsItemMoveEvent()  {}
};

class GraphicsItemMoveEndEvent : public UserEvent<ItemMoveEnd, const QGraphicsSceneMouseEvent*>
{
public:
	GraphicsItemMoveEndEvent(QGraphicsSceneMouseEvent* sceneEvent) : UserEvent<ItemMoveEnd, const QGraphicsSceneMouseEvent*>(sceneEvent) {}
    ~GraphicsItemMoveEndEvent()  {}
};


#endif /* defined(__CurveEditor__GraphicsItemEvent__) */
