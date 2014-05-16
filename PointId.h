#ifndef POINTID_H
#define POINTID_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QDebug;
QT_END_NAMESPACE

/** Unique identification for a point within curves */
class PointId
{
public:
    /**
     * @return New unique point id's.
     * @par Thread-safe.
     */
    static PointId generateId();

    /** @return Invalid point id */
    static PointId invalidId();

    /**
     * @brief Check point if validity
     * @param pointId Id
     * @return True if the id is valid
     */
    bool isValid() const;

    /** @return True if point ids are equal */
    bool operator==(const PointId& id) const;

    /** @return True if given this id is smaller than given id */
    bool operator<(const PointId& id) const;

    /** Copy point id */
    PointId(const PointId& id);

    /** Assign point id */
    PointId& operator=(const PointId& id);

private:
    explicit PointId(int id);
    int getId() const;

    int m_id;

    friend QDebug operator<<(QDebug dbg, const PointId& id);
    friend uint qHash(const PointId& key, uint seed) Q_DECL_NOTHROW;
};

/** Debug stream output for PointId */
QDebug operator<<(QDebug dbg, const PointId& id);

/** Hash function for PointId */
inline uint qHash(const PointId& key, uint seed = 0) Q_DECL_NOTHROW
{
    return uint(key.m_id) ^ seed;
}



#endif // POINTID_H
