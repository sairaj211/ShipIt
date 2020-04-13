#pragma once
#include <string>
#include "Component.h"
/*!
 * \class Controller
 *  \brief Designates if a player is controllable by a player, interfacing with the InputManager
 *
 */
class Controller : public Component
{
public:
	/*!
	 * 
	 * Default Ctor
	 */
	GEAR_API Controller();
	/*!
	 * Default Dtor
	 * 
	 */
	GEAR_API virtual ~Controller();

	/*!
	 * 
	 * Reset all of the values for Controller
	 */
	GEAR_API void Reset();
	/*!
	 * Create a new Controller
	 * 
	 * \return new controller not tied to a gamepad
	 */
	GEAR_API virtual Controller* Create();
	/*!
	 * Update the controller, is empty
	 * 
	 * \param deltaTime
	 */
	GEAR_API void Update(float deltaTime);
	
	/*!
	 * Display debug which is just the playerid
	 * 
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Read all of the features from a json root
	 *
	 * \param root
	 */
	GEAR_API void Serialize(Json::Value root);
	/*!
	 * Write all of the features to a json root
	 *
	 * \param root
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& root, bool a_ignoreDefault = false);
	/*!
	 * Handle an event 
	 * 
	 * \param pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* pEvent);
	/*!
	 * Exposed for python API if a key is pressed
	 * 
	 * \param pressed string defined in Bindings.json
	 * \return whether the key or gamepad input is pressed
	 */
	GEAR_API bool IsPressed(std::string pressed);
	/*!
	 * Exposed for python API if a key is triggered
	 * 
	 * \param trigger string defined in Bindings.json
	 * \return whether the key or gamepad input is triggered
	 */
	GEAR_API bool IsTriggered(std::string trigger);
	/*!
	 * Exposed for python API if a key is released
	 * 
	 * \param release string defined in Bindings.json
	 * \return whether the key or gamepad input is released
	 */
	GEAR_API bool IsReleased(std::string release);
	/*!
	 * Set the PlayerId
	 * 
	 * \param _playerId 0 to 3 for controller input
	 */
	GEAR_API void SetId(int _playerId);
	/*!
	 * 
	 * Get the controller playerid
	 * \return the player id
	 */
	GEAR_API int GetControllerId();

	GEAR_API void ConfirmController(bool aConfirm);
	GEAR_API void Clone(int objID);

private:
	int mPlayerId;
};
