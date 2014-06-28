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
#include "StepCurveModel.h"
#include "CurveView.h"
#include "StepCurveView.h"
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
#include <QCheckBox>

EditorView::EditorView(std::shared_ptr<EditorModel> model, QWidget* parent)
  : QWidget(parent),
    m_model(model)
{
    QGridLayout* gridLayout = new QGridLayout(this);
    setLayout(gridLayout);

    m_view = new EditorGraphicsView(this);

    m_snapToGrid = new QCheckBox("Snap", this);

    const int numberOfScaleLines = 5;
    m_scaleView = new ScaleView(numberOfScaleLines, m_model->timeRange(), m_view->sceneLayer());
    connect(m_model.get(), &EditorModel::timeRangeChanged, m_scaleView, &ScaleView::setTimeRange);

    m_beatView = new BeatLinesView(m_model->timeRange(), m_view->timeScale(), m_model->beatOffset(), m_model->bpm(), m_view->sceneLayer());
    connect(m_model.get(), &EditorModel::timeRangeChanged, m_beatView, &BeatLinesView::setTimeRange);
    connect(m_view, &EditorGraphicsView::timeScaleChanged, m_beatView, &BeatLinesView::setTimeScale);
    connect(m_model.get(), &EditorModel::beatOffsetChanged, m_beatView, &BeatLinesView::setBeatOffset);
    connect(m_model.get(), &EditorModel::bpmChanged, m_beatView, &BeatLinesView::setBpm);
    connect(this, &EditorView::newCurveAdded, m_model.get(), &EditorModel::handleRequestToAddNewCurve);

    connect(m_model.get(), &EditorModel::curveAdded, this, &EditorView::curveAdded);
    connect(m_model.get(), &EditorModel::curveRemoved, this, &EditorView::curveRemoved);
    connect(m_model.get(), &EditorModel::timeRangeChanged, this, &EditorView::timeRangeChanged);

    gridLayout->addWidget(m_view, 0, 0);
    gridLayout->addWidget(m_snapToGrid, 0, 1);

    for (auto &curveModel : m_model->curves())
        curveAdded(curveModel);
}

EditorView::~EditorView()
{
}

void EditorView::curveAdded(std::shared_ptr<CurveModelAbs> curve)
{
    std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);
    std::shared_ptr<StepCurveModel> stepCurve = CurveModelAbs::getAsStepCurve(curve);

    if (splineCurve)
        internalCurveAdded<CurveView>(splineCurve);
    else if (stepCurve)
        internalCurveAdded<StepCurveView>(stepCurve);
    else
        qWarning() << "Trying to add unknown curve type";
}

template <class T, class U>
void EditorView::internalCurveAdded(std::shared_ptr<U> curve)
{
    Iterator existing = m_curveViews.find(curve);
    assert(existing == m_curveViews.end());

    // Create new curve view
    CurveViewAbs* curveView = T::create(curve, m_view->sceneLayer());

    // Setup snap grid synching to beat lines
    connect(m_snapToGrid, &QCheckBox::toggled, curveView, &CurveViewAbs::setSnapToGrid);
    connect(m_beatView, &BeatLinesView::snapGridChanged, curveView, &CurveViewAbs::setSnapGrid);
    curveView->setSnapToGrid(m_snapToGrid->checkState() == Qt::Checked);
    curveView->setSnapGrid(m_beatView->snapGrid());

    m_curveViews.insert(curve, curveView);
}

void EditorView::curveRemoved(std::shared_ptr<CurveModelAbs> curve)
{
    internalCurveRemoved(curve);
}

void EditorView::internalCurveRemoved(std::shared_ptr<CurveModelAbs> curve)
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

    const float timeRangeSize = timeRange.max - timeRange.min;

    // New curve with value range [0, 1]
    std::shared_ptr<CurveModel> newCurve = std::make_shared<CurveModel>("New curve");
    newCurve->setValueRange(RangeF(0, 1));

    // Add points to 1/6 and 5/6 time
    newCurve->addPoint(timeRange.min + timeRangeSize / 6.0, 0.5);
    newCurve->addPoint(timeRange.min + timeRangeSize * 5.0 / 6.0, 0.5);

    emit newCurveAdded(newCurve);
}

void EditorView::addNewStepCurve()
{
    RangeF timeRange = m_model->timeRange();
    if (!timeRange.isValid())
    {
        qWarning() << "Cannot add new curve without valid timerange";
        return;
    }

    const float timeRangeSize = timeRange.max - timeRange.min;

    // New curve with value range [0, 1]
    std::shared_ptr<StepCurveModel> newCurve = std::make_shared<StepCurveModel>("New step curve");

    StepCurveModel::Options options;
    options.insert(1, "Uno");
    options.insert(2, "Dos");
    options.insert(3, "Tres");
    newCurve->setOptions(options);

    // Add points to 1/6 and 5/6 time
    newCurve->addPoint(timeRange.min + timeRangeSize / 6.0, 2);
    newCurve->addPoint(timeRange.min + timeRangeSize * 5.0 / 6.0, 2);

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
        curveMenu.addAction("Add new step curve", this, SLOT(addNewStepCurve()));
        curveMenu.exec(event->globalPos());
    }
}
