#pragma once

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <Graphics/Renderer/DeferredRenderer.h>


namespace SE_G {
	class MiniViewRenderer :
		public DeferredRenderer
	{
	public:
		// 640*360

		MiniViewRenderer(
			eastl::string name, DeferredRenderer* parentRenderer);
		~MiniViewRenderer();

		void SetParentRenderer(DeferredRenderer* parentRenderer);

		virtual void Pass() override;

		DeferredRenderer* m_parentRenderer;
	};

}
