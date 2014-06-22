//
//  CurveModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 28.12.13.
//
//

#ifndef CURVEMODEL_H
#define CURVEMODEL_H

#include "PointId.h"
#include "RangeF.h"
#include "CurveModelAbs.h"
#include <QObject>
#include <QDebug>

/**
 * CurveModel represents a curve as its control points.
 * Derived from QObject for signals and slots.
 */
class CurveModel : public CurveModelAbs
{
    Q_OBJECT
    
public:
    /** Kochanek-Bartels parameters for a single curve point */
    class KbParams
    {
    public:
        /** @return tension parameter */
        float tension() const;
        /** @return bias parameter */
        float bias() const;
        /** @return continuity parameter */
        float continuity() const;

    private:
        KbParams(float tension = 0.0f, float bias = 0.0f, float continuity = 0.0f);
        void update(float tension, float bias, float continuity);

        bool operator==(KbParams const& other) const;
        bool operator!=(KbParams const& other) const;

        // Allow curve to create parameters
        friend class CurveModel;

    private:
        float m_tension;
        float m_bias;
        float m_continuity;
    };

public:
    /**
     * @brief Construct a CurveModel
     * @param name Curve name
     */
    CurveModel(const QString& name);

    ~CurveModel();

    /**
     * @return Copy of a point params with the given id. In case an invalid or
     * non-existent id is given default params are returned.
     */
    const KbParams params(PointId id) const;

    /** @return Curve value range [min, max]. */
    RangeF valueRange() const;

signals:
    /** @brief Curve value range changed. */
    void valueRangeChanged(RangeF newRange);

public slots:
    /**
     * @brief Update parameters for an existing point
     *
     * @param id Point id
     * @param tension New tension
     * @param bias New bias
     * @param continuity New continuity
     *
     * @par No modifications are made if the id is invalid
     */
    void updatePointParams(PointId id, float tension, float bias, float continuity);

    /**
     * @brief Set curve value range (y-axis)
     * @param newRange New value range
     */
    void setValueRange(RangeF newRange);

protected:
    virtual CurveModel* getAsSplineCurve() override;

private:
    using PointContainer = QMultiMap<float, Point>;
    using ParamContainer = QMap<PointId, KbParams>;
    
    virtual bool addPointInternal(PointId id, float time, QVariant value) override;
    virtual void removePointInternal(PointId id) override;

    virtual QVariant limitValueToRange(const QVariant& value) const override;

    ParamContainer m_params;
    RangeF m_valueRange;
};

inline CurveModel::KbParams::KbParams(float tension, float bias, float continuity)
  : m_tension(tension), m_bias(bias), m_continuity(continuity)
{
}
inline float CurveModel::KbParams::tension() const
{
    return m_tension;
}
inline float CurveModel::KbParams::bias() const
{
    return m_bias;
}
inline float CurveModel::KbParams::continuity() const
{
    return m_continuity;
}
inline void CurveModel::KbParams::update(float tension, float bias, float continuity)
{
    m_tension = tension;
    m_bias = bias;
    m_continuity = continuity;
}
inline bool CurveModel::KbParams::operator==(KbParams const& other) const
{
    return
        (m_tension == other.m_tension) &&
        (m_bias == other.m_bias) &&
        (m_continuity == other.m_continuity);
}
inline bool CurveModel::KbParams::operator!=(KbParams const& other) const
{
    return !((*this)==other);
}

#endif /* CURVEMODEL_H */
