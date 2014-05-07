#include "PointPropertiesWidget.h"
#include "CurveModel.h"
#include "SceneModel.h"

#include <QSlider>
#include <QLabel>
#include <QGridLayout>

PointPropertiesWidget::PointPropertiesWidget(QWidget* parent)
:	QWidget(parent)
{
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

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(tensionLabel, 0, 0);
    gridLayout->addWidget(m_tensionSlider, 0, 1);
    gridLayout->addWidget(biasLabel, 1, 0);
    gridLayout->addWidget(m_biasSlider, 1, 1);
    gridLayout->addWidget(continuityLabel, 2, 0);
    gridLayout->addWidget(m_continuitySlider, 2, 1);

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

void PointPropertiesWidget::addCurve(std::shared_ptr<CurveModel> curve)
{
    if (m_curves.contains(curve))
        return;

    m_curves.push_back(curve);

    connect(curve.get(), SIGNAL(pointSelected(PointId)), this, SLOT(pointSelected(PointId)));
    connect(curve.get(), SIGNAL(pointDeselected(PointId)), this, SLOT(pointDeselected(PointId)));
}

void PointPropertiesWidget::removeCurve(std::shared_ptr<CurveModel> curve)
{
    if (!m_curves.contains(curve))
        return;

    disconnect(curve.get(), SIGNAL(pointSelected(PointId)), this, SLOT(pointSelected(PointId)));
    disconnect(curve.get(), SIGNAL(pointDeselected(PointId)), this, SLOT(pointDeselected(PointId)));

    QList<PointId> toBeRemoved;
    for (auto point : m_selectedPoints.keys())
        if (m_selectedPoints[point] == curve)
            toBeRemoved.push_back(point);

    for (auto point : toBeRemoved)
        m_selectedPoints.remove(point);

    m_curves.removeAll(curve);
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
        }
    }
}

void PointPropertiesWidget::pointDeselected(PointId id)
{
    m_selectedPoints.remove(id);
}

void PointPropertiesWidget::parameterChanged(int value)
{
    Q_UNUSED(value);

    float t = tension();
    float b = bias();
    float c = continuity();

    for (auto point : m_selectedPoints.keys())
    {
        std::shared_ptr<CurveModel> curve = m_selectedPoints[point];
        curve->updatePointParams(point, t, b, c);
    }
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

