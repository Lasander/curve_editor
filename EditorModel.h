//
//  EditorModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 31.12.13.
//
//

#ifndef EDITORMODEL_H
#define EDITORMODEL_H

#include "RangeF.h"
#include <QObject>
#include <QList>
#include <memory>

class CurveModelAbs;
class CurveModel;
class StepCurveModel;

/**
 * @brief Editor model groups curves to be displayed in a CurveView.
 *
 * Few EditorModel instances are contained within SceneModel (@see SceneModel).
 */
class EditorModel : public QObject
{
    Q_OBJECT
    
public:
    /**
     * @brief Construct an EditorModel.
     * @param timeRange Initial editor time range
     * @param beatOffset Initial editor beat offset
     * @param bpm Initial editor bpm
     */
    EditorModel(RangeF timeRange, double beatOffset, double bpm);
    /** Destructor */
    ~EditorModel();
    
    /** @return List of curves contained in this editor. */
    QList<std::shared_ptr<CurveModelAbs>> curves() const;

    /** @return Editor time range. */
    const RangeF timeRange() const;

    /** @return Editor beat offset. */
    double beatOffset() const;
    /** @return Editor bpm. */
    double bpm() const;
    
signals:
    /**
     * @brief A curve was associated with this editor.
     * @param curve The curve
     */
    void curveAdded(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief A curve was disassociated from this editor.
     * @param curve The curve
     */
    void curveRemoved(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief The editor time range was changed.
     * @param timeRange New time range
     */
    void timeRangeChanged(RangeF timeRange);
    
    /**
     * @brief Editor beat offset changed.
     * @param beatOffset New beat offset
     */
    void beatOffsetChanged(double beatOffset);
    /**
     * @brief Editor bpm changed.
     * @param beatOffset New bpm
     */
    void bpmChanged(double bpm);

    /**
     * @brief Request to add a new curve.
     * @param curve Curve to be added.
     */
    void requestToAddNewCurve(std::shared_ptr<CurveModelAbs> curve);

public slots:
    /**
     * @brief Associate a curve with this editor.
     * @param curve The curve
     */
    void addCurve(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief Disassociate a curve from this editor.
     * @param curve The curve
     */
    void removeCurve(std::shared_ptr<CurveModelAbs> curve);

    /**
     * @brief Disassociate all curves from this editor.
     */
    void clearCurves();

    /**
     * @brief Set editor time range.
     * @param timeRange New time range
     */
    void setTimeRange(RangeF timeRange);
    
    /**
     * @brief Set editor beat offset in seconds.
     * @param beatOffset New beat offset
     */
    void setBeatOffset(double beatOffset);
    /**
     * @brief Set editor beats per minute.
     * @param timeRange New bpm.
     */
    void setBpm(double bpm);

    /**
     * @brief Request new curve to be added. Relay the request through signal requestToAddNewCurve().
     * @param curve Curve to be added.
     */
    void handleRequestToAddNewCurve(std::shared_ptr<CurveModelAbs> curve);

private:
    /** Curve containers */
    using Container = QList<std::shared_ptr<CurveModelAbs>>;
    Container m_curves;

    /** Private helpers */
    bool addCurveInternal(std::shared_ptr<CurveModelAbs> curve);
    bool removeCurveInternal(std::shared_ptr<CurveModelAbs> curve);

    RangeF m_timeRange; /**< Time range */
    double m_beatOffset; /**< Scene music start offset */
    double m_bpm; /**< Scene music's beats per minute */
};

#endif /* EDITORMODEL_H */
