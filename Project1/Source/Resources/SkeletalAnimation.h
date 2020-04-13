#pragma once
#include "Resource.h"
#include "AffineAnimation.h"


/*!
 * Skeletal Animation Resource that defines
 * the details of a Skeletal Animation
 */
class SkeletalAnimation : public Resource {
public:
	/*!
	 * SkeletalAnimation Overloaded Constructor
	 *
	 * \param a_name The name of the SkeletalAnimation Resource
	 */
	GEAR_API SkeletalAnimation(std::string a_name);
	/*!
	 * SkeletalAnimation Default Destructor
	 *
	 */
	GEAR_API virtual ~SkeletalAnimation();
	/*!
	 * Load the SkeletalAnimation from an
	 * FBX file
	 *
	 */
	GEAR_API void Load();
	/*!
	 * Find the index of the current scale keyframe
	 * data
	 *
	 * \param a_scaleKeys A vector of ScaleKeys for each keyframe
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current scale keyframe data
	 */
	GEAR_API unsigned int FindScaleKey(std::vector<ScaleKey> a_scaleKeys, float a_animationTime);
	/*!
	 * Find the index of the current rotate keyframe
	 * data
	 *
	 * \param a_rotateKeys A vector of RotateKeys for each keyframe
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current rotate keyframe data
	 */
	GEAR_API unsigned int FindRotateKey(std::vector<RotateKey> a_rotateKeys, float a_animationTime);
	/*!
	 * Find the index of the current translate keyframe
	 * data
	 *
	 * \param a_rotateKeys A vector of TranslateKeys for each keyframe
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current translate keyframe data
	 */
	GEAR_API unsigned int FindTranslateKey(std::vector<TranslateKey> a_translateKeys, float a_animationTime);
	/*!
	 * Linearly interpolates between the current and the
	 * next scale keyframe data 
	 *
	 * \param a_jointName The name of the joint whose data should be interpolated
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated scale keyframe data 
	 */
	GEAR_API glm::vec3 InterpolateScale(std::string a_jointName, float a_animationTime);
	/*!
	 * Spherically interpolates between the current and the
	 * next rotate keyframe data
	 *
	 * \param a_jointName The name of the joint whose data should be interpolated
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated rotate keyframe data
	 */
	GEAR_API glm::quat InterpolateRotate(std::string a_jointName, float a_animationTime);
	/*!
	 * Linearly interpolates between the current and the
	 * next translate keyframe data
	 *
	 * \param a_jointName The name of the joint whose data should be interpolated
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated translate keyframe data
	 */
	GEAR_API glm::vec3 InterpolateTranslate(std::string a_jointName, float a_animationTime);

public:
	float mDuration; /*!< Duration of the Skeletal Animation in Ticks */
	float mTicksPerSecond; /*!< Speed of the Skeletal Animation in Ticks */
	std::unordered_map<std::string, std::vector<ScaleKey>> mJointScaleMap; /*!< Map from joint name to scale keyframe data for each frame */
	std::unordered_map<std::string, std::vector<RotateKey>> mJointRotateMap; /*!< Map from joint name to rotate keyframe data for each frame */
	std::unordered_map<std::string, std::vector<TranslateKey>> mJointTranslateMap; /*!< Map from joint name to translate keyframe data for each frame */
};