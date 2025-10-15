

// Model.cpp
#include "ModelLoader.h"

/*
void ModelLoader::LoadModel(const std::string& path, SceneNode* rootNode, UINT attrFlags)
{
	Assimp::Importer importer;
	const aiScene* pModel = importer.ReadFile(path,
		aiProcess_Triangulate | aiProcess_FlipUVs
		| (((attrFlags & ModelLoader::VertexAttrFlags::NORMAL) != 0) ? aiProcess_GenNormals : 0x0)
	);

	//	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	// aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_MakeLeftHanded

	if (!pModel || pModel->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pModel->mRootNode)
	{
		rootNode = nullptr;
		return;
	}

	unsigned int meshesNum = 1; // pModel->mNumMeshes;

	rootNode->verticesNum = 0;
	rootNode->indicesNum = 0;

	rootNode->verticesNum += pModel->mMeshes[0]->mNumVertices;
	rootNode->indicesNum += pModel->mMeshes[0]->mNumFaces * 3;

	rootNode->vertices = (CommonVertex*)calloc(rootNode->verticesNum, sizeof(CommonVertex));
	rootNode->indices = (int*)calloc(rootNode->indicesNum, sizeof(int));

	size_t vertexIdx = 0;
	size_t indexIdx = 0;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0, 1);

	const auto pMesh = pModel->mMeshes[0];

	//std::cout << (((attrFlags & ModelLoader::VertexAttrFlags::NORMAL))) << "\n";
	//std::cout << (((attrFlags & ModelLoader::VertexAttrFlags::NORMAL) != 0) ? aiProcess_GenNormals : 0x0) << "\n";
	for (unsigned i = 0; i < pMesh->mNumVertices; i++)
	{
		(rootNode->vertices)[vertexIdx++] = {
			XMFLOAT3(
			pMesh->mVertices[i].x,
			pMesh->mVertices[i].y,
			pMesh->mVertices[i].z
			) };

		if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
		{
			//std::cout << "aiModel has texture!\n";
			(rootNode->vertices)[vertexIdx - 1].texCoord.x = (float)pMesh->mTextureCoords[0][i].x;
			(rootNode->vertices)[vertexIdx - 1].texCoord.y = (float)pMesh->mTextureCoords[0][i].y;
			//std::cout << (float)pMesh->mTextureCoords[0][i].x << ", " << (float)pMesh->mTextureCoords[0][i].y << "\n";
		}

		if (attrFlags & VertexAttrFlags::NORMAL) {
			(rootNode->vertices)[vertexIdx - 1].normal = {
				XMFLOAT3(
				pMesh->mNormals[i].x,
				pMesh->mNormals[i].y,
				pMesh->mNormals[i].z
				) };
		}
	}
	for (unsigned i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
			
		auto col = XMFLOAT4(distr(gen), distr(gen), distr(gen), 1);

		for (unsigned j = 0; j < face.mNumIndices; j++) {
			(rootNode->indices)[indexIdx++] = face.mIndices[j];
				
			(rootNode->vertices)[face.mIndices[j]].color = col;

		}
	}

	// if (pMesh->mTextureCoords[0] && (attrFlags & VertexAttrFlags::TEXTURE))
	// {
	// 	aiMaterial* material = pModel->mMaterials[pMesh->mMaterialIndex];
	// 	std::vector<Texture> diffuseTextures = rootNode->LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pModel);
	// 	rootNode->textures.insert(rootNode->textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	// }

	return;
}
*/
