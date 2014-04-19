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

    std::shared_ptr<CurveModel> curve(new CurveModel(items));

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


SceneModel::SceneModel()
{
}

SceneModel::~SceneModel()
{
}

QList<std::shared_ptr<CurveModel>> SceneModel::curves() const
{
    return m_curves;
}

const RangeF SceneModel::timeRange() const
{
    return m_timeRange;
}

void SceneModel::addCurve(std::shared_ptr<CurveModel> model)
{
    if (m_curves.contains(model))
        return;

    // Synchronize curve time range to scene
    model->setTimeRange(m_timeRange);
    connect(this,&SceneModel::timeRangeChanged, model.get(), &CurveModel::setTimeRange);

    m_curves.push_back(model);
    emit curveAdded(model);
}

void SceneModel::removeCurve(std::shared_ptr<CurveModel> model)
{
    int removed = m_curves.removeAll(model);
    if (removed < 1)
        return;

    disconnect(this,&SceneModel::timeRangeChanged, model.get(), &CurveModel::setTimeRange);

    emit curveRemoved(model);
}

void SceneModel::setTimeRange(RangeF newTimeRange)
{
    if (m_timeRange != newTimeRange)
    {
        m_timeRange = newTimeRange;
        emit timeRangeChanged(m_timeRange);
    }
}

void SceneModel::setAllCurvesEditor(std::shared_ptr<EditorModel> editor)
{
    if (m_AllCurvesEditor != editor)
    {
        if (m_AllCurvesEditor)
        {
            // Disconnect previous editor
            disconnect(this, &SceneModel::curveAdded, m_AllCurvesEditor.get(), &EditorModel::addCurve);
            disconnect(this, &SceneModel::curveRemoved, m_AllCurvesEditor.get(), &EditorModel::removeCurve);
            disconnect(this, &SceneModel::timeRangeChanged, m_AllCurvesEditor.get(), &EditorModel::setTimeRange);
        }

        m_AllCurvesEditor = editor;

        if (m_AllCurvesEditor)
        {
            m_AllCurvesEditor->setTimeRange(m_timeRange);
            m_AllCurvesEditor->clearCurves();

            for (auto curve : m_curves)
                m_AllCurvesEditor->addCurve(curve);

            // Connect new editor
            connect(this, &SceneModel::curveAdded, m_AllCurvesEditor.get(), &EditorModel::addCurve);
            connect(this, &SceneModel::curveRemoved, m_AllCurvesEditor.get(), &EditorModel::removeCurve);
            connect(this, &SceneModel::timeRangeChanged, m_AllCurvesEditor.get(), &EditorModel::setTimeRange);
        }
    }
}

void SceneModel::setSelectedCurvesEditor(std::shared_ptr<EditorModel> editor)
{
    if (m_SelectedCurvesEditor != editor)
    {
        if (m_SelectedCurvesEditor)
        {
            // Disconnect previous editor
            disconnect(this, &SceneModel::curveSelected, m_SelectedCurvesEditor.get(), &EditorModel::addCurve);
            disconnect(this, &SceneModel::curveDeselected, m_SelectedCurvesEditor.get(), &EditorModel::removeCurve);
            disconnect(this, &SceneModel::timeRangeChanged, m_SelectedCurvesEditor.get(), &EditorModel::setTimeRange);
        }

        m_SelectedCurvesEditor = editor;

        if (m_SelectedCurvesEditor)
        {
            m_SelectedCurvesEditor->setTimeRange(m_timeRange);
            m_SelectedCurvesEditor->clearCurves();

            for (auto curve : m_curves)
                if (curve->isSelected()) m_SelectedCurvesEditor->addCurve(curve);

            // Connect new editor
            connect(this, &SceneModel::curveSelected, m_SelectedCurvesEditor.get(), &EditorModel::addCurve);
            connect(this, &SceneModel::curveDeselected, m_SelectedCurvesEditor.get(), &EditorModel::removeCurve);
            connect(this, &SceneModel::timeRangeChanged, m_SelectedCurvesEditor.get(), &EditorModel::setTimeRange);
        }
    }
}

void SceneModel::selectCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve || !m_curves.contains(curve))
    {
        qDebug() << "Trying to select invalid curve";
        return;
    }
    if (curve->isSelected())
    {
        qDebug() << "Trying to select already selected curve";
        return;
    }

    curve->setSelected(true);

    emit curveSelected(curve);
}

void SceneModel::deselectCurve(std::shared_ptr<CurveModel> curve)
{
    if (!curve || !m_curves.contains(curve))
    {
        qDebug() << "Trying to select invalid curve";
        return;
    }
    if (!curve->isSelected())
    {
        qDebug() << "Trying to deselect already deselected curve";
        return;
    }

    curve->setSelected(false);

    emit curveDeselected(curve);
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
