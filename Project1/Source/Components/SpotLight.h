#pragma once
#include "Component.h"

/*!
 * Manages the Entity's Spot Light
 * intensity, range, color, direction,
 * and spot angle.
 *
 */
class SpotLight : public Component
{
public:
	/*!
	 * SpotLight Default Constructor
	 *
	 */
	GEAR_API SpotLight();
	/*!
	 * SpotLight Default Destructor
	 *
	 */
	GEAR_API virtual ~SpotLight();
	/*!
	 * Resets the SpotLight to its
	 * default state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new
	 * SpotLight
	 *
	 * \return A pointer to a new SpotLight
	 */
	GEAR_API SpotLight* Create();
	/*!
	 * Update the SpotLight values
	 * according to the given frame time
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the SpotLight
	 * so that they are modifiable through ImGui
	 *
	 * \param a_root The root of the Json file that contains the SpotLight data
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the SpotLight with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the SpotLight data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the SpotLight data into the
	 * given Json root
	 *
	 * \param a_root The root of the Json file that the SpotLight data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Get the view matrix of the SpotLight
	 *
	 * \return The view matrix of the SpotLight
	 */
	GEAR_API glm::mat4 GetViewMatrix();
	/*!
	 * Get the perspective matrix of the SpotLight
	 *
	 * \return The perspective matrix of the SpotLight
	 */
	GEAR_API glm::mat4 GetPerspectiveMatrix();
	/*!
	 * Get the direction of the Spot Light
	 *
	 * \return The direction of the Spot Light
	 */
	GEAR_API glm::vec3 GetDirection();
	/*!
	 * Handle the SpotLight's response to a given
	 * event
	 *
	 * \param a_pEvent The event to be handled
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);

	GEAR_API void SetColor(glm::vec3 a_Color);

public:
	float mIntensity; /*!< Scalar that represents the intensity of the Entity's spot light */
	float mRange; /*!< Range of the Entity's Spot Light */
	float mSpotAngle; /*!< Angle in degrees that defines the cone of the Entity's Point Light */
	glm::vec3 mColor; /*!< Color of the Entity's Directional Light */
	float mTempIntensity;
};