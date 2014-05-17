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

/** View for a single curve point. */
class PointView :
	public QObject,
	private TransformationNode,
	private GraphicsItemDelegate
{
    Q_OBJECT
    
public:
    /**
     * @brief Construct PointView
     * @param point Initial point
     * @param parent Parent Item
     */
    PointView(Point const& point, QGraphicsItem* parent);
    ~PointView();
    
    /**
     * @brief Update point model
     * @param pointModel New point
     */
    void setPoint(Point const& point);

    /** @return Get point id */
    PointId pointId() const;
    
public:
    /** Expose QGraphicsView::pos() */
    using TransformationNode::pos;
    /** @return True if point is selected */
    bool isSelected() const;
    
signals:
    /**
     * @brief Point (view) position has changed
     * @param id Point id
     * @param time New point time
     * @param value New point value
     */
    void pointPositionChanged(PointId id, float time, QVariant value);

    /**
     * @brief Point (view) selection state has changed
     * @param id Point id
     * @param isSelected True if the point index was selected, false if deselected.
     */
    void pointSelectedChanged(PointId id, bool isSelected);

public slots:
    /**
     * @brief Set snap grid. A valid snap grid restricts possible point positions.
     * @param gridRect New snap grid.
     */
    void setSnapGrid(QRectF gridRect);

private: // Implementation for GraphicsItemDelegate
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveStartEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemMoveEndEvent* event) override;
    void handleGraphicsItemEvent(QGraphicsItem* item, GraphicsItemSelectedEvent* event) override;

    bool handleItemChange(QGraphicsItem* item, QGraphicsItem::GraphicsItemChange change, const QVariant& value, QVariant& output) override;
    
private:
    QGraphicsItem* m_item;
    QGraphicsSimpleTextItem* m_text;
    Point m_point;
    QPointF m_offset; //< Used to keep relative point grab position when dragging the point
    QRectF m_gridRect;
};

#endif /* defined(__CurveEditor__PointView__) */
