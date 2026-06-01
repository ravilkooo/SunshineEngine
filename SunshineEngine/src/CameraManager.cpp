#include "CameraManager.h"

#include <Graphics/Utils/Camera.h>

#include <Utils/DebugUtils.h>

CameraManager::CameraManager()
{
	m_availableCameras = eastl::unordered_map<SE::UUID, eastl::shared_ptr<SE_G::Camera>>();
	m_camerasUUID = eastl::vector<SE::UUID>();
}

bool CameraManager::HasCameraByUUID(SE::UUID uuid)
{
	return m_availableCameras.contains(uuid);
}

void CameraManager::AddCamera(eastl::shared_ptr<SE_G::Camera> camera)
{
	const SE::UUID id = camera->GetAssignedUUID();
	auto [it, inserted] = m_availableCameras.emplace(id, nullptr);
	if (!inserted)
	{
		printSunshineMessage((std::string("Duplicate UUID in CameraManager::AddCamera: ") +
			id.ToString()).c_str());
		return;
	}
	it->second = camera;
	m_camerasUUID.push_back(id);
	return;
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

void CameraManager::RemoveCameraByUUID(SE::UUID uuid)
{
    auto mapIt = m_availableCameras.find(uuid);
    if (mapIt == m_availableCameras.end())
        return;

    // Remove camera
    // Remove from map
    m_availableCameras.erase(mapIt);

    // Remove from cameras vector
    for (size_t i = 0; i < m_camerasUUID.size(); ++i)
    {
        if (m_camerasUUID[i] == uuid)
        {
            eastl::swap(m_camerasUUID[i], m_camerasUUID.back());
            m_camerasUUID.pop_back();
            break;
        }
    }

    return;
}

void CameraManager::Clear() {
	m_availableCameras.clear();
	m_camerasUUID.clear();
}