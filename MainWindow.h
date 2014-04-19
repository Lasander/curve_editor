#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class SceneModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    std::shared_ptr<SceneModel> m_sceneModel;
};

#endif // MAINWINDOW_H
