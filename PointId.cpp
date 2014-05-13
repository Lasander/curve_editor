#include "PointId.h"

bool isValid(PointId pointId)
{
    // Zero is invalid
    return pointId != 0;
}

PointId generateId()
{
    static PointId g_id = 1;
    PointId id = ++g_id;

    // Skip 0 as invalid id
    if (id == 0)
        return generateId();

    return id;
}

PointId invalidId()
{
    return 0;
}
