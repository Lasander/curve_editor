#ifndef PT_MATH_CUBIC_HERMITE_SPLINE_INL
#define PT_MATH_CUBIC_HERMITE_SPLINE_INL

namespace pt { namespace math {

// cubic_hermite_spline

template<typename DataSet>
inline cubic_hermite_spline<DataSet>::cubic_hermite_spline()
:   m_data()
{
}

template<typename DataSet>
inline typename cubic_hermite_spline<DataSet>::result_type
    cubic_hermite_spline<DataSet>::value_at(float time) const
{
    typename DataSet::const_iterator iter = m_data.optional_endpoint(time);
    if (iter == m_data.end())
    {
        typename DataSet::point_pair points = m_data.points_at(time);

        if (points.second == m_data.end())
        {
            // exact match.
            return points.first->value();
        }

        float p0 = points.first->value();
        float p1 = points.second->value();
        float t0 = points.first->starting_tangent();
        float t1 = points.second->ending_tangent();

        float h = points.second->time() - points.first->time();
        float t = (time - points.first->time()) / h;
        float t2 = t * t;
        float t3 = t * t * t;

        float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
        float h10 = t3 - 2.0f * t2 + t;
        float h01 = -2.0f * t3 + 3.0f * t2;
        float h11 = t3 - t2;

        float p = h00 * p0 + h10 * t0 + h01 * p1 + h11 * t1;

        return  p;
    }
    else
    {
        return iter->value();
    }
}

}} // namespace pt::math

#endif
