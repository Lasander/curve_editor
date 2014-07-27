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
class QLineEdit;
QT_END_NAMESPACE

/** Widget to adjust per point properties */
class PointPropertiesWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Construct PointPropertiesWidget
     * @param parent Parent widget
     */
    explicit PointPropertiesWidget(QWidget* parent = 0);
    ~PointPropertiesWidget();

    /**
     * @brief Set associated SceneModel. Unsets any previously set scene.
     * @param sceneModel The model
     */
    void setSceneModel(std::shared_ptr<SceneModel>& sceneModel);

private slots:
    void addCurve(std::shared_ptr<CurveModelAbs> curve);
    void removeCurve(std::shared_ptr<CurveModelAbs> curve);
    void clearCurves();

    void pointSelected(PointId id);
    void pointDeselected(PointId id);
    void selectedPointUpdated(PointId id);

    void parameterChanged(int value);
    void timeTextChanged(QString text);
    void valueTextChanged(QString text);

private:
    float tension() const;
    float bias() const;
    float continuity() const;

    QSlider* m_tensionSlider;
    QSlider* m_biasSlider;
    QSlider* m_continuitySlider;

    QLineEdit* m_timeEdit;
    QLineEdit* m_valueEdit;

    std::shared_ptr<SceneModel> m_sceneModel;
    QList<std::shared_ptr<CurveModel>> m_curves;
    QMap<PointId, std::shared_ptr<CurveModel>> m_selectedPoints;

    void setSingleSelectedPoint();
    void unsetSingleSelectedPoint();
    std::pair<PointId, std::shared_ptr<CurveModel>> m_singleSelectedPoint;
};

#endif // POINTPROPERTIESWIDGET_H
