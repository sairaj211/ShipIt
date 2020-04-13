#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "AffineAnimation.h"

ScaleKey::ScaleKey()
{
	time = 0.0f;
	scale = glm::vec3(1.0f);
}

RotateKey::RotateKey()
{
	time = 0.0f;
	rotate = glm::quat(glm::vec3(0.0f));
}

TranslateKey::TranslateKey()
{
	time = 0.0f;
	translate = glm::vec3(0.0f);
}

AffineAnimation::AffineAnimation(std::string a_name) : Resource(a_name)
{
	mDuration = 0.0f;
	mTicksPerSecond = 0.0f;
}

AffineAnimation::~AffineAnimation()
{

}

void AffineAnimation::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	// Load Json Text into String
	std::ifstream file(".\\Resources\\AffineAnimations\\" + mName + ".aat");
	std::string fileString;
	std::getline(file, fileString, (char)EOF);

	// Create Json Reader
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();

	// Parse Json String
	std::string errorString;
	if (!reader->parse(fileString.c_str(), fileString.c_str() + fileString.size(), &mRoot, &errorString)) {
		LOG_ERROR("Failed to load AffineAnimation " + mName + ": " + errorString);
		mLoadFailed = true;
		return;
	}

	// If the root is empty, the load has failed
	if (mRoot.isNull()) {
		LOG_ERROR("Failed to load AffineAnimation " + mName + ": AffineAnimation file is empty");
		mLoadFailed = true;
		return;
	}

	// Fill affine animation values
	// Load the animation duration in ticks
	Json::Value dataRoot = mRoot["Duration"];
	if (!dataRoot.isNull())
		mDuration = dataRoot.asFloat();

	// Load the animation ticks per second
	dataRoot = mRoot["TicksPerSecond"];
	if (!dataRoot.isNull())
		mTicksPerSecond = dataRoot.asFloat();

	// Load animation keyframes
	dataRoot = mRoot["Keyframes"];
	if (!dataRoot.isNull()) {
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			Json::Value keyframe = dataRoot[i];

			ScaleKey scaleKey = ScaleKey();
			RotateKey rotateKey = RotateKey();
			TranslateKey translateKey = TranslateKey();

			// Load keyframe time
			Json::Value keyframeData = keyframe["Time"];
			if (!keyframeData.isNull()) {
				float time = keyframeData.asFloat();
				scaleKey.time = time;
				rotateKey.time = time;
				translateKey.time = time;
			}

			// Load keyframe scale
			keyframeData = keyframe["Scale"];
			if (!keyframeData.isNull())
				scaleKey.scale = glm::vec3(keyframeData[0].asFloat(), keyframeData[1].asFloat(), keyframeData[2].asFloat());

			// Load keyframe rotate
			keyframeData = keyframe["Rotate"];
			if (!keyframeData.isNull()) {
				rotateKey.rotate = glm::quat(glm::radians(glm::vec3(keyframeData[0].asFloat(), keyframeData[1].asFloat(), keyframeData[2].asFloat())));
				glm::normalize(rotateKey.rotate);
			}

			// Load keyframe translate
			keyframeData = keyframe["Translate"];
			if (!keyframeData.isNull())
				translateKey.translate = glm::vec3(keyframeData[0].asFloat(), keyframeData[1].asFloat(), keyframeData[2].asFloat());

			// Add keyframes to appropriate lists
			mScaleKeys.push_back(scaleKey);
			mRotateKeys.push_back(rotateKey);
			mTranslateKeys.push_back(translateKey);
		}
	}
}

unsigned int AffineAnimation::FindScaleKey(float a_animationTime)
{
	// Find the current scale key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < mScaleKeys.size(); ++i)
		if (a_animationTime <= mScaleKeys[i + 1].time)
			return i;

	return 0;
}

unsigned int AffineAnimation::FindRotateKey(float a_animationTime)
{
	// Find the current rotation key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < mRotateKeys.size(); ++i)
		if (a_animationTime <= mRotateKeys[i + 1].time)
			return i;

	return 0;
}

unsigned int AffineAnimation::FindTranslateKey(float a_animationTime)
{
	// Find the current translation key index by comparing 
	// the current animation time with each keyframe after 
	// the first.  If the animation time is less than that 
	// keyframe's time, then we have found the current 
	// keyframe index.
	for (unsigned int i = 0; i < mTranslateKeys.size(); ++i)
		if (a_animationTime <= mTranslateKeys[i + 1].time)
			return i;

	return 0;
}

glm::vec3 AffineAnimation::InterpolateScale(float a_animationTime)
{
	glm::vec3 scale(1.0f);
	std::vector<ScaleKey> scaleKeys = mScaleKeys;
	if (scaleKeys.size() == 1) {
		scale = scaleKeys[0].scale;
	}
	else {
		unsigned int index = FindScaleKey(a_animationTime);
		glm::vec3 s0 = scaleKeys[index].scale;
		glm::vec3 s1 = scaleKeys[index + 1].scale;

		float t = (a_animationTime - scaleKeys[index].time) / (scaleKeys[index + 1].time - scaleKeys[index].time);

		// Use Linear Interpolation to interpolate between the current keyframe's scale component and the next
		scale = glm::lerp(s0, s1, t);
	}

	return scale;
}

glm::quat AffineAnimation::InterpolateRotate(float a_animationTime)
{
	glm::quat rotate(glm::vec3(0.0f));
	std::vector<RotateKey> rotateKeys = mRotateKeys;
	if (rotateKeys.size() == 1) {
		rotate = rotateKeys[0].rotate;
	}
	else {
		unsigned int index = FindRotateKey(a_animationTime);
		glm::quat r0 = rotateKeys[index].rotate;
		glm::quat r1 = rotateKeys[index + 1].rotate;

		float t = (a_animationTime - rotateKeys[index].time) / (rotateKeys[index + 1].time - rotateKeys[index].time);

		// Use Spherical Interpolation to interpolate between the current keyframe's rotate component and the next
		rotate = glm::slerp(r0, r1, t);
		glm::normalize(rotate);
	}

	return rotate;
}

glm::vec3 AffineAnimation::InterpolateTranslate(float a_animationTime)
{
	glm::vec3 translate(0.0f);
	std::vector<TranslateKey> translateKeys = mTranslateKeys;
	if (translateKeys.size() == 1) {
		translate = translateKeys[0].translate;
	}
	else {
		unsigned int index = FindTranslateKey(a_animationTime);
		glm::vec3 v0 = translateKeys[index].translate;
		glm::vec3 v1 = translateKeys[index + 1].translate;

		float t = (a_animationTime - translateKeys[index].time) / (translateKeys[index + 1].time - translateKeys[index].time);

		// Use Linear Interpolation to interpolate between the current keyframe's translate component and the next
		translate = glm::lerp(v0, v1, t);
	}

	return translate;
}