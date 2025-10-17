#include "Graphics/Lighting/SkyBox.h"

SkyBox::SkyBox(SkyBoxData initData)
{
	skyBoxData = eastl::make_shared<SkyBoxData>(initData);
}
