//
//  EditorView.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#include "EditorView.h"
#include "EditorModel.h"
#include "CurveModel.h"
#include "CurveView.h"
#include "ScaleView.h"
#include "BeatLinesView.h"
#include "EditorGraphicsView.h"

#include <QGraphicsScene>
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QScrollBar>
#include <QGridLayout>

EditorView::EditorView(std::shared_ptr<EditorModel> model, QWidget* parent)
  : QWidget(parent),
    m_model(model)
{
    QGridLayout* gridLayout = new QGridLayout(this);
    setLayout(gridLayout);

    m_view = new EditorGraphicsView(this);
    gridLayout->addWidget(m_view, 0, 0);

    const int numberOfScaleLines = 5;

    ScaleView* scaleView = new ScaleView(numberOfScaleLines, m_model->timeRange(), m_view->sceneLayer());
    connect(m_model.get(), &EditorModel::timeRangeChanged, scaleView, &ScaleView::setTimeRange);

    m_beatView = new BeatLinesView(m_model->timeRange(), m_view->timeScale(), m_model->beatOffset(), m_model->bpm(), m_view->sceneLayer());
    connect(m_model.get(), &EditorModel::timeRangeChanged, m_beatView, &BeatLinesView::setTimeRange);
    connect(m_view, &EditorGraphicsView::timeScaleChanged, m_beatView, &BeatLinesView::setTimeScale);
    connect(m_model.get(), &EditorModel::beatOffsetChanged, m_beatView, &BeatLinesView::setBeatOffset);
    connect(m_model.get(), &EditorModel::bpmChanged, m_beatView, &BeatLinesView::setBpm);
    connect(this, &EditorView::newCurveAdded, m_model.get(), &EditorModel::handleRequestToAddNewCurve);

    connect(m_model.get(), &EditorModel::curveAdded, this, &EditorView::curveAdded);
    connect(m_model.get(), &EditorModel::curveRemoved, this, &EditorView::curveRemoved);
    connect(m_model.get(), &EditorModel::timeRangeChanged, this, &EditorView::timeRangeChanged);

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
    
    // Create new curve view
    CurveView* curveView = new CurveView(curve, m_view->sceneLayer());

    // Setup snap grid synching to beat lines
    connect(m_beatView, &BeatLinesView::snapGridChanged, curveView, &CurveView::setSnapGrid);
    curveView->setSnapGrid(m_beatView->snapGrid());

    m_curveViews.insert(curve, curveView);
}

void EditorView::curveRemoved(std::shared_ptr<CurveModel> curve)
{
	Iterator removed = m_curveViews.find(curve);
    assert(removed != m_curveViews.end());

    delete removed.value();
  	m_curveViews.erase(removed);
}

void EditorView::timeRangeChanged(RangeF timeRange)
{
    m_view->setTimeRange(timeRange);
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
    newCurve->addPoint(timeRange.min + timeRangeSize / 6.0, {0.5});
    newCurve->addPoint(timeRange.min + timeRangeSize * 5.0 / 6.0, {0.5});

    emit newCurveAdded(newCurve);
}

void EditorView::contextMenuEvent(QContextMenuEvent* event)
{
    if (!m_view->scene())
        return;
    
    auto selectedItems = m_view->scene()->selectedItems();
    
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
