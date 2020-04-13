#pragma once

#include "Component.h"

/*!
 * Manages the Block dependent state
 * variables
 *
 */
class Block : public Component
{
public:
	/*!
	 * Block Default Constructor
	 *
	 */
	GEAR_API Block();
	/*!
	 * Block Default Destructor
	 *
	 */
	GEAR_API virtual ~Block();
	/*!
	 * Resets the Block to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new Block
	 *
	 * \return A pointer to a new Block
	 */
	GEAR_API Block* Create();
	/*!
	 * Nothing
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Block so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the Block with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Block data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the Block data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Block data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 * Gets a flag indicating whether the block accepts
	 * input
	 *
	 * \return A flag indicating whether the block accepts input
	 */
	GEAR_API bool CanInput();
	GEAR_API void Clone(int objID) {}

private:
	bool mCanInput;
};