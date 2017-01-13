// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Version 4 Restricted Libraries source code is supplied
// under the terms of the license agreement
//     http://www.geometrictools.com/License/Wm4RestrictedLicense.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#include "Wm4GraphicsPCH.h"
#include "Wm4BoxBVTree.h"
#include "Wm4ContBox3.h"
using namespace Wm4;

WM4_IMPLEMENT_INITIALIZE(BoxBVTree);

//----------------------------------------------------------------------------
void BoxBVTree::Initialize ()
{
    ms_aoCreateModelBound[BoundingVolume::BV_BOX] =
        &BoxBVTree::CreateModelBound;

    ms_aoCreateWorldBound[BoundingVolume::BV_BOX] =
        &BoxBVTree::CreateWorldBound;
}
//----------------------------------------------------------------------------
BoxBVTree::BoxBVTree (const TriMesh* pkMesh, int iMaxTrisPerLeaf,
    bool bStoreInteriorTris)
    :
    BoundingVolumeTree(BoundingVolume::BV_BOX,pkMesh,iMaxTrisPerLeaf,
        bStoreInteriorTris)
{
}
//----------------------------------------------------------------------------
BoundingVolume* BoxBVTree::CreateModelBound (const TriMesh* pkMesh, int i0,
    int i1, int* aiISplit, Line3f& rkLine)
{
    // tag vertices that are used in the submesh
    int iVQuantity = pkMesh->VBuffer->GetVertexQuantity();
    const int* aiIndex = pkMesh->IBuffer->GetData();
    bool* abValid = WM4_NEW bool[iVQuantity];
    memset(abValid,0,iVQuantity*sizeof(bool));
    int i;
    for (i = i0; i <= i1; i++)
    {
        int j = 3*aiISplit[i];
        abValid[aiIndex[j++]] = true;
        abValid[aiIndex[j++]] = true;
        abValid[aiIndex[j++]] = true;
    }

    // Create a contiguous set of vertices in the submesh.
    std::vector<Vector3f> kMeshVertices;
    for (i = 0; i < iVQuantity; i++)
    {
        if (abValid[i])
        {
            kMeshVertices.push_back(pkMesh->VBuffer->Position3(i));
        }
    }
    WM4_DELETE[] abValid;

    BoxBV* pkModelBound = WM4_NEW BoxBV;
    pkModelBound->Box() = ContOrientedBox<float>((int)kMeshVertices.size(),
        &kMeshVertices.front());

    rkLine.Origin = pkModelBound->Box().Center;
    rkLine.Direction = pkModelBound->Box().Axis[2];
    return pkModelBound;
}
//----------------------------------------------------------------------------
BoundingVolume* BoxBVTree::CreateWorldBound ()
{
    return WM4_NEW BoxBV;
}
//----------------------------------------------------------------------------
