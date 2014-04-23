#ifndef BEATLINESVIEW_H
#define BEATLINESVIEW_H

#include <QObject>
#include "TransformationNode.h"
#include "RangeF.h"

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
public:
    /**
     * @brief Construct BeatLinesView
     * @param timeRange Initial time range [start, end]
     * @param timeScale Initial time scale factor
     * @param beatOffset Initial first beat offset (in seconds)
     * @param bpm Initial bpm
     * @param parent Parent item
     */
    BeatLinesView(RangeF timeRange, float timeScale, float beatOffset, float bpm, QGraphicsItem* parent);
    ~BeatLinesView();

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
    void setBeatOffset(float beatOffset);
    /**
     * @brief Update bpm
     * @param bpm New bpm
     */
    void setBpm(float bpm);

private:
    void updateBeatLines();

    RangeF m_timeRange; ///< Time range [start, end]
    float m_timeScale; ///< Time scale affecting how detailed beat lines to draw
    float m_beatOffset; ///< Offset to the first beat
    float m_bpm; ///< Beats per minute

    class BeatLineView;
    QList<std::shared_ptr<BeatLineView>> m_beatLines;
};

#endif // BEATLINESVIEW_H
