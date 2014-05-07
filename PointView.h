//
//  PointView.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#ifndef __CurveEditor__PointView__
#define __CurveEditor__PointView__

#include "TransformationNode.h"
#include "GraphicsItemDelegate.h"
#include "CurveModel.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsItem;
class QGraphicsSimpleTextItem;
QT_END_NAMESPACE

/** View for a single dimension of a curve point. */
class PointView :
	public QObject,
	private TransformationNode,
	private GraphicsItemDelegate
{
    Q_OBJECT
    
public:
    PointView(CurveModel::Point const& pointModel, int index, QGraphicsItem* parent);
    ~PointView();
    
    void setPoint(CurveModel::Point const& pointModel);
    const CurveModel::Point point() const;
    
public:
    using TransformationNode::pos;
    bool isSelected() const;
    
signals:
    void pointPositionChanged(PointId id, float time, float value, int index);
    void pointSelectedChanged(PointId id, int index, bool isSelected);

private:
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveStartEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEndEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemSelectedEvent* event) override;
    
    bool handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output) override;
    
    QGraphicsItem* m_item;
    QGraphicsSimpleTextItem* m_text;
    CurveModel::Point m_pointModel;
    int m_index;
    QPointF m_offset;
};

#endif /* defined(__CurveEditor__PointView__) */
