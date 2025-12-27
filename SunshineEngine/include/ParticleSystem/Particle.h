#pragma once

#include <ParticleSystem/Particle.h>

#include <d3d11.h>
#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;


namespace SE
{
    struct Particle {
        DX::XMFLOAT4 Position;
        DX::XMFLOAT4 Velocity;
        DX::XMFLOAT4 ColorStart;
        DX::XMFLOAT4 ColorEnd;

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
