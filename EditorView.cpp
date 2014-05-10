//
//  EditorView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#include "EditorView.h"
#include "EditorModel.h"
#include "TransformationNode.h"
#include "CurveModel.h"
#include "CurveView.h"
#include "ScaleView.h"
#include "ScrollPositionKeeper.h"
#include "BeatLinesView.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>
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

EditorView::EditorView(std::shared_ptr<EditorModel> model, QWidget* parent) :
	QGraphicsView(parent),
	m_model(model),
    m_timeScale(10), // Default to 10 pixels per second
	m_horizontalScrollKeeper(new ScrollPositionKeeper(this->horizontalScrollBar(), this))
{
    setRenderHint(QPainter::Antialiasing, true);
    setDragMode(QGraphicsView::RubberBandDrag);
//    editor->setOptimizationFlags(QGraphicsView::DontSavePainterState);
//    editor->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
//    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    
    m_scene = new EditorGraphicsScene(this);
    setScene(m_scene);
    // Align scene to the left edge
    setAlignment(Qt::AlignLeft);
    
	m_sceneLayer = new TransformationNode(NULL);
    m_scene->addItem(m_sceneLayer);
    
    const int numberOfScaleLines = 5;

    ScaleView* scaleView = new ScaleView(numberOfScaleLines, m_model->timeRange(), m_sceneLayer);
    connect(m_model.get(), &EditorModel::timeRangeChanged, scaleView, &ScaleView::setTimeRange);

    BeatLinesView* beatView = new BeatLinesView(
        m_model->timeRange(), m_timeScale, m_model->beatOffset(), m_model->bpm(), m_sceneLayer);
    connect(m_model.get(), &EditorModel::timeRangeChanged, beatView, &BeatLinesView::setTimeRange);
    connect(this, &EditorView::timeScaleChanged, beatView, &BeatLinesView::setTimeScale);
    connect(m_model.get(), &EditorModel::beatOffsetChanged, beatView, &BeatLinesView::setBeatOffset);
    connect(m_model.get(), &EditorModel::bpmChanged, beatView, &BeatLinesView::setBpm);
    connect(this, &EditorView::newCurveAdded, m_model.get(), &EditorModel::handleRequestToAddNewCurve);

    // Set view transformation
    QTransform transform;
    transform.scale(1, -1);
    setTransform(transform);
    
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    connect(m_model.get(), &EditorModel::curveAdded, this, &EditorView::curveAdded);
    connect(m_model.get(), &EditorModel::curveRemoved, this, &EditorView::curveRemoved);
    connect(m_model.get(), &EditorModel::timeRangeChanged, this, &EditorView::timeRangeChanged);

    for (auto &curveModel : m_model->curves())
        curveAdded(curveModel);

    updateTransformation();
}

EditorView::~EditorView()
{
}

void EditorView::curveAdded(std::shared_ptr<CurveModel> curve)
{
    Iterator existing = m_curveViews.find(curve);
    assert(existing == m_curveViews.end());
    
    m_curveViews.insert(curve, new CurveView(curve, m_sceneLayer));
}

void EditorView::curveRemoved(std::shared_ptr<CurveModel> curve)
{
	Iterator removed = m_curveViews.find(curve);
    assert(removed != m_curveViews.end());
    
    delete removed.value();
  	m_curveViews.erase(removed);
}

void EditorView::timeRangeChanged(RangeF /*timeRange*/)
{
    updateTransformation();
}

void EditorView::duplicateSelectedPoints()
{
    for (auto curveView : m_curveViews)
        curveView->duplicateSelectedPoints();
}

void EditorView::removeSelectedPoints()
{
    // Need to iterate a copy as the original container might change as a result of removing points
    Container copyOfCurveView = m_curveViews;

    for (auto curveView : copyOfCurveView)
        curveView->removeSelectedPoints();
}

void EditorView::addNewCurve()
{
    RangeF timeRange = m_model->timeRange();
    if (!timeRange.isValid())
    {
        qWarning() << "Cannot add new curve without valid timerange";
        return;
    }

    float timeRangeSize = timeRange.max - timeRange.min;

    // New curve with value range [0, 1]
    std::shared_ptr<CurveModel> newCurve = std::make_shared<CurveModel>(1, "New curve");
    newCurve->setValueRange(RangeF(0, 1));

    // Add points to 1/6 and 5/6 time
    newCurve->addPoint(timeRange.min + timeRangeSize/6.0, {0.5});
    newCurve->addPoint(timeRange.min + timeRangeSize * 5.0/6.0, {0.5});

    emit newCurveAdded(newCurve);
}

void EditorView::contextMenuEvent(QContextMenuEvent* event)
{
    if (!scene())
        return;
    
	auto selectedItems = scene()->selectedItems();
    
	if (selectedItems.size() > 0)
    {
        const bool multipleSelected = selectedItems.size() > 1;

        QMenu pointsMenu;
        pointsMenu.setTitle("Points");
        pointsMenu.addAction(multipleSelected ? "Add points" : "Add point", this, SLOT(duplicateSelectedPoints()));
        pointsMenu.addAction(multipleSelected ? "Remove points" : "Remove point", this, SLOT(removeSelectedPoints()));
        pointsMenu.exec(event->globalPos());
    }
    else
    {
        QMenu curveMenu;
        curveMenu.setTitle("Curves");
        curveMenu.addAction("Add new curve", this, SLOT(addNewCurve()));
        curveMenu.exec(event->globalPos());
    }
}

void EditorView::resizeEvent(QResizeEvent* event)
{
    qDebug() << "resizeEvent" << event->size();
    QGraphicsView::resizeEvent(event);
    
    updateTransformation();
    return;
}

void EditorView::wheelEvent(QWheelEvent* event)
{
    qDebug() << "wheelEvent" << event->delta();
    
    qreal newTimeScale = m_timeScale + (event->delta() / 1000.0);
    if (newTimeScale < 0.01)
        newTimeScale = 0.01;
    if (newTimeScale > 100)
        newTimeScale = 100;

    setTimeScale(newTimeScale);
    
    event->accept();
}

void EditorView::updateTransformation()
{
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
    m_scene->setSceneRect(newSceneRect);
}

QRectF EditorView::calculateSceneItemsBoundingRect() const
{
    // Bounding rects for items that ignore transformations are not calculated correctly (!?),
    // maybe related to https://bugreports.qt-project.org/browse/QTBUG-38344. Calculate scene
    // items bounding rect by manually iterating instead of "m_scene->itemsBoundingRect();"

    QRectF itemsBoundingRect;
    for (QGraphicsItem* item : m_scene->items())
    {
        if (!item->isVisible())
            continue;

        QRectF boundingRect;

        if (item->flags() & QGraphicsItem::ItemIgnoresTransformations)
        {
            QTransform viewTransform = item->deviceTransform(this->viewportTransform());
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

void EditorView::setTimeScale(qreal timeScale)
{
    if (m_timeScale != timeScale)
    {
        m_timeScale = timeScale;
        emit timeScaleChanged(static_cast<float>(m_timeScale));

        updateTransformation();
    }
}
