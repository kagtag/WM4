// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Version 4 Foundation Library source code is supplied
// under the terms of the license agreement
//     http://www.geometrictools.com/License/Wm4FoundationLicense.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm4FoundationPCH.h"
#include "Wm4NaturalSpline2.h"
#include "Wm4Integrate1.h"
#include "Wm4LinearSystem.h"
#include "Wm4Polynomial1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
NaturalSpline2<Real>::NaturalSpline2 (BoundaryType eType, int iSegments,
    Real* afTime, Vector2<Real>* akPoint)
    :
    MultipleCurve2<Real>(iSegments,afTime)
{
    m_akA = akPoint;

    switch (eType)
    {
        case BT_FREE:
        {
            CreateFreeSpline();
            break;
        }
        case BT_CLAMPED:
        {
            CreateClampedSpline();
            break;
        }
        case BT_CLOSED:
        {
            CreateClosedSpline();
            break;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
NaturalSpline2<Real>::~NaturalSpline2 ()
{
    WM4_DELETE[] m_akA;
    WM4_DELETE[] m_akB;
    WM4_DELETE[] m_akC;
    WM4_DELETE[] m_akD;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline2<Real>::CreateFreeSpline ()
{
    Real* afDt = WM4_NEW Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
    {
        afDt[i] = m_afTime[i+1] - m_afTime[i];
    }

    Real* afD2t = WM4_NEW Real[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
    {
        afD2t[i] = m_afTime[i+1] - m_afTime[i-1];
    }

    Vector2<Real>* akAlpha = WM4_NEW Vector2<Real>[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
    {
        Vector2<Real> kNumer = ((Real)3.0)*(m_akA[i+1]*afDt[i-1] -
            m_akA[i]*afD2t[i] + m_akA[i-1]*afDt[i]);
        Real fInvDenom = ((Real)1.0)/(afDt[i-1]*afDt[i]);
        akAlpha[i] = fInvDenom*kNumer;
    }

    Real* afEll = WM4_NEW Real[m_iSegments+1];
    Real* afMu = WM4_NEW Real[m_iSegments];
    Vector2<Real>* akZ = WM4_NEW Vector2<Real>[m_iSegments+1];
    Real fInv;

    afEll[0] = (Real)1.0;
    afMu[0] = (Real)0.0;
    akZ[0] = Vector2<Real>::ZERO;
    for (i = 1; i < m_iSegments; i++)
    {
        afEll[i] = ((Real)2.0)*afD2t[i] - afDt[i-1]*afMu[i-1];
        fInv = ((Real)1.0)/afEll[i];
        afMu[i] = fInv*afDt[i];
        akZ[i] = fInv*(akAlpha[i] - akZ[i-1]*afDt[i-1]);
    }
    afEll[m_iSegments] = (Real)1.0;
    akZ[m_iSegments] = Vector2<Real>::ZERO;

    m_akB = WM4_NEW Vector2<Real>[m_iSegments];
    m_akC = WM4_NEW Vector2<Real>[m_iSegments+1];
    m_akD = WM4_NEW Vector2<Real>[m_iSegments];

    m_akC[m_iSegments] = Vector2<Real>::ZERO;

    const Real fOneThird = ((Real)1.0)/(Real)3.0;
    for (i = m_iSegments-1; i >= 0; i--)
    {
        m_akC[i] = akZ[i] - afMu[i]*m_akC[i+1];
        fInv = ((Real)1.0)/afDt[i];
        m_akB[i] = fInv*(m_akA[i+1] - m_akA[i]) - fOneThird*afDt[i]*(
            m_akC[i+1] + ((Real)2.0)*m_akC[i]);
        m_akD[i] = fOneThird*fInv*(m_akC[i+1] - m_akC[i]);
    }

    WM4_DELETE[] afDt;
    WM4_DELETE[] afD2t;
    WM4_DELETE[] akAlpha;
    WM4_DELETE[] afEll;
    WM4_DELETE[] afMu;
    WM4_DELETE[] akZ;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline2<Real>::CreateClampedSpline ()
{
    Real* afDt = WM4_NEW Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
    {
        afDt[i] = m_afTime[i+1] - m_afTime[i];
    }

    Real* afD2t = WM4_NEW Real[m_iSegments];
    for (i = 1; i < m_iSegments; i++)
    {
        afD2t[i] = m_afTime[i+1] - m_afTime[i-1];
    }

    Vector2<Real>* akAlpha = WM4_NEW Vector2<Real>[m_iSegments+1];
    Real fInv = ((Real)1.0)/afDt[0];
    akAlpha[0] = ((Real)3.0)*(fInv - (Real)1.0)*(m_akA[1] - m_akA[0]);
    fInv = ((Real)1.0)/afDt[m_iSegments-1];
    akAlpha[m_iSegments] = ((Real)3.0)*((Real)1.0 - fInv)*(m_akA[m_iSegments]
        - m_akA[m_iSegments-1]);
    for (i = 1; i < m_iSegments; i++)
    {
        Vector2<Real> kNumer = ((Real)3.0)*(m_akA[i+1]*afDt[i-1] -
            m_akA[i]*afD2t[i] + m_akA[i-1]*afDt[i]);
        Real fInvDenom = ((Real)1.0)/(afDt[i-1]*afDt[i]);
        akAlpha[i] = fInvDenom*kNumer;
    }

    Real* afEll = WM4_NEW Real[m_iSegments+1];
    Real* afMu = WM4_NEW Real[m_iSegments];
    Vector2<Real>* akZ = WM4_NEW Vector2<Real>[m_iSegments+1];

    afEll[0] = ((Real)2.0)*afDt[0];
    afMu[0] = (Real)0.5;
    fInv = ((Real)1.0)/afEll[0];
    akZ[0] = fInv*akAlpha[0];

    for (i = 1; i < m_iSegments; i++)
    {
        afEll[i] = ((Real)2.0)*afD2t[i] - afDt[i-1]*afMu[i-1];
        fInv = ((Real)1.0)/afEll[i];
        afMu[i] = fInv*afDt[i];
        akZ[i] = fInv*(akAlpha[i] - akZ[i-1]*afDt[i-1]);
    }
    afEll[m_iSegments] = afDt[m_iSegments-1]*((Real)2.0 -
        afMu[m_iSegments-1]);
    fInv = ((Real)1.0)/afEll[m_iSegments];
    akZ[m_iSegments] = fInv*(akAlpha[m_iSegments] -
        akZ[m_iSegments-1]*afDt[m_iSegments-1]);

    m_akB = WM4_NEW Vector2<Real>[m_iSegments];
    m_akC = WM4_NEW Vector2<Real>[m_iSegments+1];
    m_akD = WM4_NEW Vector2<Real>[m_iSegments];

    m_akC[m_iSegments] = akZ[m_iSegments];

    const Real fOneThird = ((Real)1.0)/(Real)3.0;
    for (i = m_iSegments-1; i >= 0; i--)
    {
        m_akC[i] = akZ[i] - afMu[i]*m_akC[i+1];
        fInv = ((Real)1.0)/afDt[i];
        m_akB[i] = fInv*(m_akA[i+1] - m_akA[i]) - fOneThird*afDt[i]*(
            m_akC[i+1] + ((Real)2.0)*m_akC[i]);
        m_akD[i] = fOneThird*fInv*(m_akC[i+1] - m_akC[i]);
    }

    WM4_DELETE[] afDt;
    WM4_DELETE[] afD2t;
    WM4_DELETE[] akAlpha;
    WM4_DELETE[] afEll;
    WM4_DELETE[] afMu;
    WM4_DELETE[] akZ;
}
//----------------------------------------------------------------------------
template <class Real>
void NaturalSpline2<Real>::CreateClosedSpline ()
{
    // TO DO.  A general linear system solver is used here.  The matrix
    // corresponding to this case is actually "cyclic banded", so a faster
    // linear solver can be used.  The current linear system code does not
    // have such a solver.

    Real* afDt = WM4_NEW Real[m_iSegments];
    int i;
    for (i = 0; i < m_iSegments; i++)
    {
        afDt[i] = m_afTime[i+1] - m_afTime[i];
    }

    // construct matrix of system
    GMatrix<Real> kMat(m_iSegments+1,m_iSegments+1);
    kMat[0][0] = (Real)1.0;
    kMat[0][m_iSegments] = (Real)-1.0;
    for (i = 1; i <= m_iSegments-1; i++)
    {
        kMat[i][i-1] = afDt[i-1];
        kMat[i][i  ] = ((Real)2.0)*(afDt[i-1] + afDt[i]);
        kMat[i][i+1] = afDt[i];
    }
    kMat[m_iSegments][m_iSegments-1] = afDt[m_iSegments-1];
    kMat[m_iSegments][0] = ((Real)2.0)*(afDt[m_iSegments-1] + afDt[0]);
    kMat[m_iSegments][1] = afDt[0];

    // construct right-hand side of system
    m_akC = WM4_NEW Vector2<Real>[m_iSegments+1];
    m_akC[0] = Vector2<Real>::ZERO;
    Real fInv0, fInv1;
    for (i = 1; i <= m_iSegments-1; i++)
    {
        fInv0 = ((Real)1.0)/afDt[i];
        fInv1 = ((Real)1.0)/afDt[i-1];
        m_akC[i] = ((Real)3.0)*(fInv0*(m_akA[i+1] - m_akA[i]) -
            fInv1*(m_akA[i] - m_akA[i-1]));
    }
    fInv0 = ((Real)1.0)/afDt[0];
    fInv1 = ((Real)1.0)/afDt[m_iSegments-1];
    m_akC[m_iSegments] = ((Real)3.0)*(fInv0*(m_akA[1] - m_akA[0]) -
        fInv1*(m_akA[0] - m_akA[m_iSegments-1]));

    // solve the linear systems
    Real* afInput = WM4_NEW Real[m_iSegments+1];
    Real* afOutput = WM4_NEW Real[m_iSegments+1];

    for (i = 0; i <= m_iSegments; i++)
    {
        afInput[i] = m_akC[i].X();
    }
    LinearSystem<Real>().Solve(kMat,afInput,afOutput);
    for (i = 0; i <= m_iSegments; i++)
    {
        m_akC[i].X() = afOutput[i];
    }

    for (i = 0; i <= m_iSegments; i++)
    {
        afInput[i] = m_akC[i].Y();
    }
    LinearSystem<Real>().Solve(kMat,afInput,afOutput);
    for (i = 0; i <= m_iSegments; i++)
    {
        m_akC[i].Y() = afOutput[i];
    }

    WM4_DELETE[] afInput;
    WM4_DELETE[] afOutput;
    // end linear system solving

    const Real fOneThird = ((Real)1.0)/(Real)3.0;
    m_akB = WM4_NEW Vector2<Real>[m_iSegments];
    m_akD = WM4_NEW Vector2<Real>[m_iSegments];
    for (i = 0; i < m_iSegments; i++)
    {
        fInv0 = ((Real)1.0)/afDt[i];
        m_akB[i] = fInv0*(m_akA[i+1] - m_akA[i]) - fOneThird*(m_akC[i+1] +
            ((Real)2.0)*m_akC[i])*afDt[i];
        m_akD[i] = fOneThird*fInv0*(m_akC[i+1] - m_akC[i]);
    }

    WM4_DELETE[] afDt;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>* NaturalSpline2<Real>::GetPoints () const
{
    return m_akA;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> NaturalSpline2<Real>::GetPosition (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2<Real> kResult = m_akA[iKey] + fDt*(m_akB[iKey] + fDt*(
        m_akC[iKey] + fDt*m_akD[iKey]));

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> NaturalSpline2<Real>::GetFirstDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2<Real> kResult = m_akB[iKey] + fDt*(m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)3.0)*fDt));

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> NaturalSpline2<Real>::GetSecondDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2<Real> kResult = m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)6.0)*fDt);

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> NaturalSpline2<Real>::GetThirdDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    Vector2<Real> kResult = ((Real)6.0)*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline2<Real>::GetSpeedKey (int iKey, Real fTime) const
{
    Vector2<Real> kVelocity = m_akB[iKey] + fTime*(m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)3.0)*fTime));

    return kVelocity.Length();
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline2<Real>::GetLengthKey (int iKey, Real fT0, Real fT1) const
{
    ThisPlusKey kData(this,iKey);
    return Integrate1<Real>::RombergIntegral(8,fT0,fT1,GetSpeedWithData,
        (void*)&kData);
}
//----------------------------------------------------------------------------
template <class Real>
Real NaturalSpline2<Real>::GetVariationKey (int iKey, Real fT0, Real fT1,
    const Vector2<Real>& rkA, const Vector2<Real>& rkB) const
{
    Polynomial1<Real> kXPoly(3);
    kXPoly[0] = m_akA[iKey].X();
    kXPoly[1] = m_akB[iKey].X();
    kXPoly[2] = m_akC[iKey].X();
    kXPoly[3] = m_akD[iKey].X();

    Polynomial1<Real> kYPoly(3);
    kYPoly[0] = m_akA[iKey].Y();
    kYPoly[1] = m_akB[iKey].Y();
    kYPoly[2] = m_akC[iKey].Y();
    kYPoly[3] = m_akD[iKey].Y();

    // construct line segment A + t*B
    Polynomial1<Real> kLx(1), kLy(1);
    kLx[0] = rkA.X();
    kLx[1] = rkB.X();
    kLy[0] = rkA.Y();
    kLy[1] = rkB.Y();

    // compute |X(t) - L(t)|^2
    Polynomial1<Real> kDx = kXPoly - kLx;
    Polynomial1<Real> kDy = kYPoly - kLy;
    Polynomial1<Real> kNormSqr = kDx*kDx + kDy*kDy;

    // compute indefinite integral of |X(t)-L(t)|^2
    Polynomial1<Real> kIntegral(kNormSqr.GetDegree()+1);
    kIntegral[0] = (Real)0.0;
    for (int i = 1; i <= kIntegral.GetDegree(); i++)
    {
        kIntegral[i] = kNormSqr[i-1]/i;
    }

    // compute definite Integral(t0,t1,|X(t)-L(t)|^2)
    Real fResult = kIntegral(fT1) - kIntegral(fT0);
    return fResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class NaturalSpline2<float>;

template WM4_FOUNDATION_ITEM
class NaturalSpline2<double>;
//----------------------------------------------------------------------------
}
