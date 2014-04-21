//
//  ScaleView.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 29.12.13.
//
//

#ifndef __CurveEditor__ScaleView__
#define __CurveEditor__ScaleView__

#include "TransformationNode.h"
#include "RangeF.h"
#include "GraphicsItem.h"
#include <QObject>
#include <QList>

class ScaleLineView;

class ScaleView :
	public QObject,
	private TransformationNode
{
public:
    ScaleView(int numberOfScaleLines, const RangeF& timeRange, QGraphicsItem* parent);
    ~ScaleView();
    
public slots:
    void setValueScale(RangeF valueScale);
    void setTimeRange(RangeF timeRange);
    
private:
    RangeF m_valueScale;
    RangeF m_timeRange;
    QList<ScaleLineView*> m_scaleLines;
};

#endif /* defined(__CurveEditor__ScaleView__) */
