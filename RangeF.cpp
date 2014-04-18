//
//  RangeF.cpp
//  CurveEditor
//
//  Created by Lasse Lopperi on 18.1.14.
//
//

#include "RangeF.h"
#include <qglobal.h>

RangeF RangeF::makeUnion(const RangeF& range1, const RangeF& range2)
{
	if (!range1.isValid())
        return range2;
    
	if (!range2.isValid())
        return range1;
    
    return RangeF(qMin(range1.min, range2.min), std::max(range1.max, range2.max));
}