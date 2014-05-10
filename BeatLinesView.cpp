#include "BeatLinesView.h"

#include <QFont>
#include <QPen>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QDebug>
#include <qglobal.h>

/**
 * @brief Graphics for a single vertical beat line
 */
class BeatLinesView::BeatLineView : private TransformationNode
{
public:
    BeatLineView(float position, int barIndex, int beatIndex, QGraphicsItem* parent);
    ~BeatLineView();

private:
    QGraphicsSimpleTextItem* m_text;
    QGraphicsLineItem* m_line;
};

BeatLinesView::BeatLineView::BeatLineView(float position, int barIndex, int beatIndex, QGraphicsItem* parent)
: 	TransformationNode(parent),
    m_text(new QGraphicsSimpleTextItem(QString("%1").arg(barIndex), this)),
    m_line(new QGraphicsLineItem(0, 0, 0, 1, this))
{
    QFont smaller(m_text->font());
    smaller.setPointSize(8);
    m_text->setFont(smaller);
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    m_text->setVisible(beatIndex == 0); // Show only for first beat in bar
    m_text->setPos(0, 1);

    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    // Emphasize the first beat in bar
    if (beatIndex == 0) pen.setStyle(Qt::SolidLine);
    m_line->setPen(pen);

    setPos(position, 0);
}

BeatLinesView::BeatLineView::~BeatLineView()
{}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

BeatLinesView::BeatLinesView(RangeF timeRange, float timeScale, double beatOffset, double bpm, QGraphicsItem* parent)
  : TransformationNode(parent),
    m_timeRange(timeRange),
    m_timeScale(timeScale),
    m_beatOffset(beatOffset),
    m_bpm(bpm)
{
    // Set transformation to scale y-axis to [0, 1] range
    qreal maxi = 1;
    qreal mini = 0;
    QTransform transform;
    transform.scale(1, 1 / (maxi - mini));
    transform.translate(0, -mini);
    setTransform(transform);

    updateBeatLines();
}

BeatLinesView::~BeatLinesView()
{
}

void BeatLinesView::setTimeScale(float timeScale)
{
    m_timeScale = timeScale;
    updateBeatLines();
}

void BeatLinesView::setTimeRange(RangeF timeRange)
{
    m_timeRange = timeRange;
    updateBeatLines();
}

void BeatLinesView::setBeatOffset(double beatOffset)
{
    m_beatOffset = beatOffset;
    updateBeatLines();
}

void BeatLinesView::setBpm(double bpm)
{
    m_bpm = bpm;
    updateBeatLines();
}

void BeatLinesView::updateBeatLines()
{
    // Remove old lines
    m_beatLines.clear();

    if (m_bpm <= 0)
    {
        qWarning() << "Invalid bpm for beat line drawing:" << m_bpm;
        return;
    }

    // Calculate beat distance in the view with current time scale
    const double beatStepInSecs = 60.0 / m_bpm;
    const double beatIntervalInPoints = beatStepInSecs * m_timeScale;
    const double minPointsBetweenBeats = 30.0;

    // Calculate how many beats can we draw, draw every beatIndexStep'th beat.
    // Adjust so that first beats are always shown before intra-bar beats (beats 2,3,4).
    int beatIndexStep = qMax(minPointsBetweenBeats / beatIntervalInPoints, 1.0);

    // Assume 4/4 time signature
    const int beatsPerBar = 4;
    switch (beatIndexStep)
    {
    case 1:
    case 2:
        // 1 and 2 are ok
        break;

    case 3:
        // Map 3 to 2
        beatIndexStep = 2;
        break;

    default:
        //For >= 4 make it round down to dividable by 4
        beatIndexStep = (beatIndexStep / 4) * 4;
        break;
    }

    int barIndex = 0;
    int beatIndex = 0;

    QGraphicsItem* thisAsGraphicsItem = this;
    for (float beat = m_timeRange.min + m_beatOffset; beat < m_timeRange.max; beat += (beatStepInSecs * beatIndexStep))
    {
        m_beatLines.push_back(std::make_shared<BeatLineView>(beat, barIndex, beatIndex, thisAsGraphicsItem));

        // Advance beats
        beatIndex += beatIndexStep;
        if ((beatIndex % beatsPerBar) == 0)
        {
            // If moving to a new bar advance the bar index
            barIndex += beatIndex / beatsPerBar;
            beatIndex = 0;
        }
    }
}
