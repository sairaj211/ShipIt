#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Skeleton.h"
#include "Utility.h"

Skeleton::Skeleton()
{

}

Skeleton::~Skeleton()
{

}

void Skeleton::BuildFromRoot(aiNode* a_pNode, Bone* a_pParent)
{
	std::string nodeName = a_pNode->mName.data;
	glm::mat4 nodeTranformaiton = AIMatrixToGLM(a_pNode->mTransformation);
	Bone* pBone = nullptr;

	if (mBoneMap.find(nodeName) != mBoneMap.end()) {
		pBone = &mBones[mBoneMap[nodeName]];
		pBone->pParent = a_pParent;
		pBone->bindTransformation = nodeTranformaiton;
		if (!a_pParent)
			mpSkeletonRoot = pBone;
		else
			a_pParent->children.push_back(pBone);
	}

	// Build child Bones
	for (unsigned int i = 0; i < a_pNode->mNumChildren; ++i)
		BuildFromRoot(a_pNode->mChildren[i], pBone);
}

void Skeleton::Normalize(float a_maxLength)
{
	NormalizeTraversal(mpSkeletonRoot, a_maxLength);
}

void Skeleton::NormalizeTraversal(Bone* a_pBone, float a_maxLength)
{
	a_pBone->finalTransformation[3][0] /= a_maxLength;
	a_pBone->finalTransformation[3][1] /= a_maxLength;
	a_pBone->finalTransformation[3][2] /= a_maxLength;

	a_pBone->bindTransformation[3][0] /= a_maxLength;
	a_pBone->bindTransformation[3][1] /= a_maxLength;
	a_pBone->bindTransformation[3][2] /= a_maxLength;

	a_pBone->offsetMatrix[3][0] /= a_maxLength;
	a_pBone->offsetMatrix[3][1] /= a_maxLength;
	a_pBone->offsetMatrix[3][2] /= a_maxLength;

	// Normalize child Bones
	for (unsigned int i = 0; i < a_pBone->children.size(); ++i)
		NormalizeTraversal(a_pBone->children[i], a_maxLength);
}