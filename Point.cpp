#include "Point.h"

bool Point::isSelected() const
{
    return m_isSelected;
}
void Point::setSelected(bool isSelected)
{
    m_isSelected = isSelected;
}
bool Point::operator==(const Point& other) const
{
    return
        (m_isValid == other.m_isValid) &&
        (m_isSelected == other.m_isSelected) &&
        (m_id == other.m_id) &&
        (m_time == other.m_time) &&
        (m_value == other.m_value);
}
