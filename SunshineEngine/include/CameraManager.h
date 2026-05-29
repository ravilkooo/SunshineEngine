#pragma once

#include <EASTL/unordered_map.h>
#include <EASTL/shared_ptr.h>
#include <Utils/UUID.h>

namespace SE_G {
	class Camera;
}

class CameraManager
{
public:
	CameraManager();

	void AddCamera(eastl::shared_ptr<SE_G::Camera> camera);
	bool HasCameraByUUID(SE::UUID uuid);
	eastl::shared_ptr<SE_G::Camera> GetCameraByUUID(SE::UUID uuid);
	eastl::shared_ptr<SE_G::Camera> RemoveCameraByUUID(SE::UUID uuid);
	void Clear();

private:
	eastl::unordered_map<SE::UUID, eastl::shared_ptr<SE_G::Camera>> m_availableCameras;
};

