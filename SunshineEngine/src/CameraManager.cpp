#include "CameraManager.h"

#include <Graphics/Utils/Camera.h>

#include <Utils/DebugUtils.h>

CameraManager::CameraManager()
{
	m_availableCameras = eastl::unordered_map<SE::UUID, eastl::shared_ptr<SE_G::Camera>>();
}

bool CameraManager::HasCameraByUUID(SE::UUID uuid)
{
	return m_availableCameras.contains(uuid);
}

void CameraManager::AddCamera(eastl::shared_ptr<SE_G::Camera> camera)
{
	auto uuid = camera->GetAssignedUUID();
	if (HasCameraByUUID(uuid)) {
		printSunshineMessage((std::string("Camera manager already has this camera: ") + uuid.ToString()).c_str());
		return;
	}
	m_availableCameras[uuid] = camera;
}

eastl::shared_ptr<SE_G::Camera> CameraManager::GetCameraByUUID(SE::UUID uuid)
{
	auto cameraIt = m_availableCameras.find(uuid);
	if (cameraIt == m_availableCameras.end()) {
		printSunshineMessage((std::string("Camera manager doesn't have this camera: ") + uuid.ToString()).c_str());
		return nullptr;
	}
	else
		return cameraIt->second;
}

eastl::shared_ptr<SE_G::Camera> CameraManager::RemoveCameraByUUID(SE::UUID uuid)
{
	auto cameraIt = m_availableCameras.find(uuid);
	if (cameraIt == m_availableCameras.end()) {
		printSunshineMessage((std::string("Camera manager doesn't have this camera: ") + uuid.ToString()).c_str());
		return nullptr;
	}
	else
	{
		eastl::shared_ptr<SE_G::Camera> camera = cameraIt->second;
		m_availableCameras.erase(cameraIt);
		return camera;
	}
}

void CameraManager::Clear() {
	m_availableCameras.clear();
}