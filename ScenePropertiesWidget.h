#ifndef SCENEPROPERTIESWIDGET_H
#define SCENEPROPERTIESWIDGET_H

#include <QWidget>

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
    void addCurve(std::shared_ptr<CurveModel> curve);
    void removeCurve(std::shared_ptr<CurveModel> curve);

private:
    QGridLayout* m_gridLayout;
    std::shared_ptr<SceneModel> m_sceneModel;

    QDoubleSpinBox* m_beatOffsetSpinner;
    QDoubleSpinBox* m_bpmSpinner;

    CurveTableModel* m_curveTableModel;
    QTableView* m_curveTable;
};


#endif // SCENEPROPERTIESWIDGET_H
