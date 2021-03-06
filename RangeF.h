//
//  RangeF.h
//  CurveEditor
//
//  Created by Lasse Lopperi on 11.1.14.
//
//

#ifndef __CurveEditor__RangeF__
#define __CurveEditor__RangeF__

/** Inclusive float range [min, max]. */
struct RangeF
{
    float min;
    float max;

    /** Make an invalid range */
    RangeF();
    
    RangeF(float min, float max);
    
    /** @return true if max is greater of equal to min. */
    bool isValid() const;
    
    bool operator==(const RangeF& range) const;
    bool operator!=(const RangeF& range) const;

    /** @return True if the give value is within this range. Note: No value is within an invalid range. */
    bool isInRange(float value) const;

    /** @return Return in-range value closest to the given value. Note: Given value returned with invalid range. */
    float clampToRange(float value) const;

    /** @return Range covering both given ranges. Invalid ranges are ignored. */
	static RangeF makeUnion(const RangeF& range1, const RangeF& range2);
};

inline RangeF::RangeF()
: 	min(0), max(-1)
{
}

inline RangeF::RangeF(float min_, float max_)
: 	min(min_), max(max_)
{
}

inline bool RangeF::isValid() const
{
    return max >= min;
}

inline bool RangeF::operator==(const RangeF& range) const
{
    return (min == range.min) && (max == range.max);
}
inline bool RangeF::operator!=(const RangeF& range) const
{
    return !(*this == range);
}

#endif /* (defined __CurveEditor__RangeF__) */
