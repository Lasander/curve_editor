#ifndef CURVEVIEWABS_H
#define CURVEVIEWABS_H

#include "TransformationNode.h"
#include "PointId.h"
#include <QMap>
#include <QObject>
#include <QVariant>
#include <memory>

class PointView;
class CurveModelAbs;

/** Abstract base class for curve views. Takes care of common curve view functions. */
class CurveViewAbs :
    public QObject,
    protected TransformationNode
{
    Q_OBJECT

protected:
    /**
     * @brief Protected constructor for CurveViewAbs. Used by derived classes.
     * @param model Curve model
     * @param parent Parent item
     */
    CurveViewAbs(std::shared_ptr<CurveModelAbs> model, QGraphicsItem* parent);

    /**
     * @brief Finish two-phase initialization. Connected model to the view.
     */
    void init();

public:
    ~CurveViewAbs();

signals:
    /**
     * @brief Curve snap grid has changed
     * @param gridRect New snap grid
     */
    void snapGridChanged(QRectF gridRect);

public slots:
    /**
     * @brief Set curve snap grid
     * @param gridRect New snap grid
     */
    void setSnapGrid(QRectF gridRect);

    /**
     * @brief Set snap-to-grid
     * @param snapToGrid True if curve should snap to define snap grid.
     */
    void setSnapToGrid(bool snapToGrid);
    /**
     * @brief Change curve highlighting
     * @param highlight True for highlight
     */
    void highlightCurve(bool highlight);

    /** @brief Add new points corresponding to all selected points in the curve. */
    void duplicateSelectedPoints();

    /** @brief Remove all selected points in the curve. */
    void removeSelectedPoints();

private slots:
    /**
     * @brief Add point to the view
     * @param id Added point
     */
    void addPoint(PointId id);
    /**
     * @brief Update existing point in the view
     * @param id Updated point
     */
    void updatePoint(PointId id);
    /**
     * @brief Remove point from the view
     * @param id Removed point
     */
    void removePoint(PointId id);

protected:
    /** @return current effective snap grid. Affected also by whether snapping is enabled.*/
    QRectF getSnapGrid() const;

    /** @return true if curve is highlighted */
    bool isHighlighted() const;

    /** Notification to derived class to re-draw the curve. */
    virtual void updateCurves() = 0;

    /**
     * @brief Chance for derived class to perform operations on point add.
     * @param id Added point
     * @return True if internal point adding succeeded. If it failed point won't be added.
     */
    virtual bool internalAddPoint(PointId id) = 0;
    /**
     * @brief Chance for derived class to perform operations on point update.
     * @param id Updated point
     * @return True if internal point update succeeded. If it failed point won't be updated.
     */
    virtual bool internalUpdatePoint(PointId id) = 0;
    /**
     * @brief Chance for derived class to perform operations on point remove.
     * @param id Removed point
     * @return True if internal point remove succeeded. The point will be remove regardless of the result.
     */
    virtual bool internalRemovePoint(PointId id) = 0;

    /**
     * @brief Get time and value for a new point to be duplicated from an existing point.
     * @param id Duplicated point id
     * @param nextId Next point id from the duplicated point (invalid if no next point)
     * @return Time-value pair for the new point.
     */
    virtual std::pair<float, QVariant> placeNewPoint(PointId id, PointId nextId) const = 0;

private:
    PointView* findPointView(PointId id) const;

    std::shared_ptr<CurveModelAbs> m_model;
    QMap<PointId, PointView*> m_pointViews;

    QRectF m_snapGridRect;
    bool m_snapToGrid;

    bool m_highlightCurve;
};


#endif // CURVEVIEWABS_H
