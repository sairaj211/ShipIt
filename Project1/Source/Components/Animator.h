#pragma once

#include "Component.h"

class AffineAnimation;
class SkeletalAnimation;
class Model;
struct Bone;

/*!
 * A Struct defining the attributes of an
 * Affine State in an animation
 *
 */
struct AffineState 
{
	std::string stateName; /*!< Name of the Affine State */
	AffineAnimation* pAffineAnimation; /*!< Affine Animation associated with the Affine State */
	float blendDuration; /*!< Duration of the blending used to get to this Affine State */
	bool returnToDefault; /*!< Flag that indicates whether or not the animation returns to the default Affine State after this Affine State has ended */

	/*!
	 * AffineState Default Constructor
	 *
	 */
	AffineState();
};

/*!
 * A Struct defining the attributes of an
 * Skeletal State in an animation
 *
 */
struct SkeletalState
{
	std::string stateName; /*!< Name of the Skeletal State */
	SkeletalAnimation* pSkeletalAnimation; /*!< Skeletal Animation associated with the Skeletal State */
	float blendDuration; /*!< Duration of the blending used to get to this Skeletal State */
	bool returnToDefault; /*!< Flag that indicates whether or not the animation returns to the default Skeletal State after this Skeletal State has ended */

	/*!
	 * SkeletalState Default Constructor
	 *
	 */
	SkeletalState();
};

/*!
 * Manages the Skeletal and Affine
 * animation states of the Entity
 *
 */
class Animator : public Component 
{
public:
	/*!
	 * Animator Default Constructor
	 *
	 */
	GEAR_API Animator();
	/*!
	 * Animator Default Destructor
	 *
	 */
	GEAR_API virtual ~Animator();
	/*!
	 * Resets the Animator to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer tp a new Animator
	 *
	 * \return A pointer to a new Animator
	 */
	GEAR_API Animator* Create();
	/*!
	 * Update the Skeletal and Affine animations
	 * of the current animation state according to
	 * the given frame time
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Animator so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the Animator with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Animator data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the Animator data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Animator data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Compute and get the transformation matrix that describes
	 * the current state of the Affine Animation
	 *
	 * \return The Transformation that describes the current state of the Affine Animation
	 */
	GEAR_API glm::mat4 GetAffineAnimationMatrix();
	/*!
	 * Compute and get a list of transformations for
	 * each joint in the skeleton described in the given
	 * Model according to the current Skeletal Animation
	 *
	 * \param a_pModel The Model that describes the Skeleton used for the Skeletal Animation
	 * \return A list of transformations for each joint that describe the current state of the Skeletal Animation
	 */
	GEAR_API std::vector<glm::mat4> GetSkeletalTransformations(Model* a_pModel);
	/*!
	 * Change the state of the current Affine
	 * and Skeletal Animation
	 *
	 * \param a_state The name of the state that the animation is being changed to
	 * \return True if the state has changed, false otherwise
	 */
	GEAR_API bool ChangeState(const std::string& a_state);
	/*!
	 * Get the name of the current Skeletal 
	 * State
	 * 
	 * \return The name of the current Skeletal State
	 */
	GEAR_API const char* GetSkellState();
	/*!
	 * Get the name of the current affine
	 * state
	 *
	 * \return The name of the current affine State
	 */
	GEAR_API std::string GetAffineState();
	/*!
	 * Get a list of the available affine states
	 *
	 * \return The list of affine states
	 */
	GEAR_API std::vector<std::string> GetAffineStates();
	/*!
	 * Change the state of the current Affine
	 * Animation
	 *
	 * \param a_state The name of the state that the Affine Animation is being changed to
	 * \return True if the state has changed, false otherwise
	 */
	GEAR_API bool ChangeAffineState(const std::string& a_state);
	/*!
	 * Change the state of the current Skeletal
	 * Animation
	 *
	 * \param a_state The name of the state that the Skeletal Animation is being changed to
	 * \return True if the state has changed, false otherwise
	 */
	GEAR_API bool ChangeSkeletalState(const std::string& a_state);

	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID) {}

private:
	GEAR_API void UpdateAffineAnimation(float a_deltaTime);
	GEAR_API void UpdateSkeletalAnimation(float a_deltaTime);
	GEAR_API void ComputeTransformations(Model* a_pModel);
	GEAR_API void ComputeBlendedTransformations(Model* a_pModel);

public:
	bool mPause; /*!< Flag indicated whether or not the animation time should be updated */
	float mSpeedModifier; /*!< Speed of the animations */

private:
	// Affine Animation Variables
	AffineState* mpCurrentAffineState;
	AffineState* mpNextAffineState;
	AffineState* mpRuntimeAffineState;
	std::unordered_map<std::string, AffineState*> mAffineStateMap;
	float mCurrentAffineTime;
	float mNextAffineTime;
	float mAffineBlendingTime;
	bool mComputeAffineAnimation;
	std::string mDefaultAffineState;

	glm::vec3 prevScale;
	glm::quat prevRotation;
	glm::vec3 prevTranslation;

	glm::vec3 prevNextScale;
	glm::quat prevNextRotation;
	glm::vec3 prevNextTranslation;

	glm::mat4 mAffineAnimationMatrix;

	// Skeletal Animation Variables
	SkeletalState* mpCurrentSkeletalState;
	SkeletalState* mpNextSkeletalState;
	SkeletalState* mpRuntimeSkeletalState;
	std::unordered_map<std::string, SkeletalState*> mSkeletalStateMap;
	float mCurrentSkeletalTime;
	float mNextSkeletalTime;
	float mSkeletalBlendingTime;
	bool mComputeSkeletalAnimation;
	std::string mDefaultSkeletalState;

	std::vector<glm::mat4> mSkeletalTransformations;
};
