#include "SceneModel.h"

#include "CurveModel.h"
#include "EditorModel.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileDialog>
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
    qDebug() << "Curve:" << elementName;

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

    auto curve = std::make_shared<CurveModel>(items, elementName);

    QString name = stream.attributes().value("name").toString();
    qDebug() << "Curve name:" << name;

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

        auto it = stream.attributes().cbegin();
        for (; it < stream.attributes().cend(); ++it)
        {
            qDebug() << "Attribute: " << it->name() << "=" << it->value();
        }

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

QList<std::shared_ptr<CurveModel>> loadCurves()
{
    QList<std::shared_ptr<CurveModel>> curves;

    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open curves XML", QString(), "All Files (*);;XML Files (*.xml)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open file:" << file.errorString();
            return curves;
        }

        QXmlStreamReader stream(&file);

        while (!stream.atEnd())
        {
            stream.readNext();
            if (stream.isStartElement() && stream.name().contains("catmull_rom", Qt::CaseSensitive))
            {
                std::shared_ptr<CurveModel> newCurve = createCurve(stream);
                if (newCurve)
                    curves.append(newCurve);
            }
        }
        if (stream.hasError())
        {
            qDebug() << "Error "<< stream.error() << "at (" << stream.lineNumber() << ":" << stream.columnNumber() << ") " << stream.errorString();
        }
    }

    return curves;
}


bool serializeCurve(std::shared_ptr<CurveModel> curve, QXmlStreamWriter& stream)
{
    int numberOfItems = curve->dimension();

    stream.writeStartElement(QString((numberOfItems > 1) ? "catmull_rom%1" : "catmull_rom").arg(numberOfItems));
    stream.writeAttribute("name", "MyCurve");

    //    stream.writeStartElement("curve_editor");
    //    {
    //        stream.writeStartElement("color");
    //        QXmlStreamAttributes colorAttr;
    //        colorAttr.append("r", QString::number(color().red()));
    //        colorAttr.append("g", QString::number(color().green()));
    //        colorAttr.append("b", QString::number(color().blue()));
    //        stream.writeAttributes(colorAttr);
    //        stream.writeEndElement();
    //    }
    //    {
    //        stream.writeStartElement("scale");
    //        QXmlStreamAttributes scaleAttr;
    //        scaleAttr.append("valueMin", QString::number(valueScaleMinM));
    //        scaleAttr.append("valueMax", QString::number(valueScaleMaxM));
    //        stream.writeAttributes(scaleAttr);
    //        stream.writeEndElement();
    //    }
    //    stream.writeEndElement();

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


bool serializeCurves(QList<std::shared_ptr<CurveModel>> curves, QXmlStreamWriter& stream)
{
    stream.setAutoFormatting(true);
    stream.writeStartDocument("1.0");
    stream.writeStartElement("curves");

    for (auto &curve : curves)
        if (!serializeCurve(curve, stream))
            return false;

    stream.writeEndElement();
    stream.writeEndDocument();
    return true;
}

void saveCurves(QList<std::shared_ptr<CurveModel>> curves)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save curves XML", QString(), "All Files (*);;XML Files (*.xml)");
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate))
        {
            qDebug() << "Failed to open file:" << file.errorString();
            return;
        }

        QXmlStreamWriter stream(&file);
        serializeCurves(curves, stream);
    }
}


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


SceneModel::SceneModel(RangeF timeRange)
  : m_timeRange(timeRange),
    m_beatOffset(0.0),
    m_bpm(80.0), // Default to 80bpm
    m_AllCurvesEditor(new EditorModel(m_timeRange, m_beatOffset, m_bpm)),
    m_SelectedCurvesEditor(new EditorModel(m_timeRange, m_beatOffset, m_bpm))
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

SceneModel::~SceneModel()
{
    // Editors are disconnect automatically
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

std::shared_ptr<EditorModel> SceneModel::getAllCurvesEditor()
{
    return m_AllCurvesEditor;
}

std::shared_ptr<EditorModel> SceneModel::getSelectedCurvesEditor()
{
    return m_SelectedCurvesEditor;
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

    // Listen to curve selection changes
    connect(curve.get(), &CurveModel::selectedChanged, this, &SceneModel::curveSelectionChanged);

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
    // No longer listen to curve selection changes
    disconnect(curve.get(), &CurveModel::selectedChanged, this, &SceneModel::curveSelectionChanged);

    // Remove all instance, just in case
    int removed = m_curves.removeAll(curve);
    if (removed < 1)
    {
        qWarning() << "Trying to remove non-existent curve" << curve->name();
        return;
    }

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

void SceneModel::saveCurves()
{
    ::saveCurves(m_curves);
}

void SceneModel::loadCurves()
{
    Container newCurves = ::loadCurves();
    for (auto &curve : newCurves)
        addCurve(curve);
}
