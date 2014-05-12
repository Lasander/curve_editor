#ifndef SCENEPROPERTIESWIDGET_H
#define SCENEPROPERTIESWIDGET_H

#include <QWidget>
#include <memory>

class CurveModel;
class SceneModel;
class CurveTableModel;

QT_BEGIN_NAMESPACE
class QDoubleSpinBox;
class QGridLayout;
class QTableView;
QT_END_NAMESPACE

class ScenePropertiesWidget : public QWidget
{
    Q_OBJECT

public:
    ScenePropertiesWidget(QWidget * parent = 0);
    ~ScenePropertiesWidget();

public slots:
    void setSceneModel(std::shared_ptr<SceneModel> sceneModel);

private slots:
    void setTimeLineLength(double length);

private:
    QGridLayout* m_gridLayout;
    std::shared_ptr<SceneModel> m_sceneModel;

    QDoubleSpinBox* m_beatOffsetSpinner;
    QDoubleSpinBox* m_bpmSpinner;

    QDoubleSpinBox* m_timeLineLength;

    CurveTableModel* m_curveTableModel;
    QTableView* m_curveTable;
};


#endif // SCENEPROPERTIESWIDGET_H
