#include "Graphics/Lighting/SkyBox.h"

SkyBox::SkyBox(SkyBoxData initData)
{
	m_lightData = eastl::make_shared<SkyBoxData>(initData);
}
