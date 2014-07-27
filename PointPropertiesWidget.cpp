#include "PointPropertiesWidget.h"
#include "CurveModel.h"
#include "SceneModel.h"

#include <QSlider>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

PointPropertiesWidget::PointPropertiesWidget(QWidget* parent)
:	QWidget(parent),
    m_singleSelectedPoint(std::make_pair(PointId::invalidId(), nullptr))
{
    QLabel* timeLabel = new QLabel("Time");
    m_timeEdit = new QLineEdit;
    m_timeEdit->setEnabled(false);

    QLabel* valueLabel = new QLabel("Value");
    m_valueEdit = new QLineEdit;
    m_valueEdit->setEnabled(false);

    QLabel* tensionLabel = new QLabel("Tension");
    m_tensionSlider = new QSlider(Qt::Horizontal);
    m_tensionSlider->setMinimum(-100);
    m_tensionSlider->setMaximum(100);
    m_tensionSlider->setSingleStep(5);
    m_tensionSlider->setPageStep(20);
    m_tensionSlider->setEnabled(false);
    connect(m_tensionSlider, SIGNAL(valueChanged(int)), this, SLOT(parameterChanged(int)));

    QLabel* biasLabel = new QLabel("Bias");
    m_biasSlider = new QSlider(Qt::Horizontal);
    m_biasSlider->setMinimum(-100);
    m_biasSlider->setMaximum(100);
    m_biasSlider->setSingleStep(5);
    m_biasSlider->setPageStep(20);
    m_biasSlider->setEnabled(false);
    connect(m_biasSlider, SIGNAL(valueChanged(int)), this, SLOT(parameterChanged(int)));

    QLabel* continuityLabel = new QLabel("Continuity");
    m_continuitySlider = new QSlider(Qt::Horizontal);
    m_continuitySlider->setMinimum(-100);
    m_continuitySlider->setMaximum(100);
    m_continuitySlider->setSingleStep(5);
    m_continuitySlider->setPageStep(20);
    m_continuitySlider->setEnabled(false);
    connect(m_continuitySlider, SIGNAL(valueChanged(int)), this, SLOT(parameterChanged(int)));

    int row = 0;
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(timeLabel, row, 0);
    gridLayout->addWidget(m_timeEdit, row++, 1);
    gridLayout->addWidget(valueLabel, row, 0);
    gridLayout->addWidget(m_valueEdit, row++, 1);
    gridLayout->addWidget(tensionLabel, row, 0);
    gridLayout->addWidget(m_tensionSlider, row++, 1);
    gridLayout->addWidget(biasLabel, row, 0);
    gridLayout->addWidget(m_biasSlider, row++, 1);
    gridLayout->addWidget(continuityLabel, row, 0);
    gridLayout->addWidget(m_continuitySlider, row++, 1);

    this->setLayout(gridLayout);
}

PointPropertiesWidget::~PointPropertiesWidget()
{
}

void PointPropertiesWidget::setSceneModel(std::shared_ptr<SceneModel>& sceneModel)
{
    if (m_sceneModel == sceneModel)
    {
        return;
    }

    if (m_sceneModel)
    {
        // Disconnect from old scene
        disconnect(m_sceneModel.get(), &SceneModel::curveAdded, this, &PointPropertiesWidget::addCurve);
        disconnect(m_sceneModel.get(), &SceneModel::curveRemoved, this, &PointPropertiesWidget::removeCurve);

        // Clear curve table
        clearCurves();

        // Disable spinners
        m_tensionSlider->setEnabled(false);
        m_biasSlider->setEnabled(false);
        m_continuitySlider->setEnabled(false);
    }

    // Change the model
    m_sceneModel = sceneModel;

    if (m_sceneModel)
    {
        // Connect to new scene
        connect(m_sceneModel.get(), &SceneModel::curveAdded, this, &PointPropertiesWidget::addCurve);
        connect(m_sceneModel.get(), &SceneModel::curveRemoved, this, &PointPropertiesWidget::removeCurve);

        // Enable and initialize spinners
        m_tensionSlider->setEnabled(true);
        m_biasSlider->setEnabled(true);
        m_continuitySlider->setEnabled(true);

        // Add existing curves
        for (auto curve : m_sceneModel->curves())
            addCurve(curve);
    }

}

void PointPropertiesWidget::addCurve(std::shared_ptr<CurveModelAbs> curve)
{
    std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);
    if (!splineCurve)
        return;

    if (m_curves.contains(splineCurve))
        return;

    m_curves.push_back(splineCurve);

    connect(splineCurve.get(), SIGNAL(pointSelected(PointId)), this, SLOT(pointSelected(PointId)));
    connect(splineCurve.get(), SIGNAL(pointDeselected(PointId)), this, SLOT(pointDeselected(PointId)));
}

void PointPropertiesWidget::removeCurve(std::shared_ptr<CurveModelAbs> curve)
{
    std::shared_ptr<CurveModel> splineCurve = CurveModelAbs::getAsSplineCurve(curve);
    if (!splineCurve)
        return;

    if (!m_curves.contains(splineCurve))
        return;

    disconnect(splineCurve.get(), SIGNAL(pointSelected(PointId)), this, SLOT(pointSelected(PointId)));
    disconnect(splineCurve.get(), SIGNAL(pointDeselected(PointId)), this, SLOT(pointDeselected(PointId)));

    QList<PointId> toBeRemoved;
    for (auto point : m_selectedPoints.keys())
        if (m_selectedPoints[point] == curve)
            toBeRemoved.push_back(point);

    for (auto point : toBeRemoved)
        m_selectedPoints.remove(point);

    m_curves.removeAll(splineCurve);
}

void PointPropertiesWidget::clearCurves()
{
    m_curves.clear();
}

void PointPropertiesWidget::pointSelected(PointId id)
{
    for (const auto& curve : m_curves)
    {
        if (curve->pointIds().contains(id))
        {
            m_selectedPoints.insert(id, curve);

            if (m_selectedPoints.size() == 1)
                setSingleSelectedPoint();
            else
                unsetSingleSelectedPoint();

            selectedPointUpdated(m_singleSelectedPoint.first);
            break;
        }
    }
}

void PointPropertiesWidget::pointDeselected(PointId id)
{
    Q_ASSERT(id.isValid());

    if (m_singleSelectedPoint.first == id)
    {
        unsetSingleSelectedPoint();
        Q_ASSERT(m_selectedPoints.size() == 1);
    }

    m_selectedPoints.remove(id);

    if (m_selectedPoints.size() == 1)
    {
        setSingleSelectedPoint();
    }

    selectedPointUpdated(m_singleSelectedPoint.first);
}

void PointPropertiesWidget::selectedPointUpdated(PointId id)
{
    if (id.isValid())
    {
        Q_ASSERT(id == m_singleSelectedPoint.first);

        const Point p = m_singleSelectedPoint.second->point(id);

        const QString timeText = QString("%1").arg(p.time());
        if (m_timeEdit->text() != timeText)
            m_timeEdit->setText(timeText);

        const QString valueText = p.value().toString();
        if (m_valueEdit->text() != valueText)
            m_valueEdit->setText(valueText);
    }
    else
    {
        Q_ASSERT(!m_singleSelectedPoint.first.isValid());

        m_timeEdit->setText("");
        m_valueEdit->setText("");
    }
}

void PointPropertiesWidget::parameterChanged(int value)
{
    Q_UNUSED(value);

    const float t = tension();
    const float b = bias();
    const float c = continuity();

    for (auto point : m_selectedPoints.keys())
    {
        std::shared_ptr<CurveModel> curve = m_selectedPoints[point];
        curve->updatePointParams(point, t, b, c);
    }
}

void PointPropertiesWidget::timeTextChanged(QString text)
{
    Q_ASSERT(m_singleSelectedPoint.first.isValid());

    bool ok = false;
    const float time = text.toFloat(&ok);

    // Allow empty text or anything that converts to float
    if (!ok && !text.isEmpty())
    {
        // Reset the bad text with selected point update notification
        selectedPointUpdated(m_singleSelectedPoint.first);
        return;
    }

    const Point p = m_singleSelectedPoint.second->point(m_singleSelectedPoint.first);
    if (ok && (p.time() != time))
        m_singleSelectedPoint.second->updatePoint(m_singleSelectedPoint.first, time, p.value());
}

void PointPropertiesWidget::valueTextChanged(QString text)
{
    Q_ASSERT(m_singleSelectedPoint.first.isValid());

    bool ok = false;
    const QVariant value = text.toFloat(&ok);

    // Allow empty text or anything that converts to float
    if (!ok && !text.isEmpty())
    {
        // Reset the bad text with selected point update notification
        selectedPointUpdated(m_singleSelectedPoint.first);
        return;
    }

    const Point p = m_singleSelectedPoint.second->point(m_singleSelectedPoint.first);
    if (ok && (p.value() != value))
        m_singleSelectedPoint.second->updatePoint(m_singleSelectedPoint.first, p.time(), value);
}

float PointPropertiesWidget::tension() const
{
    return m_tensionSlider->value() / 100.0f;
}

float PointPropertiesWidget::bias() const
{
    return m_biasSlider->value() / 100.0f;
}

float PointPropertiesWidget::continuity() const
{
    return m_continuitySlider->value() / 100.0f;
}

void PointPropertiesWidget::setSingleSelectedPoint()
{
    Q_ASSERT(m_selectedPoints.size() == 1);
    Q_ASSERT(m_singleSelectedPoint.first == PointId::invalidId());

    connect(m_selectedPoints.first().get(), SIGNAL(pointUpdated(PointId)), this, SLOT(selectedPointUpdated(PointId)));
    connect(m_timeEdit, SIGNAL(textChanged(QString)), this, SLOT(timeTextChanged(QString)));
    connect(m_valueEdit, SIGNAL(textChanged(QString)), this, SLOT(valueTextChanged(QString)));
    m_singleSelectedPoint = std::make_pair(m_selectedPoints.firstKey(), m_selectedPoints.first());

    m_timeEdit->setEnabled(true);
    m_valueEdit->setEnabled(true);
}

void PointPropertiesWidget::unsetSingleSelectedPoint()
{
    if (m_singleSelectedPoint.first.isValid())
    {
        disconnect(m_singleSelectedPoint.second.get(), SIGNAL(pointUpdated(PointId)), this, SLOT(selectedPointUpdated(PointId)));
        disconnect(m_timeEdit, SIGNAL(textChanged(QString)), this, SLOT(timeTextChanged(QString)));
        disconnect(m_valueEdit, SIGNAL(textChanged(QString)), this, SLOT(valueTextChanged(QString)));
        m_singleSelectedPoint = std::make_pair(PointId::invalidId(), nullptr);
    }
    m_timeEdit->setEnabled(false);
    m_valueEdit->setEnabled(false);
}
