#pragma once

#include <d3d11.h>
#include <wrl.h>

namespace Bind
{
	enum class PipelineStage {
		VERTEX_SHADER, PIXEL_SHADER, COMPUTE_SHADER
	};

	class Bindable
	{
	public:
		virtual void Bind(ID3D11DeviceContext* context) noexcept = 0;
		virtual void Unbind(ID3D11DeviceContext* context) noexcept {};
		//virtual void Update();
		virtual ~Bindable() = default;
	};
}