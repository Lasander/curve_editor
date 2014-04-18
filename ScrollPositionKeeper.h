//
//  ScrollPositionKeeper.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 1.1.14.
//
//

#ifndef __CurveEditor__ScrollPositionKeeper__
#define __CurveEditor__ScrollPositionKeeper__

#include <QObject>

QT_BEGIN_NAMESPACE
class QAbstractSlider;
QT_END_NAMESPACE

/** Keep the relative position of a QAbstractSlider when the slider range changes.
 If scroll range appears for the first time assumes relative position in the middle. */
class ScrollPositionKeeper : public QObject
{
    Q_OBJECT
    
public:
    ScrollPositionKeeper(QAbstractSlider* slider, QObject* parent);
    ~ScrollPositionKeeper();
    
private slots:
    void sliderRangeChanged(int min, int max);
    
private:
    QAbstractSlider* m_slider;
    int m_previousSliderMin;
    int m_previousSliderMax;
};


#endif /* defined(__CurveEditor__ScrollPositionKeeper__) */
