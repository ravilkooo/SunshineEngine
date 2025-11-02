#pragma once
#include "RenderTechnique.h"
#include <Bindable/ConstantBuffer.h>
#include <Utils/UUID.h>


class GPassTechnique :
    public RenderTechnique
{
public:
    eastl::shared_ptr<Bind::PixelConstantBuffer<UUIDhilo>> m_uuidBuffer;

    GPassTechnique(ID3D11Device* device, eastl::string technique,
        Sunshine::UUID uuid);
    ~GPassTechnique() = default;
    
    // add uuid constant buffer
    // add vshader
    // add pshader (texture or no texture)
    // add texture (if has texture)
    // add texture sampler (if has texture)
};

