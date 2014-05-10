//
//  EditorView.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 27.12.13.
//
//

#ifndef __CurveEditor__EditorView__
#define __CurveEditor__EditorView__

#include "RangeF.h"
#include <QObject>
#include <QGraphicsView>
#include <QMap>
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QResizeEvent;
class QWheelEvent;
QT_END_NAMESPACE

class CurveView;
class BeatLinesView;
class CurveModel;
class EditorModel;
class ScrollPositionKeeper;

/**
 * @brief EditorView displays the contents of an EditorModel (@see EditorModel) and
 * handles the UI events.
 */
class EditorView : public QGraphicsView
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
    void newCurveAdded(std::shared_ptr<CurveModel> curve);

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
    void curveAdded(std::shared_ptr<CurveModel> curve);
    /**
     * @brief A curve was removed
     * @param curve The curve
     */
    void curveRemoved(std::shared_ptr<CurveModel> curve);

    /**
     * @brief Editor model time range changed.
     * @param timeRange New time range
     */
    void timeRangeChanged(RangeF timeRange);

    /** @brief New curve was selected to be added from a context menu. */
    void addNewCurve();

private:
    void contextMenuEvent(QContextMenuEvent *event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    
    void setTimeScale(qreal timeScale);
    void updateTransformation();
    QRectF calculateSceneItemsBoundingRect() const;
    
    QGraphicsScene* m_scene;
    QGraphicsItem* m_sceneLayer;
    BeatLinesView* m_beatView;
    std::shared_ptr<EditorModel> m_model;
    
    qreal m_timeScale;
    ScrollPositionKeeper* m_horizontalScrollKeeper;

    /** Curve container provides mapping between curve models and views. */
    using Container = QMap<std::shared_ptr<CurveModel>, CurveView*>;
    using Iterator = Container::Iterator;
    using ConstIterator = Container::ConstIterator;
    Container m_curveViews;
};

#endif /* defined(__CurveEditor__EditorView__) */
