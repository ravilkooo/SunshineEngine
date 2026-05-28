#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;


namespace SE
{
    struct Particle {
        DX::XMFLOAT4 Position;
        DX::XMFLOAT4 Velocity;

        DX::XMFLOAT3 ColorStart;
        float AlphaStart;

        DX::XMFLOAT3 ColorEnd;
        float AlphaEnd;

        float SizeStart;
        float SizeEnd;
        float Age;
        float LifeSpan;

        float ScreenSpin;
        float ScreenSpinSpeed;
        float WorldSpin;
        float WorldSpinSpeed;

        DX::XMFLOAT3 WorldSpinAxis;
        float Mass;

        float orientation;
        DX::XMFLOAT3 padding;
    };

    struct ParticleIndexElement
    {
        float distance; //squared distance from camera
        float index; //index in the particle buffer
    };
}
