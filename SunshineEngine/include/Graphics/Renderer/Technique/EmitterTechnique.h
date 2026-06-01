#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>

#include "RenderTechnique.h"

#include <Physics/PhysicsEnums.h>
#include <Physics/CollisionUtils.h>

namespace DX = DirectX;
namespace DXSM = DirectX::SimpleMath;
#include <SimpleMath.h>

class ID3D11Device;
class ID3D11DeviceContext;

namespace SE
{
    enum class EmitterType : UINT {
        Point,
        // To-do: add other emitter shapes
        Count
    };

    struct PointEmitterSettings
    {
		float minLongtitude = 0.0f;
		float maxLongtitude = DX::XM_2PI;
		float minLatitude = -DX::XM_PIDIV2;
		float maxLatitude = DX::XM_PIDIV2;
	};

    struct EmitterSettings
    {
        union
        {
            PointEmitterSettings asPoint;
            // To-do: add other emitter settings
		} data;
    };

    struct EmitterData
    {
		EmitterSettings settings;
		EmitterType emitterType = EmitterType::Point;
    };

}

namespace SE
{
    class ParticleData;
}

namespace SE_G {
    namespace Bind {
        template <typename T>
        class VertexConstantBuffer;
    }

    class EmitterTechnique :
        public RenderTechnique
    {
    public:
        EmitterTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
            eastl::string technique, SE::ParticleData* particleData);

        void Pass(ID3D11DeviceContext* context) override;
        void BindAll(ID3D11DeviceContext* context) override;
        void DrawTechnique(ID3D11DeviceContext* context) override;

        void UpdateSettingsCB(ID3D11DeviceContext* context);

    private:
        SE::ParticleData* m_particleData;

        SE::EmitterData m_emitterData;

        eastl::unique_ptr<Bind::VertexConstantBuffer<SE::EmitterSettings>> m_settingsCB;

    private:
        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        
        static eastl::shared_ptr<Bind::VertexShader> s_pointEmitterShader;
    };
}