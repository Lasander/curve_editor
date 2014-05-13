#ifndef POINTID_H
#define POINTID_H

/** Unique identification for a point within a CurveModel */
using PointId = int;

/**
 * @brief Check point if validity
 * @param pointId Id
 * @return True if the id is valid
 */
bool isValid(PointId pointId);

/** @return New unique point id's */
PointId generateId();

/** @return Invalid point id */
PointId invalidId();

#endif // POINTID_H
