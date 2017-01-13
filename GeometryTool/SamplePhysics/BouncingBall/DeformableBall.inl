// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Version 4 Restricted Libraries source code is supplied
// under the terms of the license agreement
//     http://www.geometrictools.com/License/Wm4RestrictedLicense.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

//----------------------------------------------------------------------------
inline float DeformableBall::GetDuration () const
{
    return m_fDuration;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetPeriod () const
{
    return m_fPeriod;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetMinActive () const
{
    return m_fMinActive;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetMaxActive () const
{
    return m_fMaxActive;
}
//----------------------------------------------------------------------------
inline bool& DeformableBall::DoAffine ()
{
    return m_bDoAffine;
}
//----------------------------------------------------------------------------
inline TriMeshPtr& DeformableBall::Mesh ()
{
    return m_spkMesh;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetAmplitude (float fTime)
{
    return m_fDeformMult*(fTime-m_fMinActive)*(m_fMaxActive-fTime);
}
//----------------------------------------------------------------------------
