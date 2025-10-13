#pragma once

#include <filesystem>
#include <random>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GraphicsUtils/SceneNode.h"

class ModelLoader
{
public:
    enum VertexAttrFlags : UINT {
        POSITION = 0x1,
        COLOR = 0x2,
        TEXTURE = 0x4,
        NORMAL = 0x8,
    };
    static void LoadModel(const std::string& path, SceneNode* rootNode, UINT attrFlags = VertexAttrFlags::POSITION);
    // void ProcessNode(aiNode* node, const aiScene* scene, SceneNode* parent);

    // void ProcessMesh(aiMesh* mesh, const aiScene* scene, SceneNode* meshNode)
};