#ifndef PT_MATH_KB_SPLINE_H
#define PT_MATH_KB_SPLINE_H

#include "cubic_hermite_spline.h"
#include "kb_data_set.h"

namespace pt { namespace math {

template<typename T>
class kb_spline : public cubic_hermite_spline<kb_data_set<T>>
{
public:
    kb_spline(kb_data_set<T> const data)
    :   cubic_hermite_spline<kb_data_set<T>>(data)
    {
    }
};

typedef kb_spline<float> kb_splinef;

}} // namespace pt::math

#endif
