//
//  ScrollPositionKeeper.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 1.1.14.
//
//

#include "ScrollPositionKeeper.h"
#include <QAbstractSlider>
#include <QDebug>
#include <assert.h>

ScrollPositionKeeper::ScrollPositionKeeper(QAbstractSlider* slider, QObject* parent)
:	QObject(parent),
	m_slider(slider),
	m_previousSliderMin(m_slider->minimum()),
	m_previousSliderMax(m_slider->maximum())
{
    connect(m_slider, &QAbstractSlider::rangeChanged, this, &ScrollPositionKeeper::sliderRangeChanged);
}

ScrollPositionKeeper::~ScrollPositionKeeper()
{
}

void ScrollPositionKeeper::sliderRangeChanged(int min, int max)
{
    qDebug() << "ScrollPositionKeeper::sliderRangeChanged" << m_previousSliderMin << m_previousSliderMax << min << max;
    
    int rangeBefore = m_previousSliderMax - m_previousSliderMin;
    float relativeScroll = (rangeBefore > 0) ? static_cast<float>(m_slider->value() - m_previousSliderMin) / static_cast<float>(rangeBefore) : 0.5;
    assert(relativeScroll >= 0);
    assert(relativeScroll <= 1);
    
    int rangeAfter = max - min;

    m_previousSliderMin = min;
    m_previousSliderMax = max;
    
    m_slider->setValue(min + (relativeScroll * rangeAfter));
}
