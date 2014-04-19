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
    ScenePropertiesWidget(QWidget * parent = 0)
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

    ~ScenePropertiesWidget()
    {
    }

public slots:

private:

};



MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_sceneModel(new SceneModel)
{
    qsrand(QTime::currentTime().msec());
    
    std::shared_ptr<CurveModel> c1(new CurveModel(2));
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    
    std::shared_ptr<CurveModel> c2(new CurveModel(1));
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 1);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 1);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 1);
    c2->addPoint(qrand() % 500, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 1);

    std::shared_ptr<EditorModel> allCurvesEditor(new EditorModel);
    std::shared_ptr<EditorModel> selectedCurvesEditor(new EditorModel);
    m_sceneModel->setAllCurvesEditor(allCurvesEditor);
    m_sceneModel->setSelectedCurvesEditor(selectedCurvesEditor);

    m_sceneModel->addCurve(c1);
    m_sceneModel->addCurve(c2);
    m_sceneModel->setTimeRange(RangeF(0, 1000));

    m_sceneModel->selectCurve(c2);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new EditorView(allCurvesEditor));
    layout->addWidget(new EditorView(selectedCurvesEditor));
    
    QWidget* widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    
    this->resize(500, 500);
    
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("Load curves", m_sceneModel.get(), SLOT(loadCurves()));
    fileMenu->addAction("Save curves", m_sceneModel.get(), SLOT(saveCurves()));
    
    QDockWidget* pointDockWidget = new QDockWidget(tr("Point properties"), this);
    pointDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    pointDockWidget->setWidget(new PointPropertiesWidget);
    addDockWidget(Qt::LeftDockWidgetArea, pointDockWidget);

    QDockWidget* sceneDockWidget = new QDockWidget(tr("Scene sproperties"), this);
    sceneDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    sceneDockWidget->setWidget(new ScenePropertiesWidget);
    addDockWidget(Qt::LeftDockWidgetArea, sceneDockWidget);
}

MainWindow::~MainWindow()
{

}
