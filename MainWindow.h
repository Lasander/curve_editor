#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class SceneModel;
class EditorView;
class PointPropertiesWidget;
class ScenePropertiesWidget;

QT_BEGIN_NAMESPACE
class QAction;
class QLayout;
class QWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    void newScene();
    void openScene();
    void saveScene();
    void saveSceneAs();
    void closeScene();

    void exportSceneCurves();

private:
    /**
     * @brief Prompt for existing scene file name.
     * @return File name
     */
    QString promptForSceneOpenFile();
    /**
     * @brief Prompt for new scene file name.
     * @return File name
     */
    QString promptForSceneSaveFile();

    /** Update enabled state for scene actions */
    void updateSceneActionStates();

    QWidget* m_centralWidget;

    QLayout* m_editorContainer;
    QList<EditorView*> m_editors;

    PointPropertiesWidget* m_pointProperties;
    ScenePropertiesWidget* m_sceneProperties;

    std::shared_ptr<SceneModel> m_sceneModel;

    QAction* m_newSceneAction;
    QAction* m_openSceneAction;
    QAction* m_saveSceneAction;
    QAction* m_saveSceneAsAction;
    QAction* m_closeSceneAction;

    QAction* m_exportCurvesAction;
};

#endif // MAINWINDOW_H
