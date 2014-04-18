//
//  EditorModel.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 31.12.13.
//
//

#ifndef __CurveEditor__EditorModel__
#define __CurveEditor__EditorModel__

#include "RangeF.h"
#include <QObject>
#include <QList>

class CurveModel;

class EditorModel : public QObject
{
    Q_OBJECT
    
public:
    EditorModel();
    ~EditorModel();
    
    QList<std::shared_ptr<CurveModel>> curves() const;
    const RangeF timeRange() const;
    
signals:
    void curveAdded(std::shared_ptr<CurveModel> curve);
    void curveRemoved(std::shared_ptr<CurveModel> curve);
    void timeRangeChanged(RangeF timeRange);
    
public slots:
    void addCurve(std::shared_ptr<CurveModel> model);
    void removeCurve(std::shared_ptr<CurveModel> model);
    void curveTimeRangeChanged(RangeF timeRange);
    void setSceneTimeRange(RangeF timeRange);

    void saveCurves();
    void loadCurves();
    
private:
    using Container = QList<std::shared_ptr<CurveModel>>;
    using Iterator = Container::Iterator;
    using ConstIterator = Container::ConstIterator;
    
	Container m_curves;

    void updateTimeRange();
    RangeF calculateTimeRange() const;

    /** Overall cache time range */
    RangeF m_timeRange;

    /** Scene time range */
    RangeF m_sceneTimeRange;
};

#endif /* defined(__CurveEditor__EditorModel__) */
