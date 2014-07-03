#ifndef POINTPROPERTIESWIDGET_H
#define POINTPROPERTIESWIDGET_H

#include "PointId.h"
#include <QWidget>
#include <QMap>
#include <memory>

class CurveModelAbs;
class CurveModel;
class SceneModel;

QT_BEGIN_NAMESPACE
class QSlider;
QT_END_NAMESPACE

class PointPropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PointPropertiesWidget(QWidget* parent = 0);
    ~PointPropertiesWidget();

    void setSceneModel(std::shared_ptr<SceneModel>& sceneModel);

private slots:
    void addCurve(std::shared_ptr<CurveModelAbs> curve);
    void removeCurve(std::shared_ptr<CurveModelAbs> curve);
    void clearCurves();

    void pointSelected(PointId id);
    void pointDeselected(PointId id);

    void parameterChanged(int value);

private:
    float tension() const;
    float bias() const;
    float continuity() const;

    QSlider* m_tensionSlider;
    QSlider* m_biasSlider;
    QSlider* m_continuitySlider;

    std::shared_ptr<SceneModel> m_sceneModel;
    QList<std::shared_ptr<CurveModel>> m_curves;
    QMap<PointId, std::shared_ptr<CurveModel>> m_selectedPoints;
};

#endif // POINTPROPERTIESWIDGET_H
