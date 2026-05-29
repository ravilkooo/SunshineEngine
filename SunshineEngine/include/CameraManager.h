#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/shared_ptr.h>
#include <Utils/UUID.h>

namespace SE_G {
	class Camera;
}

class CameraManager
{
	friend class PlayerSettingPanel;
public:
	CameraManager();

	void AddCamera(eastl::shared_ptr<SE_G::Camera> camera);
	bool HasCameraByUUID(SE::UUID uuid);
	eastl::shared_ptr<SE_G::Camera> GetCameraByUUID(SE::UUID uuid);
	void RemoveCameraByUUID(SE::UUID uuid);
	void Clear();

private:
	// Чтобы быстро и последовательно итероваться
	eastl::vector<SE::UUID> m_camerasUUID;
	// Владеет объектами. Нужен чтобы быстро находить по UUID
	eastl::unordered_map<SE::UUID, eastl::shared_ptr<SE_G::Camera>> m_availableCameras;
};

