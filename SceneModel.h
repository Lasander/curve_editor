//
//  SceneModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 19.04.14.
//
//

#ifndef __CurveEditor__SceneModel__
#define __CurveEditor__SceneModel__

#include "RangeF.h"
#include <QObject>
#include <QList>

class CurveModel;
class EditorModel;

class SceneModel : public QObject
{
    Q_OBJECT

public:
    SceneModel();
    ~SceneModel();

    QList<std::shared_ptr<CurveModel>> curves() const;
    const RangeF timeRange() const;

signals:
    void curveAdded(std::shared_ptr<CurveModel> curve);
    void curveRemoved(std::shared_ptr<CurveModel> curve);

    void curveSelected(std::shared_ptr<CurveModel> curve);
    void curveDeselected(std::shared_ptr<CurveModel> curve);

    void timeRangeChanged(RangeF timeRange);

public slots:
    void addCurve(std::shared_ptr<CurveModel> curve);
    void removeCurve(std::shared_ptr<CurveModel> curve);
    void setTimeRange(RangeF timeRange);

    void setAllCurvesEditor(std::shared_ptr<EditorModel> editor);
    void setSelectedCurvesEditor(std::shared_ptr<EditorModel> editor);

    void selectCurve(std::shared_ptr<CurveModel> curve);
    void deselectCurve(std::shared_ptr<CurveModel> curve);

    void saveCurves();
    void loadCurves();

private:
    using Container = QList<std::shared_ptr<CurveModel>>;
    using Iterator = Container::Iterator;
    using ConstIterator = Container::ConstIterator;

    Container m_curves;

    RangeF m_timeRange; /**< Scene time range */

    std::shared_ptr<EditorModel> m_AllCurvesEditor;
    std::shared_ptr<EditorModel> m_SelectedCurvesEditor;

};

#endif // __CurveEditor__SceneModel__
