#pragma once

#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/string.h>

#include "RenderTechnique.h"

#include <Physics/PhysicsEnums.h>
#include <Physics/CollisionUtils.h>
#include <Graphics/Utils/Color.h>

class ID3D11Device;
class ID3D11DeviceContext;

namespace SE_G {
    namespace Bind {
        template <typename T>
        class VertexConstantBuffer;
    }


    class ColliderTechnique :
        public RenderTechnique
    {
    public:
        ColliderTechnique(ID3D11Device* device, TransformComponent* assignedTransform,
            eastl::string technique, eastl::shared_ptr<SE::ColliderData> colliderData);

        void Pass(ID3D11DeviceContext* context) override;
        void BindAll(ID3D11DeviceContext* context) override;
        void DrawTechnique(ID3D11DeviceContext* context) override;

        void UpdateTransformCB(ID3D11DeviceContext* context);
        void UpdateSettingsCB(ID3D11DeviceContext* context);

    private:
        eastl::shared_ptr<SE::ColliderData> m_colliderData;

        eastl::unique_ptr<Bind::VertexConstantBuffer<SE::ColliderTransformCB>> m_transformCB;
        eastl::unique_ptr<Bind::VertexConstantBuffer<SE::ColliderSettings>> m_settingsCB;

        // To-do: implement custom mesh collider
        // void InitCustomColliderMesh();
        // eastl::unique_ptr<Bind::VertexBuffer> m_customVertexBuffer;

    private:
        static void InitStaticData(ID3D11Device* device);

        static bool s_staticDataInitializated;
        // every object need constantBuffer for settings
        
        // (radius)
        static eastl::shared_ptr<Bind::VertexShader> s_sphereShader;

        // (xyz-size)
        static eastl::shared_ptr<Bind::VertexShader> s_boxShader;

        // (upper, lower radius, height)
        static eastl::shared_ptr<Bind::VertexShader> s_capsuleShader;

        // (upper, lower radius, height)
        static eastl::shared_ptr<Bind::VertexShader> s_taperedCapsuleShader;

        // (at least scaling or full transform buffer, see local transform)
        static eastl::shared_ptr<Bind::VertexShader> s_customShader;

    };

}