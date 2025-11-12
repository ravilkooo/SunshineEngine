#include "Graphics/Lighting/SkyBox.h"

namespace SE_G {
	SkyBox::SkyBox(SkyBoxData initData)
	{
		m_lightData = eastl::make_shared<SkyBoxData>(initData);
	}

	SkyBox_Info::SkyBox_Info(SkyBoxData initData)
	{
		m_lightData = eastl::make_shared<SkyBoxData>(initData);
		m_name = "SkyBox";
		m_group = GameObjectGroup::Lighting;
	}
}