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
    BeatLineView(float position, QGraphicsItem* parent);
    ~BeatLineView();

    void setPos(float pos);
    void setText(const QString& str);

private:
    QGraphicsTextItem* m_text;
    QGraphicsLineItem* m_line;
};

BeatLineView::BeatLineView(float position, QGraphicsItem* parent)
: 	TransformationNode(parent),
    m_text(new QGraphicsTextItem(this)),
    m_line(new QGraphicsLineItem(0, 0, 0, 1, this))
{
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);

    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    m_line->setPen(pen);

    setPos(position);
}

BeatLineView::~BeatLineView()
{}

void BeatLineView::setPos(float pos)
{
    TransformationNode::setPos(pos, 0);
}

void BeatLineView::setText(const QString& str)
{
    m_text->setPlainText(str);
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////

class BeatLinesView :
    public QObject,
    private TransformationNode
{
public:
    BeatLinesView(QGraphicsItem* parent);
    ~BeatLinesView();

public slots:
    void setTimeScale(RangeF timeScale);

private:
    RangeF m_timeScale;
    QList<BeatLineView*> m_beatLines;
};

BeatLinesView::BeatLinesView(QGraphicsItem* parent) :
    TransformationNode(parent)
{
    // Set transformation to scale y-axis to [0, 1] range
    qreal maxi = 1;
    qreal mini = 0;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);

    for (int i = 0; i < 1000; i += 100)
    {
        BeatLineView* beatLine = new BeatLineView(i, this);
        m_beatLines.push_back(beatLine);
    }
}

BeatLinesView::~BeatLinesView()
{
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
	m_timeScale(1),
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
    
	QList<float> scaleLines;
    scaleLines.push_back(-100);
    scaleLines.push_back(-50);
    scaleLines.push_back(0);
    scaleLines.push_back(50);
    scaleLines.push_back(100);
    
    ScaleView* scaleView = new ScaleView(scaleLines, m_sceneLayer);
    connect(m_model.get(), &EditorModel::timeRangeChanged, scaleView, &ScaleView::setTimeScale);
    scaleView->setTimeScale(m_model->timeRange());
    
    BeatLinesView* beatView = new BeatLinesView(m_sceneLayer);

    // Set view transformation
    QTransform transform;
    transform.scale(1, -1);
    setTransform(transform);
    
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    connect(m_model.get(), &EditorModel::curveAdded, this, &EditorView::curveAdded);
    connect(m_model.get(), &EditorModel::curveRemoved, this, &EditorView::curveRemoved);
//    connect(m_model.get(), &EditorModel::timeRangeChanged, this, &EditorView::timeRangeChanged);
    
    for (auto &curveModel : m_model->curves())
        curveAdded(curveModel);
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

void EditorView::timeRangeChanged(RangeF)
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
    
    if (newTimeScale != m_timeScale)
    {
    	m_timeScale = newTimeScale;
        updateTransformation();
    }
    
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
