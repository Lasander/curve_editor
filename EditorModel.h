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

/**
 * @brief Editor model groups curves to be displayed in a CurveView.
 */
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
    void clearCurves();
    void setTimeRange(RangeF timeRange);
    
private:
    using Container = QList<std::shared_ptr<CurveModel>>;
    
    Container m_curves; /**< Curves associated with this editor */
    RangeF m_timeRange; /**< Time range */
};

#endif /* defined(__CurveEditor__EditorModel__) */
