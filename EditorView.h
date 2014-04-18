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

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QResizeEvent;
class QWheelEvent;
QT_END_NAMESPACE

class CurveView;
class CurveModel;
class EditorModel;
class ScrollPositionKeeper;

class EditorView : public QGraphicsView
{
    Q_OBJECT
    
public:
    EditorView(std::shared_ptr<EditorModel> model, QWidget* parent = nullptr);
    ~EditorView();

public slots:
    void curveAdded(std::shared_ptr<CurveModel> curve);
    void curveRemoved(std::shared_ptr<CurveModel> curve);
    
    void timeRangeChanged(RangeF timeRange);
    
    void duplicateSelectedPoints();
    void removeSelectedPoints();
    
private:
    void contextMenuEvent(QContextMenuEvent *event);
    void resizeEvent(QResizeEvent* event);
    void wheelEvent(QWheelEvent* event);
    
    void updateTransformation();
    
    QGraphicsScene* m_scene;
    QGraphicsItem* m_sceneLayer;
    std::shared_ptr<EditorModel> m_model;
    
    qreal m_timeScale;
    ScrollPositionKeeper* m_horizontalScrollKeeper;

    using Container = QMap<std::shared_ptr<CurveModel>, CurveView*>;
    using Iterator = Container::Iterator;
    using ConstIterator = Container::ConstIterator;
    Container m_curveViews;
};

#endif /* defined(__CurveEditor__EditorView__) */
