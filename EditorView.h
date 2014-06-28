//
//  EditorView.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include "RangeF.h"
#include <QObject>
#include <QMap>
#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QCheckBox;
QT_END_NAMESPACE

class CurveViewAbs;
class BeatLinesView;
class CurveModelAbs;
class CurveModel;
class StepCurveModel;
class EditorModel;
class ScrollPositionKeeper;
class EditorGraphicsView;
class ScaleView;

/**
 * @brief EditorView displays the contents of an EditorModel (@see EditorModel) and
 * handles the UI events.
 */
class EditorView : public QWidget
{
    Q_OBJECT
    
public:
    /**
     * @brief Construct an EditorView.
     * @param model Associated editor model
     * @param parent Optional parent widget
     */
    EditorView(std::shared_ptr<EditorModel> model, QWidget* parent = nullptr);

    /** Destrcutor */
    ~EditorView();

signals:
    /** @brief View time scaling changed. */
    void timeScaleChanged(float timeScale);

    /**
     * @brief New curve was added through the editor view.
     * @param curve The curve
     */
    void newCurveAdded(std::shared_ptr<CurveModelAbs> curve);

public slots:
    /**
     * @brief Create new point for each selected point in respective curves.
     */
    void duplicateSelectedPoints();

    /**
     * @brief Remove selected points from respective curves.
     */
    void removeSelectedPoints();

private slots: /** Signals from EditorModel */
    /**
     * @brief New curve was added
     * @param curve The curve
     */
    void curveAdded(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief A curve was removed
     * @param curve The curve
     */
    void curveRemoved(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief Editor model time range changed.
     * @param timeRange New time range
     */
    void timeRangeChanged(RangeF timeRange);

    /** @brief New curve was selected to be added from a context menu. */
    void addNewCurve();
    void addNewStepCurve();

private:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
    
    std::shared_ptr<EditorModel> m_model; /**< Model */

    EditorGraphicsView* m_view; /**< View to QGraphicsScene */
    BeatLinesView* m_beatView; /**< View for vertical beat lines */
    ScaleView* m_scaleView; /**< View for horizontal scale lines */
    QCheckBox* m_snapToGrid; /**< "Snap" checkbox */

    /** Curve container provides mapping between curve models and views. */
    using Container = QMap<std::shared_ptr<CurveModelAbs>, CurveViewAbs*>;
    using Iterator = Container::Iterator;
    using ConstIterator = Container::ConstIterator;

    Container m_curveViews;

    /** Helpers for common handling of different types of curves and views */
    template <class T, class U> void internalCurveAdded(std::shared_ptr<U> curve);
    void internalCurveRemoved(std::shared_ptr<CurveModelAbs> curve);
};

#endif /* EDITORVIEW_H */
