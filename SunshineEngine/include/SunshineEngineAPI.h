// SunshineEngineAPI.h
#pragma once

#ifdef SUNSHINE_ENGINE_EXPORTS
#   define SUNSHINE_ENGINE_API __declspec(dllexport)
#else
#   define SUNSHINE_ENGINE_API __declspec(dllimport)
#endif

#include <cstddef> // for size_t

// POD-friendly math type
struct SUNSHINE_ENGINE_API Vec3 {
    float x, y, z;
};

// POD array view (no allocators, no hidden layout)
struct SUNSHINE_ENGINE_API Float3ArrayView {
    const Vec3* data;
    size_t size;
};

// Opaque handle (Editor/Game never sees internal types)
struct SUNSHINE_ENGINE_API MeshHandle {
    void* ptr;
};

// Public API functions
extern "C" {

    SUNSHINE_ENGINE_API MeshHandle Engine_LoadMesh(const char* path);

    SUNSHINE_ENGINE_API Float3ArrayView Engine_GetMeshVertices(MeshHandle mesh);

    SUNSHINE_ENGINE_API void Engine_ReleaseMesh(MeshHandle mesh);
}
