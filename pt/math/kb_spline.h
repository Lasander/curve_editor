#ifndef PT_MATH_KB_SPLINE_H
#define PT_MATH_KB_SPLINE_H

#include "cubic_hermite_spline.h"
#include "kb_data_set.h"
//#include "spline_value_source.h"

namespace pt { namespace math {

template<typename T, int C>
class kb_spline : public cubic_hermite_spline<kb_data_set<T>,C>
{
public:
    kb_spline(kb_data_set<T> const data[C])
    :   cubic_hermite_spline<kb_data_set<T>,C>(data)
    {
    }
};

template<typename T>
class kb_spline<T,1> : public cubic_hermite_spline<kb_data_set<T>,1>
{
public:
    kb_spline(kb_data_set<T> const data)
    :   cubic_hermite_spline<kb_data_set<T>,1>(data)
    {
    }
};

typedef kb_spline<float,1> kb_splinef;
typedef kb_spline<float,2> kb_spline2f;
typedef kb_spline<float,3> kb_spline3f;
typedef kb_spline<float,4> kb_spline4f;

//typedef spline_value_source<kb_spline<float,1> > kb_splinef_value_source;
//typedef spline_value_source<kb_spline<float,2> > kb_spline2f_value_source;
//typedef spline_value_source<kb_spline<float,3> > kb_spline3f_value_source;
//typedef spline_value_source<kb_spline<float,4> > kb_spline4f_value_source;

}} // namespace pt::math

#endif
