#pragma once
#include "Resource.h"

/*!
 * A Struct defining the attributes of the
 * scale keys for each keyframe
 *
 */
struct ScaleKey
{
	float time; /*!< Time of the keyframe */
	glm::vec3 scale; /*!< Scale value of the keyframe */

	/*!
	 * ScaleKey Default Constructor
	 *
	 */
	ScaleKey();
};

/*!
 * A Struct defining the attributes of the
 * rotate keys for each keyframe
 *
 */
struct RotateKey
{
	float time; /*!< Time of the keyframe */
	glm::quat rotate; /*!< Rotate value of the keyframe */

	/*!
	 * RotateKey Default Constructor
	 *
	 */
	RotateKey();
};

/*!
 * A Struct defining the attributes of the
 * translate keys for each keyframe
 *
 */
struct TranslateKey
{
	float time; /*!< Time of the keyframe */
	glm::vec3 translate; /*!< Translate value of the keyframe */

	/*!
	 * TranslateKey Default Constructor
	 *
	 */
	TranslateKey();
};

/*!
 * AffineAnimation Resource that defines
 * the details of an Affine Animation
 *
 */
class AffineAnimation : public Resource
{
public:
	/*!
	 * AffineAnimation Overloaded Constructor
	 *
	 * \param a_name Name of the AffineAnimation Resource
	 */
	GEAR_API AffineAnimation(std::string a_name);
	/*!
	 * AffineAnimation Default Destructor
	 *
	 */
	GEAR_API virtual ~AffineAnimation();
	/*!
	 * Load the AffineAnimation from Json
	 *
	 */
	GEAR_API void Load();
	/*!
	 * Find the index of the current scale keyframe
	 * data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current scale keyframe data
	 */
	GEAR_API unsigned int FindScaleKey(float a_animationTime);
	/*!
	 * Find the index of the current rotate keyframe
	 * data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current rotate keyframe data
	 */
	GEAR_API unsigned int FindRotateKey(float a_animationTime);
	/*!
	 * Find the index of the current translate keyframe
	 * data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The index of the current translate keyframe data
	 */
	GEAR_API unsigned int FindTranslateKey(float a_animationTime);
	/*!
	 * Linearly interpolates between the current and the
	 * next scale keyframe data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated scale keyframe data
	 */
	GEAR_API glm::vec3 InterpolateScale(float a_animationTime);
	/*!
	 * Spherically interpolates between the current and the
	 * next rotate keyframe data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated rotate keyframe data
	 */
	GEAR_API glm::quat InterpolateRotate(float a_animationTime);
	/*!
	 * Linearly interpolates between the current and the
	 * next translate keyframe data
	 *
	 * \param a_animationTime The current time of the animation in Ticks
	 * \return The Interpolated translate keyframe data
	 */
	GEAR_API glm::vec3 InterpolateTranslate(float a_animationTime);
public:
	Json::Value mRoot; /*!< Root node of the Json hierarchy for the Affine Animation */
	float mDuration; /*!< Duration of the Affine Animation in Ticks */
	float mTicksPerSecond; /*!< Speed of the Affine Animation in Ticks */
	std::vector<ScaleKey> mScaleKeys; /*!< Vector of scale keyframe data for each frame */
	std::vector<RotateKey> mRotateKeys; /*!< Vector of rotate keyframe data for each frame */
	std::vector<TranslateKey> mTranslateKeys; /*!< Vector of translate keyframe data for each frame */
};