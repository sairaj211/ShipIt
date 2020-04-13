#pragma once
#include "Component.h"
#include "Core.h"
class Event;
class Generator : public Component
{
public:
	/*! Constructor
	 */
	GEAR_API Generator();
	GEAR_API ~Generator();
	
	/*!  Return a new instance of the Body
	 * \return  refeerence to a new Body
	 */
	GEAR_API Generator *Create();
	/*!  Actions Related to the Bodies.
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
	/*!  Call this to save all of the values of the body
	 * to the json indicated with the input
	 * \param the root for writing the component
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API void Reset();
	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID) {}
};

