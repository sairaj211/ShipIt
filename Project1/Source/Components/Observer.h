#pragma once
#include "Component.h"


/*!
 * \class Observer
 *  \brief class to hold game logic key and value pairs
 *  Mainly a script friendly class for ensuring that the game logic is correctly handled.
 *  Most methods are empty for this purpose
 *
 */
class Observer : public Component
{
public:
	GEAR_API Observer();
	GEAR_API ~Observer();

	GEAR_API Observer *Create();
	/*!  Actions Related to the logic in the scene.
	 * \param  deltaTime
	 */
	GEAR_API void Update(float);
	/*! Expose variables for The Editor
	 */
	GEAR_API void DisplayDebug();
	/*! solve Inv Mas,  get references of self transform, solve actPosition and prevPosition
	 */
	GEAR_API virtual void Init();
	/*! Call this to read all of the values of the body
	 * to the json indicated with the input
	 * \param the root for reading the
	 */
	GEAR_API void Serialize(Json::Value);
	/*!  
	 * Call this to save all of the values of the body
	 * to the json indicated with the input
	 * \param the root for writing the component
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	GEAR_API void HandleEvent(Event* a_pEvent);
	GEAR_API void Reset();
	GEAR_API void SetValue(std::string a_key, float a_value);
	GEAR_API float GetValue(std::string a_key);
	GEAR_API void Clone(int objID) {}
public:
	std::unordered_map<std::string, float> mValues;
};

