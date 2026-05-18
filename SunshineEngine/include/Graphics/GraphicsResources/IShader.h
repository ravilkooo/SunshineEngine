#pragma once

#include <Graphics/Bindable/Bindable.h>
#include <ResourceManager/IResource.h>

class IShader : public IResource
{
private:
    ResourceGUID m_GUID = 0;
    size_t m_MemorySize = 0;
public:
	virtual SE_G::Bind::PipelineStage GetPipelineStage() = 0;

    // Inherited via IResource
	SunshineResource::ResourceType GetType() const override { return SunshineResource::ResourceType::SHADER; };
    ResourceGUID GetGUID() const override { return m_GUID; }
    size_t GetSizeInMemory() const override { return m_MemorySize; }
};