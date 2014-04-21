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

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QAction>
#include <QScrollBar>

#include <QPen>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

class BeatLineView : private TransformationNode
{
public:
    BeatLineView(float position, int barIndex, int beatIndex, QGraphicsItem* parent);
    ~BeatLineView();

private:
    QGraphicsSimpleTextItem* m_text;
    QGraphicsLineItem* m_line;
};

BeatLineView::BeatLineView(float position, int barIndex, int beatIndex, QGraphicsItem* parent)
: 	TransformationNode(parent),
    m_text(new QGraphicsSimpleTextItem(QString("%1.%2").arg(barIndex).arg(beatIndex), this)),
    m_line(new QGraphicsLineItem(0, 0, 0, 1, this))
{
    QFont smaller(m_text->font());
    smaller.setPointSize(8);
    m_text->setFont(smaller);
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    m_text->setVisible(false); // TODO: Don't show texts for now
    m_text->setPos(0, 1);

    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    // Emphasize the first beat in bar
    if (beatIndex == 1) pen.setStyle(Qt::SolidLine);
    m_line->setPen(pen);

    setPos(position, 0);
}

BeatLineView::~BeatLineView()
{}

///////////////////////////////////////////////////
///////////////////////////////////////////////////

class BeatLinesView :
    public QObject,
    private TransformationNode
{
public:
    BeatLinesView(RangeF timeRange, float timeScale, float beatOffset, float bpm, QGraphicsItem* parent);
    ~BeatLinesView();

public slots:
    void setTimeRange(RangeF timeRange);
    void setTimeScale(float timeScale);
    void setBeatOffset(float beatOffset);
    void setBpm(float bpm);

private:
    void updateBeatLines();

    RangeF m_timeRange; ///< Time range [start, end]
    float m_timeScale; ///< Time scale affecting how detailed beat lines to draw
    float m_beatOffset; ///< Offset to the first beat
    float m_bpm; ///< Beats per minute

    QList<std::shared_ptr<BeatLineView>> m_beatLines;
};

BeatLinesView::BeatLinesView(RangeF timeRange, float timeScale, float beatOffset, float bpm, QGraphicsItem* parent)
  : TransformationNode(parent),
    m_timeRange(timeRange),
    m_timeScale(timeScale),
    m_beatOffset(beatOffset),
    m_bpm(bpm)
{
    // Set transformation to scale y-axis to [0, 1] range
    qreal maxi = 1;
    qreal mini = 0;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);

    updateBeatLines();
}

BeatLinesView::~BeatLinesView()
{
}

void BeatLinesView::setTimeScale(float timeScale)
{
    m_timeScale = timeScale;
    updateBeatLines();
}

void BeatLinesView::setTimeRange(RangeF timeRange)
{
    m_timeRange = timeRange;
    updateBeatLines();
}

void BeatLinesView::setBeatOffset(float beatOffset)
{
    m_beatOffset = beatOffset;
    updateBeatLines();
}

void BeatLinesView::setBpm(float bpm)
{
    m_bpm = bpm;
    updateBeatLines();
}

void BeatLinesView::updateBeatLines()
{
    // Remove old lines
    m_beatLines.clear();

    // Assumer 4/4 time signature
    const int beatsPerBar = 4;
    int barIndex = 1;
    int beatIndex = 1;

    float beatStep = 60.0f / m_bpm;
    QGraphicsItem* thisAsGraphicsItem = this;

    for (float beat = m_timeRange.min + m_beatOffset;
         beat < m_timeRange.max; beat += beatStep)
    {
        m_beatLines.push_back(std::make_shared<BeatLineView>(beat, barIndex, beatIndex, thisAsGraphicsItem));

        if (++beatIndex > beatsPerBar)
        {
            // Current bar ended
            beatIndex = 1;
            ++barIndex;
        }
    }
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

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
    
	m_sceneLayer = new TransformationNode(NULL);
    m_scene->addItem(m_sceneLayer);
    
    ScaleView* scaleView = new ScaleView(5, m_model->timeRange(), m_sceneLayer);
    connect(m_model.get(), &EditorModel::timeRangeChanged, scaleView, &ScaleView::setTimeRange);

    BeatLinesView* beatView = new BeatLinesView(
        m_model->timeRange(), m_timeScale, m_model->beatOffset(), m_model->bpm(), m_sceneLayer);
    connect(m_model.get(), &EditorModel::timeRangeChanged, beatView, &BeatLinesView::setTimeRange);
    connect(this, &EditorView::timeScaleChanged, beatView, &BeatLinesView::setTimeScale);
    connect(m_model.get(), &EditorModel::beatOffsetChanged, beatView, &BeatLinesView::setBeatOffset);
    connect(m_model.get(), &EditorModel::bpmChanged, beatView, &BeatLinesView::setBpm);

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
    
  	m_curveViews.insert(curve, new CurveView(m_sceneLayer, curve));
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
    for (auto &curveView : m_curveViews)
        curveView->duplicateSelectedPoints();
}

void EditorView::removeSelectedPoints()
{
    for (auto &curveView : m_curveViews)
        curveView->removeSelectedPoints();
}

void EditorView::contextMenuEvent(QContextMenuEvent* event)
{
    if (!scene())
        return;
    
	auto selectedItems = scene()->selectedItems();
    
	if (selectedItems.size() > 0)
    {
        QMenu menu;
    	menu.addAction(selectedItems.size() > 1 ? "Add points" : "Add point", this, SLOT(duplicateSelectedPoints()));
	    menu.addAction(selectedItems.size() > 1 ? "Remove points" : "Remove point", this, SLOT(removeSelectedPoints()));
        menu.exec(event->globalPos());
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
    const int horizontalScrollBarHeight = 15;
    
    QSizeF newSize(contentsRect().size().width(), contentsRect().size().height());

    bool sceneFitsInViewBefore = m_scene->sceneRect().width() <= newSize.width();
    
    //  Update scene transformation according to new size and timescale => affecs scene rect
    {
        qreal xScale = m_timeScale;
        qreal yScale = newSize.height();
        if (!sceneFitsInViewBefore)
            yScale -= horizontalScrollBarHeight;
        
        m_sceneLayer->setTransform(QTransform::fromScale(xScale, yScale));
    }

    QRectF sceneRectAfter = m_scene->itemsBoundingRect();
    QRectF newSceneRect(0, 0, qMax(sceneRectAfter.width(), newSize.width()), newSize.height());

    bool sceneFitsInViewAfter = sceneRectAfter.width() <= newSize.width();
    
    //  Update scene transformation again if need for horizontalScrollBar has changed
    if (sceneFitsInViewBefore != sceneFitsInViewAfter)
    {
        qreal xScale = m_timeScale;
        qreal yScale = newSize.height();
        if (!sceneFitsInViewAfter)
            yScale -= horizontalScrollBarHeight;

        m_sceneLayer->setTransform(QTransform::fromScale(xScale, yScale));
    }
    
    m_scene->setSceneRect(newSceneRect);
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
