#ifndef BEATLINESVIEW_H
#define BEATLINESVIEW_H

#include <QObject>
#include "TransformationNode.h"
#include "RangeF.h"
#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

/**
 * @brief View for vertical beat lines.
 *
 * Beat lines are drawn throughout the time range starting from beatOffset with an
 * interval defined by the bpm.
 *
 * Time scale can affect how low level beats are visible.
 */
class BeatLinesView :
    public QObject,
    private TransformationNode
{
    Q_OBJECT
public:
    /**
     * @brief Construct BeatLinesView
     * @param timeRange Initial time range [start, end]
     * @param timeScale Initial time scale factor
     * @param beatOffset Initial first beat offset (in seconds)
     * @param bpm Initial bpm
     * @param parent Parent item
     */
    BeatLinesView(RangeF timeRange, float timeScale, double beatOffset, double bpm, QGraphicsItem* parent);
    ~BeatLinesView();

    /** @return Current snap grid */
    QRectF snapGrid() const;

signals:
    /**
     * @brief Notify that beat snap grid has changed.
     * Snap grid contains an example grid cell, which can be used to determine closest snap position for any point.
     *
     * @param gridRect New snap grid
     */
    void snapGridChanged(QRectF gridRect);

public slots:
    /**
     * @brief Update time range
     * @param timeRange New time range
     */
    void setTimeRange(RangeF timeRange);
    /**
     * @brief Update time scale
     * @param timeScale New time scale
     */
    void setTimeScale(float timeScale);
    /**
     * @brief Update beat offset
     * @param beatOffset New beat offset
     */
    void setBeatOffset(double beatOffset);
    /**
     * @brief Update bpm
     * @param bpm New bpm
     */
    void setBpm(double bpm);

private:
    void updateBeatLines();

    RangeF m_timeRange; ///< Time range [start, end]
    float m_timeScale; ///< Time scale affecting how detailed beat lines to draw
    double m_beatOffset; ///< Offset to the first beat
    double m_bpm; ///< Beats per minute
    QRectF m_snapGridRect;

    class BeatLineView;
    QList<std::shared_ptr<BeatLineView>> m_beatLines;
};

#endif // BEATLINESVIEW_H
