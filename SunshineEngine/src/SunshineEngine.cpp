// Engine.cpp (compiled into Engine.dll)
#include "SunshineEngineAPI.h"
// #include <EASTL/vector.h>
// #include <EASTL/string.h>

// Internal type, never exposed
struct Mesh {
    /*
    eastl::vector<Vec3> vertices;
    eastl::string name;
    */
};

// Load a mesh (dummy for example)
MeshHandle Engine_LoadMesh(const char* path) {
    /*
    Mesh* mesh = new Mesh();
    mesh->name = eastl::string(path);
    
    // Dummy triangle
    mesh->vertices.push_back({0, 0, 0});
    mesh->vertices.push_back({1, 0, 0});
    mesh->vertices.push_back({0, 1, 0});
    
    return MeshHandle{mesh};
    */
    return MeshHandle();
}

Float3ArrayView Engine_GetMeshVertices(MeshHandle handle) {
    /*
    Mesh* mesh = static_cast<Mesh*>(handle.ptr);
    return Float3ArrayView{ mesh->vertices.data(), mesh->vertices.size() };
    */
    return Float3ArrayView();
}

void Engine_ReleaseMesh(MeshHandle handle) {
    /*
    Mesh* mesh = static_cast<Mesh*>(handle.ptr);
    delete mesh;
    */
}
