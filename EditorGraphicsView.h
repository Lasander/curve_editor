#ifndef EDITORGRAPHICSVIEW_H
#define EDITORGRAPHICSVIEW_H

#include "RangeF.h"
#include <QGraphicsView>

class ScrollPositionKeeper;

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

/**
 * @brief QGraphicsView for an editor.
 * Creates a QGraphicsScene and a root QGraphicsItem (scene layer).
 * The view manages scaling the scene to fit the view vertically and
 * also the time scaling, which can be affected with mouse wheel.
 */
class EditorGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief Construct EditorGraphicsView
     * @param parent Parent widget
     */
    EditorGraphicsView(QWidget* parent = 0);
    ~EditorGraphicsView();

    /** @return Scene root item. Scene items should be added under this item. */
    QGraphicsItem* sceneLayer() const;

    /** @return View time (x-axis) scale factor. */
    float timeScale() const;

signals:
    /**
     * @brief View time scale changed
     * @param timeScale New time scale
     */
    void timeScaleChanged(float timeScale);

public slots:
    /**
     * @brief Set view time scale
     * @param timeScale New time scale
     */
    void setTimeScale(float timeScale);

    /**
     * @brief Set view time range [min, max]
     * @param timeRange New time range
     */
    void setTimeRange(RangeF timeRange);

private:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    void updateSceneTransformation();
    QRectF calculateSceneItemsBoundingRect() const;

private:
    QGraphicsItem* m_sceneLayer;
    float m_timeScale;
    ScrollPositionKeeper* m_horizontalScrollKeeper;
};

#endif // EDITORGRAPHICSVIEW_H
