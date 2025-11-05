#include "Graphics/Lighting/SkyBox.h"

namespace SE_G {
	SkyBox::SkyBox(SkyBoxData initData)
	{
		m_lightData = eastl::make_shared<SkyBoxData>(initData);
	}
}