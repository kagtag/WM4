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
#include "Wm4MultitextureEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,MultitextureEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(MultitextureEffect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(MultitextureEffect,ShaderEffect);

//----------------------------------------------------------------------------
MultitextureEffect::MultitextureEffect (int iTextureQuantity)
    :
    ShaderEffect(1)
{
    m_akImageName = 0;
    SetTextureQuantity(iTextureQuantity);
}
//----------------------------------------------------------------------------
MultitextureEffect::MultitextureEffect ()
{
    m_iTextureQuantity = 0;
    m_akImageName = 0;
}
//----------------------------------------------------------------------------
MultitextureEffect::~MultitextureEffect ()
{
}
//----------------------------------------------------------------------------
void MultitextureEffect::SetTextureQuantity (int iTextureQuantity)
{
    assert(iTextureQuantity >= 1);
    m_iTextureQuantity = iTextureQuantity;
    WM4_DELETE[] m_akImageName;
    m_akImageName = WM4_NEW std::string[m_iTextureQuantity];

    m_kAlphaState.resize(m_iTextureQuantity);
    SetDefaultAlphaState();
}
//----------------------------------------------------------------------------
int MultitextureEffect::GetTextureQuantity () const
{
    return m_iTextureQuantity;
}
//----------------------------------------------------------------------------
void MultitextureEffect::SetImageName (int i, const std::string& rkImageName)
{
    assert(0 <= i && i < m_iTextureQuantity);
    m_akImageName[i] = rkImageName;
}
//----------------------------------------------------------------------------
const std::string& MultitextureEffect::GetImageName (int i) const
{
    assert(0 <= i && i < m_iTextureQuantity);
    return m_akImageName[i];
}
//----------------------------------------------------------------------------
void MultitextureEffect::Configure ()
{
    if (m_iTextureQuantity == 1)
    {
        m_kVShader[0] = WM4_NEW VertexShader("Texture");
        m_kPShader[0] = WM4_NEW PixelShader("Texture");
        m_kPShader[0]->SetTextureQuantity(1);
        m_kPShader[0]->SetImageName(0,m_akImageName[0]);
        return;
    }

    const size_t uiNumberSize = 4;
    char acNumber[uiNumberSize];

    // In a single-effect drawing pass, texture 0 is a source to be blended
    // with a nonexistent destination.  As such, we think of the source mode
    // as SBF_ONE and the destination mode as SDF_ZERO.
    std::string kVShaderName("T0d2");
    std::string kPShaderName("T0s1d0");
    int i;
    for (i = 1; i < m_iTextureQuantity; i++)
    {
        kVShaderName += "T";
        kPShaderName += "T";
        System::Sprintf(acNumber,uiNumberSize,"%d",i);
        kVShaderName += std::string(acNumber) + std::string("d2");
        kPShaderName += std::string(acNumber);

        AlphaState* pkAS = m_kAlphaState[i];

        // Source blending mode.
        kPShaderName += "s";
        System::Sprintf(acNumber,uiNumberSize,"%d",(int)pkAS->SrcBlend);
        kPShaderName += std::string(acNumber);

        // Destination blending mode.
        kPShaderName += "d";
        System::Sprintf(acNumber,uiNumberSize,"%d",(int)pkAS->DstBlend);
        kPShaderName += std::string(acNumber);
    }
    kVShaderName += std::string("PassThrough");

    m_kVShader[0] = WM4_NEW VertexShader(kVShaderName);
    m_kPShader[0] = WM4_NEW PixelShader(kPShaderName);
    m_kPShader[0]->SetTextureQuantity(m_iTextureQuantity);
    for (i = 0; i < m_iTextureQuantity; i++)
    {
        m_kPShader[0]->SetImageName(i,m_akImageName[i]);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void MultitextureEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iTextureQuantity);
    SetTextureQuantity(m_iTextureQuantity);
    for (int i = 0; i < m_iTextureQuantity; i++)
    {
        rkStream.Read(m_akImageName[i]);
    }

    WM4_END_DEBUG_STREAM_LOAD(MultitextureEffect);
}
//----------------------------------------------------------------------------
void MultitextureEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool MultitextureEffect::Register (Stream& rkStream) const
{
    return ShaderEffect::Register(rkStream);
}
//----------------------------------------------------------------------------
void MultitextureEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // native data
    rkStream.Write(m_iTextureQuantity);
    for (int i = 0; i < m_iTextureQuantity; i++)
    {
        rkStream.Write(m_akImageName[i]);
    }

    WM4_END_DEBUG_STREAM_SAVE(MultitextureEffect);
}
//----------------------------------------------------------------------------
int MultitextureEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = ShaderEffect::GetDiskUsed(rkVersion) +
        sizeof(m_iTextureQuantity);
 
    for (int i = 0; i < m_iTextureQuantity; i++)
    {
        iSize += sizeof(int) + (int)m_akImageName[i].length();
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* MultitextureEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("texture quantity =",m_iTextureQuantity));

    const size_t uiTitleSize = 16;
    char acTitle[uiTitleSize];
    for (int i = 0; i < m_iTextureQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"image[%d] =",i);
        pkTree->Append(Format(acTitle,m_akImageName[i].c_str()));
    }

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
