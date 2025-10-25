#pragma once
#include "RenderTechnique.h"



class GPassTechnique :
    public RenderTechnique
{
public:
    GPassTechnique(ID3D11Device* device, eastl::string technique);
    ~GPassTechnique() = default;
    
    // add uuid constant buffer
    // add vshader
    // add pshader (texture or no texture)
    // add texture (if has texture)
    // add texture sampler (if has texture)
};

