//
//  SceneModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 19.04.14.
//
//

#ifndef __CurveEditor__SceneModel__
#define __CurveEditor__SceneModel__

#include "PointId.h"
#include "RangeF.h"
#include <QObject>
#include <QList>

class CurveModel;
class EditorModel;

QT_BEGIN_NAMESPACE
class QXmlStreamReader;
class QXmlStreamWriter;
QT_END_NAMESPACE

/**
 * @brief Scene model repesenting a collection of curves within the same time frame.
 *
 * Scene controls specific editors (@see EditorModel), which can be used to display the scene.
 */
class SceneModel : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a SceneModel.
     * @param timeRange Initial scene time range
     */
    SceneModel(RangeF timeRange = RangeF());
    /** @brief Destructor. */
    ~SceneModel();

    /**
     * @brief Deserialize new scenemodel from xml stream.
     * @param stream The stream
     * @return Deserialized scene model or null object if creation failed
     */
    static std::shared_ptr<SceneModel> create(QXmlStreamReader& stream);

    /** @return Curves contained in this scene. */
    QList<std::shared_ptr<CurveModel>> curves() const;

    /** @return Scene time range. */
    const RangeF timeRange() const;

    /** @return Offset to first beat in seconds. */
    double beatOffset() const;
    /** @return Rhythm in beats per minute. */
    double bpm() const;

    /** @return An editor model that will contain all curves in the scene. */
    std::shared_ptr<EditorModel> getAllCurvesEditor() const;
    /** @return An editor model that will contain selected curves in the scene. */
    std::shared_ptr<EditorModel> getSelectedCurvesEditor() const;

    /** @return File name associated with the scene. */
    const QString& fileName() const;

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

    /** @brief Scene beat offset changed. */
    void beatOffsetChanged(double beatOffset);
    /** @brief Scene bpm changed. */
    void bpmChanged(double bpm);

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
     * @brief Set scene beat offset in seconds.
     * @param beatOffset New beat offset
     */
    void setBeatOffset(double beatOffset);
    /**
     * @brief Set scene beats per minute.
     * @param timeRange New bpm.
     */
    void setBpm(double bpm);

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
     * @brief Serialize scene to xml.
     * @param stream Xml output stream
     */
    void serialize(QXmlStreamWriter& stream);

    /**
     * @brief Serialize scene curves to xml.
     * @param stream Xml output stream
     */
    void serializeCurves(QXmlStreamWriter& stream);

    /**
     * @brief Set file name for the scene
     * @param fileName File name
     */
    void setFileName(const QString& fileName);

private slots:
    /**
     * @brief Notification of curve model selection change.
     *
     * Expected only from curves currently in the scene. Note: The curve sending this signal
     * is retrieved using QObject::sender().
     *
     * @param status Current selection status
     */
    void curveSelectionChanged(bool status);

    /**
     * @brief Notification of curve point being removed. Used to remove empty curves from the scene
     *
     * Expected only from curves currently in the scene. Note: The curve sending this signal
     * is retrieved using QObject::sender().
     *
     * @param point Remove point id (not used)
     */
    void curvePointRemoved(PointId point);

private:
    using Container = QList<std::shared_ptr<CurveModel>>;
    Container m_curves;

    RangeF m_timeRange; /**< Scene time range */

    double m_beatOffset; /**< Scene music start offset */
    double m_bpm; /**< Scene music's beats per minute */

    std::shared_ptr<EditorModel> m_AllCurvesEditor; /**< Editor model containing all curves */
    std::shared_ptr<EditorModel> m_SelectedCurvesEditor; /**< Editor model containing selected curves */

    QString m_fileName; /**< File name associated with this scene */
};

#endif // __CurveEditor__SceneModel__
