#include "PointId.h"
#include <QDebug>
#include <QMutex>

PointId PointId::generateId()
{
    static PointId s_id = invalidId();

    static QMutex s_mutex;
    s_mutex.lock();
    PointId new_id(s_id.getId() + 1);
    s_id = new_id;
    s_mutex.unlock();

    // Get new if invalid
    if (!new_id.isValid())
        return generateId();

    return new_id;
}

PointId PointId::invalidId()
{
    return PointId(0);
}

bool PointId::isValid() const
{
    return !((*this) == invalidId());
}

bool PointId::operator==(const PointId& id) const
{
    return m_id == id.m_id;
}

bool PointId::operator<(const PointId& id) const
{
    return m_id < id.m_id;
}

PointId::PointId(const PointId& id)
  : m_id(id.m_id)
{
}

PointId& PointId::operator=(const PointId& id)
{
    m_id = id.m_id;
    return *this;
}

PointId::PointId(int id)
  : m_id(id)
{
}

int PointId::getId() const
{
    return m_id;
}

QDebug operator<<(QDebug dbg, const PointId& id)
{
     dbg.nospace() << "PointId(" << id.m_id << ")";
     return dbg.space();
 }

