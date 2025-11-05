#include "Graphics/Bindable/Topology.h"

namespace SE_G {
	namespace Bind
	{
		Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY type)
			: type(type)
		{
		}

		void Topology::Bind(ID3D11DeviceContext* context) noexcept
		{
			context->IASetPrimitiveTopology(type);
		}
	}
}