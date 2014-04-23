#include "BeatLinesView.h"

#include <QFont>
#include <QPen>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>

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
    m_text(new QGraphicsSimpleTextItem(QString("%1.%2").arg(barIndex).arg(beatIndex), this)),
    m_line(new QGraphicsLineItem(0, 0, 0, 1, this))
{
    QFont smaller(m_text->font());
    smaller.setPointSize(8);
    m_text->setFont(smaller);
    m_text->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    m_text->setVisible(false); // TODO: Don't show texts for now
    m_text->setPos(0, 1);

    QPen pen(Qt::DotLine);
    pen.setCosmetic(true);
    // Emphasize the first beat in bar
    if (beatIndex == 1) pen.setStyle(Qt::SolidLine);
    m_line->setPen(pen);

    setPos(position, 0);
}

BeatLinesView::BeatLineView::~BeatLineView()
{}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

BeatLinesView::BeatLinesView(RangeF timeRange, float timeScale, float beatOffset, float bpm, QGraphicsItem* parent)
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

void BeatLinesView::setBeatOffset(float beatOffset)
{
    m_beatOffset = beatOffset;
    updateBeatLines();
}

void BeatLinesView::setBpm(float bpm)
{
    m_bpm = bpm;
    updateBeatLines();
}

void BeatLinesView::updateBeatLines()
{
    // Remove old lines
    m_beatLines.clear();

    // Assumer 4/4 time signature
    const int beatsPerBar = 4;
    int barIndex = 1;
    int beatIndex = 1;

    float beatStep = 60.0f / m_bpm;
    QGraphicsItem* thisAsGraphicsItem = this;

    for (float beat = m_timeRange.min + m_beatOffset;
         beat < m_timeRange.max; beat += beatStep)
    {
        m_beatLines.push_back(std::make_shared<BeatLineView>(beat, barIndex, beatIndex, thisAsGraphicsItem));

        if (++beatIndex > beatsPerBar)
        {
            // Current bar ended
            beatIndex = 1;
            ++barIndex;
        }
    }
}
