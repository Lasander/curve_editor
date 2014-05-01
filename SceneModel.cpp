#include "SceneModel.h"

#include "CurveModel.h"
#include "EditorModel.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>

///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////

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

    // Parse item count
    QString temp = elementName;
    temp = temp.remove(0, strlen("catmull_rom"));
    bool ok = false;
    int items = temp.toInt(&ok);
    if (!ok) items = 1;
    if (items < 1 || items > 4)
    {
        stream.raiseError(QString("Unsupported curve item count: %1").arg(items));
        return nullptr;
    }

    auto curve = std::make_shared<CurveModel>(items, stream.attributes().value("name").toString());

    while (!stream.atEnd())
    {
        stream.readNext();
        if (stream.isEndElement())
        {
            if (stream.name().compare(elementName, Qt::CaseSensitive) == 0)
            {
                return curve;
            }
        }

        for (auto it = stream.attributes().cbegin(); it < stream.attributes().cend(); ++it)
            qDebug() << "Attribute: " << it->name() << "=" << it->value();

        if (stream.isStartElement() && stream.name() == "key")
        {
            bool ok = false;
            float time = stream.attributes().value("time").toString().toFloat(&ok);
            if (!ok)
            {
                stream.raiseError(QString("Bad time attribute for key point"));
                continue;
            }

            QString valueStr = stream.attributes().value("value").toString();
            QStringList list = valueStr.split(QRegExp("\\s+"));
            if (list.size() != items)
            {
                stream.raiseError(QString("Curve/key item count mismatch %1 vs. %2").arg(items).arg(list.size()));
                continue;
            }

            QList<float> values;
            for (auto& valueString : list)
                values.append(valueString.toFloat());

            curve->addPoint(time, values);
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
    int numberOfItems = curve->dimension();

    stream.writeStartElement((numberOfItems > 1) ? QString("catmull_rom%1").arg(numberOfItems) : "catmull_rom");
    stream.writeAttribute("name", curve->name());

    for (auto id : curve->pointIds())
    {
        const CurveModel::Point p = curve->point(id);

        stream.writeEmptyElement("key");
        stream.writeAttribute("time", QString("%1").arg(p.time()));

        QString value = QString("%1").arg(p.value(0));
        for (int i = 1; i < numberOfItems; ++i)
            value += QString(" %1").arg(p.value(i));

        stream.writeAttribute("value", value);
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
            Container newCurves = ::loadCurves(stream);
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

QList<std::shared_ptr<CurveModel>> SceneModel::curves() const
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

void SceneModel::addCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to add invalid curve";
        return;
    }

    if (m_curves.contains(curve))
    {
        qWarning() << "Trying to add duplicate curve" << curve->name();
        return;
    }

    // Synchronize curve time range to scene
    curve->setTimeRange(m_timeRange);
    connect(this, &SceneModel::timeRangeChanged, curve.get(), &CurveModel::setTimeRange);

    // Listen to curve changes
    connect(curve.get(), &CurveModel::selectedChanged, this, &SceneModel::curveSelectionChanged);
    connect(curve.get(), &CurveModel::pointRemoved, this, &SceneModel::curvePointRemoved);

    m_curves.push_back(curve);
    emit curveAdded(curve);

    if (curve->isSelected())
    {
        // Notify curve selection in case an already select curve is added
        emit curveSelected(curve);
    }
}

void SceneModel::removeCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve)
    {
        qWarning() << "Trying to remove invalid curve";
        return;
    }

    if (!m_curves.contains(curve))
    {
        qWarning() << "Trying to remove non-existent curve";
        return;
    }

    // Deselect curve on removal
    curve->setSelected(false);
    // No longer listen to curve changes
    disconnect(curve.get(), &CurveModel::selectedChanged, this, &SceneModel::curveSelectionChanged);
    disconnect(curve.get(), &CurveModel::pointRemoved, this, &SceneModel::curvePointRemoved);

    // Remove all instance, just in case
    m_curves.removeAll(curve);

    emit curveRemoved(curve);

    disconnect(this, &SceneModel::timeRangeChanged, curve.get(), &CurveModel::setTimeRange);
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

void SceneModel::selectCurve(std::shared_ptr<CurveModel> curve)
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

    curve->setSelected(true); // CurveModel selection notification will result in emit curveSelected
}

void SceneModel::deselectCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve || !m_curves.contains(curve))
    {
        qWarning() << "Trying to select invalid curve";
        return;
    }
    if (!curve->isSelected())
    {
        qWarning() << "Trying to deselect already deselected curve";
        return;
    }

    curve->setSelected(false); // CurveModel deselection notification will result in emit curveDeselected
}

void SceneModel::curveSelectionChanged(bool status)
{
    const CurveModel* sendingCurve = static_cast<CurveModel*>(sender());

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

    const CurveModel* sendingCurve = static_cast<CurveModel*>(sender());

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
        if (!serializeCurve(curve, stream))
        {
            qWarning() << "Scene curve serialization failed";
            return;
        }


    stream.writeEndElement();
    stream.writeEndDocument();
}


void SceneModel::setFileName(const QString& fileName)
{
    m_fileName = fileName;
}
