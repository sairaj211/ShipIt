#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Model.h"
#include "Mesh.h"
#include "Managers/ShapeGenerator.h"
#include "Skeleton.h"
#include "Utility.h"

Model::Model(std::string a_name) : Resource(a_name)
{
	mpSkeleton = nullptr;
	mpScene = nullptr;
	mGlobalInverse = glm::mat4(1.0f);
	mMinMaxX = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
	mMinMaxY = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
	mMinMaxZ = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::min());
}

Model::~Model()
{
	for (int i = 0; i < mMeshes.size(); ++i)
		delete mMeshes[i];
	mMeshes.clear();

	delete mpSkeleton;
}

void Model::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	if (!ShapeGenerator::GetInstance()->IsShape(mName)) {
		Assimp::Importer importer;
		std::string path = ".\\Resources\\Models\\" + mName + ".fbx";
		const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);

		if (!scene) {
			LOG_ERROR("Failed to load Model file " + mName);
			mLoadFailed = true;
			return;
		}

		mpScene = importer.GetOrphanedScene();

		// Load meshes
		for (unsigned int i = 0; i < mpScene->mNumMeshes; ++i) {
			Mesh* pMesh = SerializeMesh(mpScene->mMeshes[i]);
			mMeshes.push_back(pMesh);
		}

		// Compute Global Inverse
		mGlobalInverse = AIMatrixToGLM(mpScene->mRootNode->mTransformation.Inverse());

		// Load Skeleton Hierarchy
		if (mpSkeleton)
			mpSkeleton->BuildFromRoot(mpScene->mRootNode, nullptr);
	}
	else {
		mMeshes.push_back(ShapeGenerator::GetInstance()->GetShapeMesh(mName));
	}

	mMaxLength = GetMaxLength();

	// Initialize Meshes
	for (Mesh* pMesh : mMeshes) {
		pMesh->NormalizeVertices(mMaxLength);
		pMesh->Initialize();
	}

	// Normalize Animation
	if (mpSkeleton)
		mpSkeleton->Normalize(mMaxLength);

	// Normalize model AABB lengths
	mMinMaxX /= mMaxLength;
	mMinMaxY /= mMaxLength;
	mMinMaxZ /= mMaxLength;
}

Mesh* Model::SerializeMesh(aiMesh* a_meshData)
{
	Mesh* pNewMesh = new Mesh();

	pNewMesh->mMeshName = std::string(a_meshData->mName.C_Str());

	// Load Vertices
	for (unsigned int i = 0; i < a_meshData->mNumVertices; ++i) {
		Vertex vertex;
		vertex.position = glm::vec3(a_meshData->mVertices[i].x, a_meshData->mVertices[i].y, a_meshData->mVertices[i].z);
		vertex.normal = glm::vec3(a_meshData->mNormals[i].x, a_meshData->mNormals[i].y, a_meshData->mNormals[i].z);
		if (a_meshData->mTextureCoords[0])
			vertex.textureCoordinate = glm::vec2(a_meshData->mTextureCoords[0][i].x, a_meshData->mTextureCoords[0][i].y);
		else
			vertex.textureCoordinate = glm::vec2(0.0f);
		if (a_meshData->HasTangentsAndBitangents())
			vertex.tangent = glm::vec3(a_meshData->mTangents[i].x, a_meshData->mTangents[i].y, a_meshData->mTangents[i].z);
		else
			vertex.tangent = glm::vec3(0.0f);
		
		UpdateMinMax(pNewMesh, vertex.position);
		pNewMesh->mVertices.push_back(vertex);
	}

	// Load Indices
	for (unsigned int i = 0; i < a_meshData->mNumFaces; ++i) {
		aiFace* faceData = &a_meshData->mFaces[i];
		for (unsigned int j = 0; j < faceData->mNumIndices; ++j)
			pNewMesh->mIndices.push_back(faceData->mIndices[j]);
	}

	// Initalize Skeleton if a Mesh has Bones
	if (a_meshData->HasBones() && !mpSkeleton)
		mpSkeleton = new Skeleton();

	// Load Bones
	for (unsigned int i = 0; i < a_meshData->mNumBones; ++i) {
		aiBone* pCurrentBone = a_meshData->mBones[i];
		std::string boneName = pCurrentBone->mName.data;

		unsigned int boneIndex = 0;
		if (mpSkeleton->mBoneMap.find(boneName) == mpSkeleton->mBoneMap.end()) {
			// Add new bone to skeleton
			boneIndex = unsigned int(mpSkeleton->mBones.size());
			mpSkeleton->mBoneMap[boneName] = boneIndex;

			Bone bone;
			bone.name = boneName;
			bone.finalTransformation = glm::mat4(1.0f);
			bone.offsetMatrix = AIMatrixToGLM(pCurrentBone->mOffsetMatrix);
			mpSkeleton->mBones.push_back(bone);
		}
		else {
			// Get existing bone index
			boneIndex = mpSkeleton->mBoneMap[boneName];
		}

		// Add Weights and Indices to existing Vertices
		for (unsigned int j = 0; j < pCurrentBone->mNumWeights; ++j) {
			unsigned int vertexIndex = pCurrentBone->mWeights[j].mVertexId;
			pNewMesh->mVertices[vertexIndex].AddBone(boneIndex, pCurrentBone->mWeights[j].mWeight);
		}
	}

	return pNewMesh;
}

float Model::GetMaxLength()
{
	for (Mesh* pMesh : mMeshes) {
		// Check X
		if (pMesh->mMinMaxX.x < mMinMaxX.x)
			mMinMaxX.x = pMesh->mMinMaxX.x;
		if (pMesh->mMinMaxX.y > mMinMaxX.y)
			mMinMaxX.y = pMesh->mMinMaxX.y;

		// Check Y
		if (pMesh->mMinMaxY.x < mMinMaxY.x)
			mMinMaxY.x = pMesh->mMinMaxY.x;
		if (pMesh->mMinMaxY.y > mMinMaxY.y)
			mMinMaxY.y = pMesh->mMinMaxY.y;

		// Check Z
		if (pMesh->mMinMaxZ.x < mMinMaxZ.x)
			mMinMaxZ.x = pMesh->mMinMaxZ.x;
		if (pMesh->mMinMaxZ.y > mMinMaxZ.y)
			mMinMaxZ.y = pMesh->mMinMaxZ.y;
	}

	// Return maximum AABB side length
	return std::max(fabsf(mMinMaxX.x - mMinMaxX.y), std::max(fabsf(mMinMaxY.x - mMinMaxY.y), fabsf(mMinMaxZ.x - mMinMaxZ.y)));
}

void Model::UpdateMinMax(Mesh* a_pMesh, glm::vec3 a_vertex)
{
	// Check X
	if (a_vertex.x < a_pMesh->mMinMaxX.x)
		a_pMesh->mMinMaxX.x = a_vertex.x;
	if (a_vertex.x > a_pMesh->mMinMaxX.y)
		a_pMesh->mMinMaxX.y = a_vertex.x;

	// Check Y
	if (a_vertex.y < a_pMesh->mMinMaxY.x)
		a_pMesh->mMinMaxY.x = a_vertex.y;
	if (a_vertex.y > a_pMesh->mMinMaxY.y)
		a_pMesh->mMinMaxY.y = a_vertex.y;

	// Check Z
	if (a_vertex.z < a_pMesh->mMinMaxZ.x)
		a_pMesh->mMinMaxZ.x = a_vertex.z;
	if (a_vertex.z > a_pMesh->mMinMaxZ.y)
		a_pMesh->mMinMaxZ.y = a_vertex.z;
}