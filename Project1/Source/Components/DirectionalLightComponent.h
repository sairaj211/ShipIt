#pragma once
#include "Component.h"

/*!
 * Manages the Entity's Direction Light
 * color, intensity, and direction
 * 
 */
class DRLightComponent : public Component
{
public:
	/*!
	 * DRLightComponent Default Constructor
	 * 
	 */
	GEAR_API DRLightComponent();
	/*!
	 * DRLightComponent Default Destructor
	 * 
	 */
	GEAR_API virtual ~DRLightComponent();
	/*!
	 * Resets the DRLightComponent to its
	 * default state
	 * 
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new
	 * DRLightComponent
	 * 
	 * \return A pointer to a new DRLightComponent
	 */
	GEAR_API DRLightComponent* Create();
	/*!
	 * Update the DRLightComponent values
	 * according to the given frame time
	 * 
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * 
	 * 
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Display the members of the DRLightComponent
	 * so that they are modifiable through ImGui
	 * 
	 * \param a_root The root of the Json file that contains the DRLightComponent data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the DRLightComponent data into the
	 * given Json root
	 * 
	 * \param a_root The root of the Json file that the DRLightComponent data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Get the view matrix of the Directional Light
	 *
	 * \return The view matrix of the Directional Light
	 */
	GEAR_API glm::mat4 GetViewMatrix();
	/*!
	 * Get the perspective matrix of the Directional
	 * Light
	 * 
	 * \return The perspective matrix of the Directional Light
	 */
	GEAR_API glm::mat4 GetPerspectiveMatrix();
	/*!
	 * Get the direction of the Directional Light
	 *
	 * \return The direction of the Directional Light
	 */
	GEAR_API glm::vec3 GetDirection();
	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	
	GEAR_API void Clone(int objID);

public:
	float mIntensity; /*!< Scalar that represents the intensity of the Entity's Directional Light */
	glm::vec3 mLcolor; /*!< Color of the Entity's Directional Light */
};