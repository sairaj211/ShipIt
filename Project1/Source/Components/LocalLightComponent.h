#pragma once
#include "Component.h"

/*!
 * Manages the Entity's Point Light
 * color, intensity, and radius
 * 
 */
class LocalLightComponent : public Component
{
public:
	/*!
	 * LocalLightComponent Default Constructor
	 * 
	 */
	GEAR_API LocalLightComponent();
	/*!
	 * LocalLightComponent Default Destructor
	 * 
	 */
	GEAR_API virtual ~LocalLightComponent();
	/*!
	 * Resets the LocalLightComponent to
	 * its default state
	 * 
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new
	 * LocalLightComponent
	 * 
	 * \return A pointer to a new LocalLightComponent
	 */
	GEAR_API LocalLightComponent* Create();
	/*!
	 * Update the LocalLightComponent
	 * values according to the given
	 * frame time
	 * 
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the LocalLightComponent
	 * so that they are modifiable through ImGui
	 * 
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the LocalLightComponent with
	 * the data stored in the given Json data
	 * 
	 * \param a_root The root of the Json file that contains the LocalLightComponent data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the LocalLightComponent data into
	 * the given Json root
	 * 
	 * \param a_root The root of the Json file that the LocalLightComponent data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);


	GEAR_API void SetColor(glm::vec3 a_Color);
public:
	float mIntensity; /*!< Scalar that represents the intensity of the Entity's Point Light */
	float mRadius; /*!< Radius of the Entity's Point Light */
	glm::vec3 mLcolor; /*!< Color of the Entity's Point Light */
};