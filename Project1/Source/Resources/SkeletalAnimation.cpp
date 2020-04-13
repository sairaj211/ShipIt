#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "SkeletalAnimation.h"
#include "Utility.h"

SkeletalAnimation::SkeletalAnimation(std::string a_name) : Resource(a_name)
{
	mDuration = 0.0f;
	mTicksPerSecond = 0.0f;
}

SkeletalAnimation::~SkeletalAnimation()
{

}

void SkeletalAnimation::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	Assimp::Importer importer;
	std::string path = ".\\Resources\\Animations\\" + mName + ".fbx";
	const aiScene* scene = importer.ReadFile(path.c_str(), 0);

	if (!scene) {
		LOG_ERROR("Failed to load SkeletalAnimation file " + mName);
		mLoadFailed = true;
		return;
	}

	if (!scene->HasAnimations()) {
		LOG_ERROR("Failed to load SkeletalAnimation file " + mName + ": No animations in file");
		mLoadFailed = true;
		return;
	}
	
	if (scene->mNumAnimations > 1) {
		LOG_ERROR("Failed to load SkeletalAnimation file " + mName + ": Too many animations in file");
		mLoadFailed = true;
		return;
	}

	// Load Keyframe Data for each Joint
	aiAnimation* pAnimation = scene->mAnimations[0];
	for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i) {
		aiNodeAnim* pCurrentJointNode = pAnimation->mChannels[i];
		std::string jointName = pCurrentJointNode->mNodeName.data;
		
		// Load Scale Keys
		std::vector<ScaleKey> scaleKeys;
		for (unsigned int j = 0; j < pCurrentJointNode->mNumScalingKeys; ++j) {
			ScaleKey scaleKey;
			scaleKey.time = float(pCurrentJointNode->mScalingKeys[j].mTime);
			scaleKey.scale = AIVectorToGLM(pCurrentJointNode->mScalingKeys[j].mValue);

			scaleKeys.push_back(scaleKey);
		}

		// Load Rotate Keys
		std::vector<RotateKey> rotateKeys;
		for (unsigned int j = 0; j < pCurrentJointNode->mNumRotationKeys; ++j) {
			RotateKey rotateKey;
			rotateKey.time = float(pCurrentJointNode->mRotationKeys[j].mTime);
			rotateKey.rotate = AIQuaternionToGLM(pCurrentJointNode->mRotationKeys[j].mValue);

			rotateKeys.push_back(rotateKey);
		}

		// Load Translate Keys
		std::vector<TranslateKey> translateKeys;
		for (unsigned int j = 0; j < pCurrentJointNode->mNumPositionKeys; ++j) {
			TranslateKey translateKey;
			translateKey.time = float(pCurrentJointNode->mPositionKeys[j].mTime);
			translateKey.translate = AIVectorToGLM(pCurrentJointNode->mPositionKeys[j].mValue);

			translateKeys.push_back(translateKey);
		}

		// Set Joint Maps
		mJointScaleMap[jointName] = scaleKeys;
		mJointRotateMap[jointName] = rotateKeys;
		mJointTranslateMap[jointName] = translateKeys;
	}

	// Load Animation Time
	mDuration = float(pAnimation->mDuration);
	mTicksPerSecond = float(pAnimation->mTicksPerSecond);
}

unsigned int SkeletalAnimation::FindScaleKey(std::vector<ScaleKey> a_scaleKeys, float a_animationTime)
{
	// Find the current scale key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < a_scaleKeys.size(); ++i)
		if (a_animationTime < a_scaleKeys[i + 1].time)
			return i;

	return 0;
}

unsigned int SkeletalAnimation::FindRotateKey(std::vector<RotateKey> a_rotateKeys, float a_animationTime)
{
	// Find the current rotation key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < a_rotateKeys.size(); ++i)
		if (a_animationTime < a_rotateKeys[i + 1].time)
			return i;

	return 0;
}

unsigned int SkeletalAnimation::FindTranslateKey(std::vector<TranslateKey> a_translateKeys, float a_animationTime)
{
	// Find the current translation key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < a_translateKeys.size(); ++i)
		if (a_animationTime < a_translateKeys[i + 1].time)
			return i;

	return 0;
}

glm::vec3 SkeletalAnimation::InterpolateScale(std::string a_jointName, float a_animationTime)
{
	glm::vec3 scale(1.0f);
	std::vector<ScaleKey> scaleKeys = mJointScaleMap[a_jointName];
	if (scaleKeys.size() == 1) {
		scale = scaleKeys[0].scale;
	}
	else {
		unsigned int index = FindScaleKey(scaleKeys, a_animationTime);
		glm::vec3 s0 = scaleKeys[index].scale;
		glm::vec3 s1 = scaleKeys[index + 1].scale;

		float t = (a_animationTime - scaleKeys[index].time) / (scaleKeys[index + 1].time - scaleKeys[index].time);

		// Use Linear Interpolation to interpolate between the current keyframe's scale component and the next
		scale = glm::lerp(s0, s1, t);
	}

	return scale;
}

glm::quat SkeletalAnimation::InterpolateRotate(std::string a_jointName, float a_animationTime)
{
	glm::quat rotate(glm::vec3(0.0f));
	std::vector<RotateKey> rotateKeys = mJointRotateMap[a_jointName];
	if (rotateKeys.size() == 1) {
		rotate = rotateKeys[0].rotate;
	}
	else {
		unsigned int index = FindRotateKey(rotateKeys, a_animationTime);
		glm::quat r0 = rotateKeys[index].rotate;
		glm::quat r1 = rotateKeys[index + 1].rotate;

		float t = (a_animationTime - rotateKeys[index].time) / (rotateKeys[index + 1].time - rotateKeys[index].time);

		// Use Spherical Interpolation to interpolate between the current keyframe's rotate component and the next
		rotate = glm::slerp(r0, r1, t);
		glm::normalize(rotate);
	}

	return rotate;
}

glm::vec3 SkeletalAnimation::InterpolateTranslate(std::string a_jointName, float a_animationTime)
{
	glm::vec3 translate(0.0f);
	std::vector<TranslateKey> translateKeys = mJointTranslateMap[a_jointName];
	if (translateKeys.size() == 1) {
		translate = translateKeys[0].translate;
	}
	else {
		unsigned int index = FindTranslateKey(translateKeys, a_animationTime);
		glm::vec3 v0 = translateKeys[index].translate;
		glm::vec3 v1 = translateKeys[index + 1].translate;

		float t = (a_animationTime - translateKeys[index].time) / (translateKeys[index + 1].time - translateKeys[index].time);

		// Use Linear Interpolation to interpolate between the current keyframe's translate component and the next
		translate = glm::lerp(v0, v1, t);
	}

	return translate;
}