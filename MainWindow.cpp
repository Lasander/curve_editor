#include "MainWindow.h"
#include "EditorView.h"
#include "EditorModel.h"
#include "CurveModel.h"
#include "SceneModel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QMenu>
#include <QMenuBar>

#include <QWidget>
#include <QSlider>
#include <QDockWidget>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>
#include <QGridLayout>

class PointPropertiesWidget : public QWidget
{
public:
    PointPropertiesWidget(QWidget * parent = 0)
    :	QWidget(parent)
    {
        QLabel* tensionLabel = new QLabel("Tension");
        QSlider* tensionSlider = new QSlider(Qt::Horizontal);
        tensionSlider->setMinimum(-100);
        tensionSlider->setMaximum(100);
        tensionSlider->setSingleStep(5);
        tensionSlider->setPageStep(20);

        QLabel* biasLabel = new QLabel("Bias");
        QSlider* biasSlider = new QSlider(Qt::Horizontal);
        biasSlider->setMinimum(-100);
        biasSlider->setMaximum(100);
        biasSlider->setSingleStep(5);
        biasSlider->setPageStep(20);

        QLabel* continuityLabel = new QLabel("Continuity");
        QSlider* continuitySlider = new QSlider(Qt::Horizontal);
        continuitySlider->setMinimum(-100);
        continuitySlider->setMaximum(100);
        continuitySlider->setSingleStep(5);
        continuitySlider->setPageStep(20);

        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->addWidget(tensionLabel, 0, 0);
        gridLayout->addWidget(tensionSlider, 0, 1);
        gridLayout->addWidget(biasLabel, 1, 0);
        gridLayout->addWidget(biasSlider, 1, 1);
        gridLayout->addWidget(continuityLabel, 2, 0);
        gridLayout->addWidget(continuitySlider, 2, 1);
        
        this->setLayout(gridLayout);
	}
    
    ~PointPropertiesWidget()
    {
    }
    
public slots:
    
private:

};

class ScenePropertiesWidget : public QWidget
{
public:
    ScenePropertiesWidget(std::shared_ptr<SceneModel> sceneModel, QWidget * parent = 0)
    :	QWidget(parent),
        m_gridLayout(new QGridLayout()),
        m_sceneModel(sceneModel)
    {
        connect(m_sceneModel.get(), &SceneModel::curveAdded, this, &ScenePropertiesWidget::addCurve);
        for (auto curve : m_sceneModel->curves())
            addCurve(curve);

        this->setLayout(m_gridLayout);
    }

    ~ScenePropertiesWidget()
    {
    }

public slots:
    void addCurve(std::shared_ptr<CurveModel> curve)
    {
        QLabel* nameLabel = new QLabel(curve->name());
        m_gridLayout->addWidget(nameLabel, m_gridLayout->rowCount(), 0);
    }

private:
    QGridLayout* m_gridLayout;
    std::shared_ptr<SceneModel> m_sceneModel;
};



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_sceneModel(std::make_shared<SceneModel>())
{
    qsrand(QTime::currentTime().msec());
    
    auto c1 = std::make_shared<CurveModel>(2, "First");
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    
    auto c2 = std::make_shared<CurveModel>(1, "Second");
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);

    m_sceneModel->addCurve(c1);
    m_sceneModel->addCurve(c2);
    m_sceneModel->setTimeRange(RangeF(0, 1000));

    m_sceneModel->selectCurve(c2);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new EditorView(m_sceneModel->getAllCurvesEditor()));
    layout->addWidget(new EditorView(m_sceneModel->getSelectedCurvesEditor()));
    
    QWidget* widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    
    this->resize(1200, 500);
    
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("Load curves", m_sceneModel.get(), SLOT(loadCurves()));
    fileMenu->addAction("Save curves", m_sceneModel.get(), SLOT(saveCurves()));
    
    QDockWidget* pointDockWidget = new QDockWidget(tr("Point properties"), this);
    pointDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    pointDockWidget->setWidget(new PointPropertiesWidget);
    addDockWidget(Qt::LeftDockWidgetArea, pointDockWidget);

    QDockWidget* sceneDockWidget = new QDockWidget(tr("Scene sproperties"), this);
    sceneDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ScenePropertiesWidget* sceneProperties = new ScenePropertiesWidget(m_sceneModel);

    sceneDockWidget->setWidget(sceneProperties);
    addDockWidget(Qt::LeftDockWidgetArea, sceneDockWidget);
}

MainWindow::~MainWindow()
{

}
