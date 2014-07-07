#include "EditorGraphicsView.h"
#include "TransformationNode.h"
#include "ScrollPositionKeeper.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QScrollBar>

// A custom scene to handle right-mouse click without deselecting all
class EditorGraphicsScene : public QGraphicsScene
{
public:
    EditorGraphicsScene(QObject* parent)
    :	QGraphicsScene(parent)
    {
    }

    ~EditorGraphicsScene()
    {
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton)
        {
            event->accept();
            return;
        }
        QGraphicsScene::mousePressEvent(event);
    }
};


EditorGraphicsView::EditorGraphicsView(QWidget* parent)
:	QGraphicsView(parent),
    m_timeScale(10.0f) // Default scale to 10 pixels per second
{
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::RubberBandDrag);

    // Create a scene and a root node (scene layer)
    setScene(new EditorGraphicsScene(this));
    m_sceneLayer = new TransformationNode(nullptr);
    scene()->addItem(m_sceneLayer);

    // Align scene to the left edge of the view
    setAlignment(Qt::AlignLeft);

    m_horizontalScrollKeeper = new ScrollPositionKeeper(horizontalScrollBar(), this);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set view transformation, flip y-axis
    QTransform transform;
    transform.scale(1, -1);
    setTransform(transform);

    updateSceneTransformation();
}

EditorGraphicsView::~EditorGraphicsView()
{
}

QGraphicsItem* EditorGraphicsView::sceneLayer() const
{
    return m_sceneLayer;
}

float EditorGraphicsView::timeScale() const
{
    return m_timeScale;
}

void EditorGraphicsView::setTimeScale(float timeScale)
{
    if (m_timeScale != timeScale)
    {
        m_timeScale = timeScale;
        emit timeScaleChanged(m_timeScale);

        updateSceneTransformation();
    }
}

void EditorGraphicsView::setTimeRange(RangeF timeRange)
{
    Q_UNUSED(timeRange);
    updateSceneTransformation();
}

void EditorGraphicsView::resizeEvent(QResizeEvent* event)
{
    qDebug() << "EditorGraphicsView::resizeEvent" << event->size();
    QGraphicsView::resizeEvent(event);
    updateSceneTransformation();
}

void EditorGraphicsView::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();
    qDebug() << "wheelEvent" << delta;

    // Control modifier boosts 10x
    if (event->modifiers().testFlag(Qt::ControlModifier))
        delta *= 10;

    qreal newTimeScale = m_timeScale + (delta / 1000.0);
    if (newTimeScale < 0.01)
        newTimeScale = 0.01;
    if (newTimeScale > 100)
        newTimeScale = 100;

    setTimeScale(newTimeScale);

    event->accept();
}

void EditorGraphicsView::updateSceneTransformation()
{
    if (!scene())
    {
        qWarning() << "EditorGraphicsView::updateTransformation: No scene!";
        return;
    }

    const bool scrollBarVisible = horizontalScrollBar()->isVisible();
    const int scrollBarHeight = scrollBarVisible ? horizontalScrollBar()->height() : 0;
    // contentsRect() gets the size, but doesn't take into account scrollbars taking up space, adjust accordingly
    const QSizeF newSize(contentsRect().size().width(), contentsRect().size().height() - scrollBarHeight);

    // Update scene transformation according to new size and timescale => affecs scene bounding rect
    m_sceneLayer->setTransform(QTransform::fromScale(m_timeScale, newSize.height()));
    QRectF newSceneRect = calculateSceneItemsBoundingRect();

    // Adjust scene rect to match the new view vertical size and to be positioned in (0, 0)
    newSceneRect.setTopLeft(QPoint(0, 0));
    newSceneRect.setHeight(newSize.height());
    qDebug() << "  SceneRect:" << sceneRect() << "->" << newSceneRect;
    setSceneRect(newSceneRect);
}

QRectF EditorGraphicsView::calculateSceneItemsBoundingRect() const
{
    // Bounding rects for items that ignore transformations are not calculated correctly (!?),
    // maybe related to https://bugreports.qt-project.org/browse/QTBUG-38344. Calculate scene
    // items bounding rect by manually iterating instead of "m_scene->itemsBoundingRect();"

    QRectF itemsBoundingRect;
    for (QGraphicsItem* item : scene()->items())
    {
        if (!item->isVisible())
            continue;

        QRectF boundingRect;

        if (item->flags() & QGraphicsItem::ItemIgnoresTransformations)
        {
            QTransform viewTransform = item->deviceTransform(viewportTransform());
            QRectF viewRect = viewTransform.mapRect(item->boundingRect());
            boundingRect = mapToScene(viewRect.x(), viewRect.y(), viewRect.width(), viewRect.height()).boundingRect();
        }
        else
        {
            boundingRect = item->sceneBoundingRect();
        }

        itemsBoundingRect |= boundingRect;
    }

    return itemsBoundingRect;
}
