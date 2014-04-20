//
//  SceneModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 19.04.14.
//
//

#ifndef __CurveEditor__SceneModel__
#define __CurveEditor__SceneModel__

#include "RangeF.h"
#include <QObject>
#include <QList>

class CurveModel;
class EditorModel;

/**
 * @brief Scene model repesenting a collection of curves within the same time frame.
 *
 * Scene controls specific editors (@see EditorModel), which can be used to display the scene.
 */
class SceneModel : public QObject
{
    Q_OBJECT

public:
    /** @brief Construct a SceneModel. */
    SceneModel();
    /** @brief Destructor. */
    ~SceneModel();

    /** @return Curves contained in this scene. */
    QList<std::shared_ptr<CurveModel>> curves() const;

    /** @return Scene time range. */
    const RangeF timeRange() const;

    /**
     * @brief getAllCurvesEditor
     * @param editor
     */
    std::shared_ptr<EditorModel> getAllCurvesEditor();
    std::shared_ptr<EditorModel> getSelectedCurvesEditor();

signals:
    /** @brief A curve wad added to the scene. */
    void curveAdded(std::shared_ptr<CurveModel> curve);
    /** @brief A curve wad removed from the scene. */
    void curveRemoved(std::shared_ptr<CurveModel> curve);

    /** @brief A curve wad selected. Note: Multiple curves can be selected simultaneously. */
    void curveSelected(std::shared_ptr<CurveModel> curve);
    /** @brief A curve wad deselected. */
    void curveDeselected(std::shared_ptr<CurveModel> curve);

    /** @brief Scene time range changed. */
    void timeRangeChanged(RangeF timeRange);

public slots:
    /**
     * @brief Add new curve to scene.
     * @param curve Curve to add
     */
    void addCurve(std::shared_ptr<CurveModel> curve);
    /**
     * @brief Remove curve from scene.
     * @param curve Curve to remove.
     */
    void removeCurve(std::shared_ptr<CurveModel> curve);
    /**
     * @brief Set scene time range.
     * @param timeRange New time range.
     */
    void setTimeRange(RangeF timeRange);

    /**
     * @brief Select a curve.
     * @param curve Curve to be selected.
     */
    void selectCurve(std::shared_ptr<CurveModel> curve);
    /**
     * @brief Deselect a curve.
     * @param curve Curve to be deselected.
     */
    void deselectCurve(std::shared_ptr<CurveModel> curve);

    /**
     * @brief Save scene curves to xml.
     *
     * A file save dialog is opened to select the output file.
     */
    void saveCurves();

    /**
     * @brief Load xml curves to scene.
     *
     * A file open dialog is opened to select the input file.
     */
    void loadCurves();

private:
    using Container = QList<std::shared_ptr<CurveModel>>;
    Container m_curves;

    RangeF m_timeRange; /**< Scene time range */

    std::shared_ptr<EditorModel> m_AllCurvesEditor; /**< Editor model containing all curves */
    std::shared_ptr<EditorModel> m_SelectedCurvesEditor; /**< Editor model containing selected curves */
};

#endif // __CurveEditor__SceneModel__