#include "SceneModel.h"

#include "CurveModel.h"
#include "StepCurveModel.h"
#include "EditorModel.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

/**
 * Helper to read one float attribute from xml stream.
 * It is ok to try to read non-existent attributes; the stream won't be messed up.
 *
 * @param stream Stream
 * @param attribute Attribute name
 * @param result [out] Resulting float, not touched if read fails.
 * @return True if read succeeded
 */
bool readFloat(QXmlStreamReader& stream, QString attribute, float& result)
{
    if (!stream.attributes().hasAttribute(attribute))
        return false;

    bool ok = false;
    const QString string = stream.attributes().value(attribute).toString();
    const float value = string.toFloat(&ok);
    if (!ok)
    {
        QString message = QString("Bad float value for element: ") + stream.name().toString() + ", attribute:" + attribute + " - " + string;
        stream.raiseError(message);
        return false;
    }

    result = value;
    return true;
}

std::shared_ptr<CurveModel> createCurve(QXmlStreamReader& stream)
{
    // Read start of curve element
    if (!stream.isStartElement())
    {
        stream.raiseError("Bad curve start");
        return nullptr;
    }
    if (!stream.name().contains("catmull_rom", Qt::CaseSensitive))
    {
        stream.raiseError("Unknown curve element");
        return nullptr;
    }

    // Save element name
    QString elementName = stream.name().toString();

    // Create a curve object
    std::shared_ptr<CurveModel> curve = std::make_shared<CurveModel>(stream.attributes().value("name").toString());

    // Value range (offset + multiplier)
    float valueOffset = 0;
    float valueMultiplier = 1;

    while (!stream.atEnd())
    {
        stream.readNext();
        if (stream.isEndElement())
        {
            if (stream.name().compare(elementName, Qt::CaseSensitive) == 0)
            {
                // Curve stream ended
                qDebug() << "Curve" << curve->name() << "serialized";
                const RangeF valueRange(valueOffset, valueOffset + valueMultiplier);
                curve->setValueRange(valueRange);
                return curve;
            }
        }

        // Debug print all attributes
        for (auto it = stream.attributes().cbegin(); it < stream.attributes().cend(); ++it)
            qDebug() << "Attribute: " << it->name() << "=" << it->value();

        if (stream.isStartElement() && stream.name() == "key")
        {
            float time;
            if (!readFloat(stream, "time", time))
                continue;

            float value;
            if (!readFloat(stream, "value", value))
                continue;

            const float scaledValue = value * valueMultiplier + valueOffset;
            const PointId pid = curve->addPoint(time, scaledValue);
            if (!pid.isValid())
                continue;

            // Tension, bias and continuity are optional, don't care if they don't exist
            float tension = 0.0f;
            float bias = 0.0f;
            float continuity = 0.0f;
            readFloat(stream, "tension", tension);
            readFloat(stream, "bias", bias);
            readFloat(stream, "continuity", continuity);

            curve->updatePointParams(pid, tension, bias, continuity);
        }
        else if (stream.isStartElement() && stream.name() == "value_offset")
        {
            if (!readFloat(stream, "v", valueOffset))
                continue;
        }
        else if (stream.isStartElement() && stream.name() == "value_multiplier")
        {
            if (!readFloat(stream, "v", valueMultiplier))
                continue;
        }
    }

    stream.raiseError("Unexpected stream end");
    return nullptr;
}

QList<std::shared_ptr<CurveModel>> loadCurves(QXmlStreamReader& stream)
{
    QList<std::shared_ptr<CurveModel>> curves;

    while (!stream.atEnd() || (stream.isEndElement() && stream.name().contains("curves")))
    {
        stream.readNext();
        if (stream.isStartElement() && stream.name().contains("catmull_rom", Qt::CaseSensitive))
        {
            std::shared_ptr<CurveModel> newCurve = createCurve(stream);
            if (newCurve)
                curves.append(newCurve);
        }
    }
    return curves;
}

bool serializeCurve(std::shared_ptr<CurveModel> curve, QXmlStreamWriter& stream)
{
    stream.writeStartElement("catmull_rom");
    stream.writeAttribute("name", curve->name());

    // Value range (offset + multiplier)
    const RangeF valueRange = curve->valueRange();
    const float offset = valueRange.min;
    const float multiplier = valueRange.max - valueRange.min;
    stream.writeEmptyElement("value_offset");
    stream.writeAttribute("v", QString("%1").arg(valueRange.min));
    stream.writeEmptyElement("value_multiplier");
    stream.writeAttribute("v", QString("%1").arg(multiplier));

    for (auto id : curve->pointIds())
    {
        const Point p = curve->point(id);

        stream.writeEmptyElement("key");

        const float time = p.time();
        const float value = (p.value().toFloat() - offset) / multiplier;

        stream.writeAttribute("time", QString("%1").arg(time));
        stream.writeAttribute("value", QString("%1").arg(value));

        const CurveModel::KbParams params = curve->params(id);
        if (params.tension() != 0.0f)
            stream.writeAttribute("tension", QString("%1").arg(params.tension()));

        if (params.bias() != 0.0f)
            stream.writeAttribute("bias", QString("%1").arg(params.bias()));

        if (params.continuity() != 0.0f)
            stream.writeAttribute("continuity", QString("%1").arg(params.continuity()));
    }

    stream.writeEndElement();
    return true;
}

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


SceneModel::SceneModel(RangeF timeRange)
  : m_timeRange(timeRange),
    m_beatOffset(0.0),
    m_bpm(80.0), // Default to 80bpm
    m_AllCurvesEditor(new EditorModel(m_timeRange, m_beatOffset, m_bpm)),
    m_SelectedCurvesEditor(new EditorModel(m_timeRange, m_beatOffset, m_bpm)),
    m_fileName() // No filename by default
{
    // Connect standard editors
    connect(this, &SceneModel::curveAdded, m_AllCurvesEditor.get(), &EditorModel::addCurve);
    connect(this, &SceneModel::curveRemoved, m_AllCurvesEditor.get(), &EditorModel::removeCurve);
    connect(this, &SceneModel::timeRangeChanged, m_AllCurvesEditor.get(), &EditorModel::setTimeRange);
    connect(this, &SceneModel::beatOffsetChanged, m_AllCurvesEditor.get(), &EditorModel::setBeatOffset);
    connect(this, &SceneModel::bpmChanged, m_AllCurvesEditor.get(), &EditorModel::setBpm);

    connect(this, &SceneModel::curveSelected, m_SelectedCurvesEditor.get(), &EditorModel::addCurve);
    connect(this, &SceneModel::curveDeselected, m_SelectedCurvesEditor.get(), &EditorModel::removeCurve);
    connect(this, &SceneModel::timeRangeChanged, m_SelectedCurvesEditor.get(), &EditorModel::setTimeRange);
    connect(this, &SceneModel::beatOffsetChanged, m_SelectedCurvesEditor.get(), &EditorModel::setBeatOffset);
    connect(this, &SceneModel::bpmChanged, m_SelectedCurvesEditor.get(), &EditorModel::setBpm);

    connect(m_AllCurvesEditor.get(), &EditorModel::requestToAddNewCurve, this, &SceneModel::addCurve);
    connect(m_SelectedCurvesEditor.get(), &EditorModel::requestToAddNewCurve, this, &SceneModel::addCurve);
}

std::shared_ptr<SceneModel> SceneModel::create(QXmlStreamReader& stream)
{
    std::shared_ptr<SceneModel> sceneModel = std::make_shared<SceneModel>(RangeF());

    while (!stream.atEnd())
    {
        stream.readNext();
        if (stream.isStartElement() && stream.name().contains("timerange", Qt::CaseSensitive))
        {
            bool isValid = stream.attributes().value("valid").toString().compare("true") == 0;
            if (isValid)
            {
                const QString strStart = stream.attributes().value("start").toString();
                const QString strEnd = stream.attributes().value("end").toString();

                const float start = strStart.toFloat(&isValid);
                const float end = strEnd.toFloat(&isValid);

                sceneModel->setTimeRange(isValid ? RangeF(start, end) : RangeF());
            }
        }
        else if (stream.isStartElement() && stream.name().contains("music", Qt::CaseSensitive))
        {
            bool beatOffsetOk = false;
            const double beatOffset = stream.attributes().value("firstBeatOffset").toString().toDouble(&beatOffsetOk);
            if (beatOffsetOk)
                sceneModel->setBeatOffset(beatOffset);

            bool bpmOk = false;
            const double bpm = stream.attributes().value("beatsPerMinute").toString().toDouble(&bpmOk);
            if (bpmOk)
                sceneModel->setBpm(bpm);
        }
        else if (stream.isStartElement() && stream.name().contains("curves", Qt::CaseSensitive))
        {
            QList<std::shared_ptr<CurveModel>> newCurves = ::loadCurves(stream);
            for (auto &curve : newCurves)
                sceneModel->addCurve(curve);
        }
    }

    if (stream.hasError())
    {
        qDebug() << "Error "<< stream.error() << "at (" << stream.lineNumber() << ":" << stream.columnNumber() << ") " << stream.errorString();
    }

    return sceneModel;
}

SceneModel::~SceneModel()
{
    // Editors are disconnected automatically
}

QList<std::shared_ptr<CurveModelAbs>> SceneModel::curves() const
{
    return m_curves;
}

const RangeF SceneModel::timeRange() const
{
    return m_timeRange;
}

double SceneModel::beatOffset() const
{
    return m_beatOffset;
}

double SceneModel::bpm() const
{
    return m_bpm;
}

std::shared_ptr<EditorModel> SceneModel::getAllCurvesEditor() const
{
    return m_AllCurvesEditor;
}

std::shared_ptr<EditorModel> SceneModel::getSelectedCurvesEditor() const
{
    return m_SelectedCurvesEditor;
}

const QString& SceneModel::fileName() const
{
    return m_fileName;
}

void SceneModel::addCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!addCurveInternal(curve))
        return;

    emit curveAdded(curve);

    if (curve->isSelected())
        emit curveSelected(curve);
}

bool SceneModel::addCurveInternal(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to add invalid curve";
        return false;
    }

    if (m_curves.contains(curve))
    {
        qWarning() << "Trying to add duplicate curve" << curve->name();
        return false;
    }

    // Synchronize curve time range to scene
    curve->setTimeRange(m_timeRange);
    connect(this, &SceneModel::timeRangeChanged, curve.get(), &CurveModelAbs::setTimeRange);

    // Listen to curve changes
    connect(curve.get(), &CurveModelAbs::selectedChanged, this, &SceneModel::curveSelectionChanged);
    connect(curve.get(), &CurveModelAbs::pointRemoved, this, &SceneModel::curvePointRemoved);

    m_curves.push_back(curve);
    return true;
}

void SceneModel::removeCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!removeCurveInternal(curve))
        return;

    if (curve->isSelected())
        emit curveDeselected(curve);

    emit curveRemoved(curve);
}

bool SceneModel::removeCurveInternal(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to remove invalid curve";
        return false;
    }

    if (!m_curves.contains(curve))
    {
        qWarning() << "Trying to remove non-existent curve";
        return false;
    }

    // Deselect curve on removal
    curve->setSelected(false);
    // No longer listen to curve changes
    disconnect(curve.get(), &CurveModelAbs::selectedChanged, this, &SceneModel::curveSelectionChanged);
    disconnect(curve.get(), &CurveModelAbs::pointRemoved, this, &SceneModel::curvePointRemoved);
    disconnect(this, &SceneModel::timeRangeChanged, curve.get(), &CurveModelAbs::setTimeRange);

    // Remove all instance, just in case
    m_curves.removeAll(curve);

    return true;
}

void SceneModel::setTimeRange(RangeF newTimeRange)
{
    if (m_timeRange != newTimeRange)
    {
        m_timeRange = newTimeRange;
        emit timeRangeChanged(m_timeRange);
    }
}

void SceneModel::setBeatOffset(double beatOffset)
{
    if (m_beatOffset != beatOffset)
    {
        m_beatOffset = beatOffset;
        emit beatOffsetChanged(m_beatOffset);
    }
}

void SceneModel::setBpm(double bpm)
{
    if (m_bpm != bpm)
    {
        m_bpm = bpm;
        emit bpmChanged(m_bpm);
    }
}

void SceneModel::selectCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve || !m_curves.contains(curve))
    {
        qWarning() << "Trying to select invalid curve";
        return;
    }
    if (curve->isSelected())
    {
        qWarning() << "Trying to select already selected curve";
        return;
    }

    curve->setSelected(true); // CurveModelAbs selection notification will result in emit curveSelected
}

void SceneModel::deselectCurve(std::shared_ptr<CurveModelAbs> curve)
{
    if (!curve || !m_curves.contains(curve))
    {
        qWarning() << "Trying to deselect invalid curve";
        return;
    }
    if (!curve->isSelected())
    {
        qWarning() << "Trying to deselect already deselected curve";
        return;
    }

    curve->setSelected(false); // CurveModelAbs deselection notification will result in emit curveDeselected
}


void SceneModel::curveSelectionChanged(bool status)
{
    const void* sendingCurve = sender();

    // Find the curve that sent the notification and emit (de)selection
    for (auto curve : m_curves)
        if (curve.get() == sendingCurve)
        {
            if (status)
                emit curveSelected(curve);
            else
                emit curveDeselected(curve);
            return;
        }

    qWarning() << "Manual selection change notification from unknown curve" << status;
}

void SceneModel::curvePointRemoved(PointId point)
{
    Q_UNUSED(point);

    const void* sendingCurve = sender();

    // Find the curve that sent the notification and emit (de)selection
    for (auto curve : m_curves)
        if (curve.get() == sendingCurve)
        {
            // Remove whole curve if last point was removed
            if (curve->numberOfPoints() == 0)
                removeCurve(curve);

            return;
        }

    qWarning() << "Point removed notification from unknown curve";
}

void SceneModel::serialize(QXmlStreamWriter& stream)
{
    // Serialize scene data
    stream.writeStartElement("scene");
    {
        stream.writeStartElement("timerange");
        stream.writeAttribute("valid", m_timeRange.isValid() ? "true" : "false");
        if (m_timeRange.isValid())
        {
            stream.writeAttribute("start", QString("%1").arg(m_timeRange.min));
            stream.writeAttribute("end", QString("%1").arg(m_timeRange.max));
        }
        stream.writeEndElement();
    }
    {
        stream.writeStartElement("music");
        stream.writeAttribute("firstBeatOffset", QString("%1").arg(m_beatOffset));
        stream.writeAttribute("beatsPerMinute", QString("%1").arg(m_bpm));
        stream.writeEndElement();
    }

    // Serialize curves
    serializeCurves(stream);

    // End scene
    stream.writeEndElement();
}

void SceneModel::serializeCurves(QXmlStreamWriter& stream)
{
    stream.writeStartElement("curves");

    for (auto &curve : m_curves)
    {
        std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);

        if (!splineCurve)
            continue; // Skip step curves for now

        if (!serializeCurve(splineCurve, stream))
        {
            qWarning() << "Scene curve serialization failed";
            return;
        }
    }

    stream.writeEndElement();
    stream.writeEndDocument();
}


void SceneModel::setFileName(const QString& fileName)
{
    m_fileName = fileName;
}
