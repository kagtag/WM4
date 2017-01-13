// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Version 4 Restricted Libraries source code is supplied
// under the terms of the license agreement
//     http://www.geometrictools.com/License/Wm4RestrictedLicense.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM4INTERSECTINGINTERVALS_H
#define WM4INTERSECTINGINTERVALS_H

#include "Wm4PhysicsLIB.h"
#include "Wm4EdgeKey.h"
#include "Wm4Vector2.h"

namespace Wm4
{

template <class Real>
class WM4_PHYSICS_ITEM IntersectingIntervals
{
public:
    // Intervals are of the form [b,e] where b is the beginning value and e
    // is the ending value with b < e.  The end points are stored as a
    // Vector2.

    // construction and destruction
    IntersectingIntervals (std::vector<Vector2<Real> >& rkInterval);
    ~IntersectingIntervals ();

    // This function is called by the constructor and does the sort-and-sweep
    // to initialize the update system.  However, if you add or remove items
    // from the array of intervals after the constructor call, you will need
    // to call this function once before you start the multiple calls of the
    // update function.
    void Initialize ();

    // After the system is initialized, you can move the intervals using this
    // function.  It is not enough to modify the input array of intervals
    // since the end point values stored internally by this class must also
    // change.  You can also retrieve the current interval information.
    void SetInterval (int i, Real fMin, Real fMax);
    void GetInterval (int i, Real& rfMin, Real& rfMax) const;
    
    // When you are finished moving intervals, call this function to determine
    // the overlapping intervals.  An incremental update is applied to
    // determine the new set of overlapping intervals.
    void Update ();

    // If (i,j) is in the overlap set, then interval i and interval j are
    // overlapping.  The indices are those for the the input array.  The
    // set elements (i,j) are stored so that i < j.
    const std::set<EdgeKey>& GetOverlap () const;

private:
    class WM4_PHYSICS_ITEM EndPoint
    {
    public:
        Real Value;
        int Type;  // '0' if interval min, '1' if interval max
        int Index;  // index of interval containing this end point

        // support for sorting of end points
        bool operator< (const EndPoint& rkEP) const
        {
            if (Value < rkEP.Value)
            {
                return true;
            }
            if (Value > rkEP.Value)
            {
                return false;
            }
            return Type < rkEP.Type;
        }
    };

    std::vector<Vector2<Real> >& m_rkInterval;
    std::vector<EndPoint> m_kEndPoint;
    std::set<EdgeKey> m_kOverlap;

    // The intervals are indexed 0 <= i < n.  The end point array has 2*n
    // entries.  The original 2*n interval values are ordered as b[0], e[0],
    // b[1], e[1], ..., b[n-1], e[n-1].  When the end point array is sorted,
    // the mapping between interval values and end points is lost.  In order
    // to modify interval values that are stored in the end point array, we
    // need to maintain the mapping.  This is done by the following lookup
    // table of 2*n entries.  The value m_kLookup[2*i] is the index of b[i]
    // in the end point array.  The value m_kLookup[2*i+1] is the index of
    // e[i] in the end point array.
    std::vector<int> m_kLookup;
};

typedef IntersectingIntervals<float> IntersectingIntervalsf;
typedef IntersectingIntervals<double> IntersectingIntervalsd;

}

#endif
