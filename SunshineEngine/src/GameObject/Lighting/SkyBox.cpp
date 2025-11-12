#include "GameObject/Lighting/SkyBox.h"

SkyBox::SkyBox(SE_G::SkyBoxData initData)
{
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(initData);
}

SkyBox_Info::SkyBox_Info(SE_G::SkyBoxData initData)
{
	m_lightData = eastl::make_shared<SE_G::SkyBoxData>(initData);
	m_name = "SkyBox";
	m_group = GameObjectGroup::Lighting;
}