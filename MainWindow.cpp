#include "MainWindow.h"
#include "EditorView.h"
#include "EditorModel.h"
#include "CurveModel.h"
#include "SceneModel.h"
#include "ScenePropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QMenu>
#include <QMenuBar>

#include <QAction>
#include <QWidget>
#include <QSlider>
#include <QDockWidget>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    m_centralWidget(new QWidget)
{
    // Initialize central widget
    setCentralWidget(m_centralWidget);

    // Initialize scene action
    m_newSceneAction = new QAction(tr("New scene"), this);
    m_newSceneAction->setShortcut(QKeySequence::New);
    m_newSceneAction->setStatusTip(tr("Open a new empty scene"));
    connect(m_newSceneAction, SIGNAL(triggered()), this, SLOT(newScene()));

    m_openSceneAction = new QAction(tr("Open scene"), this);
    m_openSceneAction->setShortcut(QKeySequence::Open);
    m_openSceneAction->setStatusTip(tr("Open an existing scene"));
    connect(m_openSceneAction, SIGNAL(triggered()), this, SLOT(openScene()));

    m_saveSceneAction = new QAction(tr("Save scene"), this);
    m_saveSceneAction->setShortcut(QKeySequence::Save);
    m_saveSceneAction->setStatusTip(tr("Save current scene"));
    connect(m_saveSceneAction, SIGNAL(triggered()), this, SLOT(saveScene()));

    m_saveSceneAsAction = new QAction(tr("Save scene as"), this);
    m_saveSceneAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveSceneAsAction->setStatusTip(tr("Save current scene with a new name"));
    connect(m_saveSceneAsAction, SIGNAL(triggered()), this, SLOT(saveSceneAs()));

    m_closeSceneAction = new QAction(tr("Close scene"), this);
    m_closeSceneAction->setShortcut(QKeySequence::Close);
    m_closeSceneAction->setStatusTip(tr("Close current scene with a new name"));
    connect(m_closeSceneAction, SIGNAL(triggered()), this, SLOT(closeScene()));

    m_exportCurvesAction = new QAction(tr("Export curves"), this);
    m_exportCurvesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E)); // CTRL+E
    m_exportCurvesAction->setStatusTip(tr("Save scene curves to a new file"));
    connect(m_exportCurvesAction, SIGNAL(triggered()), this, SLOT(exportSceneCurves()));

    updateSceneActionStates();

    // Create file menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_newSceneAction);
    fileMenu->addAction(m_openSceneAction);
    fileMenu->addAction(m_saveSceneAction);
    fileMenu->addAction(m_saveSceneAsAction);
    fileMenu->addAction(m_closeSceneAction);

    // Create curves menu
    QMenu* curvesMenu = menuBar()->addMenu("&Curves");
    curvesMenu->addAction(m_exportCurvesAction);

    // Add dock widgets
    QDockWidget* pointDockWidget = new QDockWidget(tr("Point properties"), this);
    pointDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    pointDockWidget->setWidget(new PointPropertiesWidget);
    addDockWidget(Qt::LeftDockWidgetArea, pointDockWidget);

    QDockWidget* sceneDockWidget = new QDockWidget(tr("Scene sproperties"), this);
    sceneDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_sceneProperties = new ScenePropertiesWidget;
    sceneDockWidget->setWidget(m_sceneProperties);
    addDockWidget(Qt::LeftDockWidgetArea, sceneDockWidget);

    this->resize(1200, 500);
}

MainWindow::~MainWindow()
{
}

void MainWindow::newScene()
{
    qDebug() << "New scene";

    if (m_sceneModel)
    {
        qWarning() << "Cannot make new scene when old is still open";
        return;
    }

    // Create test scene with some curves
    qsrand(QTime::currentTime().msec());

    auto c1 = std::make_shared<CurveModel>(2, "First");
    c1->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});
    c1->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50), static_cast<float>((qrand() % 100) - 50)});

    auto c2 = std::make_shared<CurveModel>(1, "Second");
    c2->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);
    c2->addPoint(qrand() % 100, {static_cast<float>((qrand() % 100) - 50)}, 0, 0, 0);

    m_sceneModel = std::make_shared<SceneModel>(RangeF(0, 100));
    m_sceneModel->addCurve(c1);
    m_sceneModel->addCurve(c2);
    m_sceneModel->selectCurve(c2);
    m_sceneModel->setBpm(60);
    m_sceneProperties->setSceneModel(m_sceneModel);

    EditorView* allCurvesEditorView = new EditorView(m_sceneModel->getAllCurvesEditor());
    m_editors.push_back(allCurvesEditorView);
    EditorView* selectedCurvesEditorView = new EditorView(m_sceneModel->getSelectedCurvesEditor());
    m_editors.push_back(selectedCurvesEditorView);

    m_editorContainer = new QVBoxLayout;
    m_editorContainer->addWidget(allCurvesEditorView);
    m_editorContainer->addWidget(selectedCurvesEditorView);
    m_centralWidget->setLayout(m_editorContainer);

    updateSceneActionStates();
}

void MainWindow::openScene()
{
    qDebug() << "Open scene";

    if (m_sceneModel)
    {
        qWarning() << "Cannot make open scene when old is still open";
        return;
    }

    QString fileName = promptForSceneOpenFile();

    // Should have a name now
    if (fileName.isEmpty())
    {
        qWarning() << "No file name for scene, unable to open";
        return;
    }

    // Open file for reading
    QFile sceneFile(fileName);
    if (!sceneFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << sceneFile.errorString();
        return;
    }

    QXmlStreamReader stream(&sceneFile);
    m_sceneModel = SceneModel::create(stream);

    if (!m_sceneModel)
    {
        qWarning() << "Failed to open scene from file:" << fileName;
        return;
    }
    m_sceneModel->setFileName(fileName);
    m_sceneProperties->setSceneModel(m_sceneModel);

    EditorView* allCurvesEditorView = new EditorView(m_sceneModel->getAllCurvesEditor());
    m_editors.push_back(allCurvesEditorView);
    EditorView* selectedCurvesEditorView = new EditorView(m_sceneModel->getSelectedCurvesEditor());
    m_editors.push_back(selectedCurvesEditorView);

    m_editorContainer = new QVBoxLayout;
    m_editorContainer->addWidget(allCurvesEditorView);
    m_editorContainer->addWidget(selectedCurvesEditorView);
    m_centralWidget->setLayout(m_editorContainer);

    updateSceneActionStates();
}

void MainWindow::saveScene()
{
    qDebug() << "Save scene";

    if (!m_sceneModel)
    {
        qWarning() << "No scene, unable to save";
        return;
    }

    if (m_sceneModel->fileName().isEmpty())
    {
        // No file yet, prompt for it
        m_sceneModel->setFileName(promptForSceneSaveFile());
    }

    // Should have a name now
    if (m_sceneModel->fileName().isEmpty())
    {
        qWarning() << "No file name for scene, unable to save";
        return;
    }

    // Open the file
    QFile sceneFile(m_sceneModel->fileName());
    if (!sceneFile.open(QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate))
    {
        qWarning() << "Failed to open file for saving:" << sceneFile.errorString();
        return;
    }

    // Serialize the scene
    QXmlStreamWriter stream(&sceneFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument("1.0");

    m_sceneModel->serialize(stream);

    stream.writeEndDocument();
}

void MainWindow::saveSceneAs()
{
    qDebug() << "Save scene as";

    if (!m_sceneModel)
    {
        qWarning() << "No scene, unable to save";
        return;
    }

    // Prompt for new scene file name
    QString newSceneFileName = promptForSceneSaveFile();
    if (newSceneFileName.isEmpty())
    {
        qWarning() << "No new file name for scene, unable to save as";
        return;
    }

    // Apply new file name and save
    m_sceneModel->setFileName(newSceneFileName);
    saveScene();
}

namespace {
QString g_promptTitle("Select scene file name");
QString g_filters("XML Files (*.xml);;All Files (*.*)");
QString g_defaultFilter("XML Files (*.xml)");
} // anonymous namespace

QString MainWindow::promptForSceneOpenFile()
{
    return QFileDialog::getOpenFileName(this, g_promptTitle, QString(), g_filters, &g_defaultFilter);
}


QString MainWindow::promptForSceneSaveFile()
{
    return QFileDialog::getSaveFileName(this, g_promptTitle, QString(), g_filters, &g_defaultFilter);
}

void MainWindow::closeScene()
{
    qDebug() << "Close scene";

    if (!m_sceneModel)
    {
        qWarning() << "No scene, unable to close";
        return;
    }

    // Remove all editors from the layout containing them
    for (auto editor : m_editors)
    {
        m_editorContainer->removeWidget(editor);
        // After removal the ownership is here again, delete the editor view
        delete editor;
    }
    m_editors.clear();

    // Delete the layout
    delete m_editorContainer;
    m_editorContainer = nullptr;

    // Delete scene
    m_sceneModel.reset();
    m_sceneProperties->setSceneModel(m_sceneModel);

    updateSceneActionStates();
}

void MainWindow::exportSceneCurves()
{
    qDebug() << "Export scene curves";

    if (!m_sceneModel)
    {
        qWarning() << "No scene, unable to export curves";
        return;
    }

    // Prompt for new curve file name
    static QString s_promptTitle("Select curves file name");
    QString newCurveFileName = QFileDialog::getSaveFileName(this, s_promptTitle, QString(), g_filters, &g_defaultFilter);

    if (newCurveFileName.isEmpty())
    {
        qWarning() << "No new file name for curves, unable to export";
        return;
    }

    // Open the file
    QFile curvesFile(newCurveFileName);
    if (!curvesFile.open(QIODevice::WriteOnly  | QIODevice::Text | QIODevice::Truncate))
    {
        qWarning() << "Failed to open file for export:" << curvesFile.errorString();
        return;
    }

    // Serialize the scene curves
    QXmlStreamWriter stream(&curvesFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument("1.0");

    m_sceneModel->serializeCurves(stream);

    stream.writeEndDocument();
}

void MainWindow::updateSceneActionStates()
{
    if (m_sceneModel)
    {
        // We have an existing scene, allow saving and closing
        m_newSceneAction->setEnabled(false);
        m_openSceneAction->setEnabled(false);
        m_saveSceneAction->setEnabled(true);
        // Allow to save as if wa have previously saved
        m_saveSceneAsAction->setEnabled(m_sceneModel->fileName().isEmpty());
        m_closeSceneAction->setEnabled(true);

        // Got scene, allow exporting curves
        m_exportCurvesAction->setEnabled(true);
    }
    else
    {
        // We don't have an existing scene, allow new and open
        m_newSceneAction->setEnabled(true);
        m_openSceneAction->setEnabled(true);
        m_saveSceneAction->setEnabled(false);
        m_saveSceneAsAction->setEnabled(false);
        m_closeSceneAction->setEnabled(false);

        // No scene, cannot export curves
        m_exportCurvesAction->setEnabled(false);
    }
}

