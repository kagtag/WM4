// Geometric Tools, Inc.
// http://www.geometrictools.com
// Copyright (c) 1998-2006.  All Rights Reserved
//
// The Wild Magic Version 4 Restricted Libraries source code is supplied
// under the terms of the license agreement
//     http://www.geometrictools.com/License/Wm4RestrictedLicense.pdf
// and may not be copied or disclosed except in accordance with the terms
// of that agreement.

#ifndef WM4INTARRAY_H
#define WM4INTARRAY_H

#include "Wm4GraphicsLIB.h"
#include "Wm4TSharedArray.h"

namespace Wm4
{
typedef TSharedArray<int> IntArray;
typedef Pointer<IntArray> IntArrayPtr;
WM4_REGISTER_STREAM(IntArray);
}

#endif
