#ifndef PT_MATH_CUBIC_HERMITE_SPLINE_INL
#define PT_MATH_CUBIC_HERMITE_SPLINE_INL

namespace pt { namespace math {

// cubic_hermite_spline

template<typename DataSet, int C>
inline cubic_hermite_spline<DataSet,C>::cubic_hermite_spline(
    DataSet const data[C])
{
    for (size_t i = 0; i < C; ++i)
    {
        assert(data[i].size() > 0);
        m_data[i] = data[i];
    }
}

template<typename DataSet, int C>
inline typename cubic_hermite_spline<DataSet,C>::result_type
    cubic_hermite_spline<DataSet,C>::value_at(float time) const
{
    result_type result;

    for (int i = 0; i < C; ++i)
    {
        typename DataSet::const_iterator iter = m_data[i].optional_endpoint(time);
        if (iter == m_data[i].end())
        {
            typename DataSet::point_pair points = m_data[i].points_at(time);

            if (points.second == m_data[i].end())
            {
                // exact match.
                result[i] = points.first->value();
                continue;
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

            result[i] = p;
        }
        else
        {
            result[i] = iter->value();
        }
    }

    return result;
}

template<typename DataSet>
inline cubic_hermite_spline<DataSet,1>::cubic_hermite_spline(
    DataSet const& data)
:   m_data(data)
{
//    assert(data.size() > 0);
}

template<typename DataSet>
inline typename cubic_hermite_spline<DataSet,1>::result_type
    cubic_hermite_spline<DataSet,1>::value_at(float time) const
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
