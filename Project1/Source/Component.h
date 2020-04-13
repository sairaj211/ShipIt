#pragma once
#include "Core.h"
class Event;

/*!
 * Base Component that defines the functions
 * and member data expected in each Component
 * 
 */
class Component
{
public:
	/*!
	 * Component Overloaded Constructor
	 * 
	 * \param a_type The type of Component being created
	 */
	GEAR_API Component(std::string a_type);
	/*!
	 * Component Default Destructor
	 *
	 */
	GEAR_API virtual ~Component();
	/*!
	 * Resets the Component to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Initializes the Component with information
	 * available after its creation
	 *
	 */
	GEAR_API virtual void Init() {};
	/*!
	 * Creates a pointer to a new Component
	 *
	 * \return A pointer to a new Component
	 */
	GEAR_API virtual Component* Create() = 0;
	/*!
	 * Updates the values of the Component
	 *
	 * \param a_deltaTime The current difference in time between frames
	 */
	GEAR_API virtual void Update(float a_deltaTime) = 0;
	/*!
	 * Display the members of the Component so that they 
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API virtual void DisplayDebug() = 0;
	/*!
	 * Initializes the Component with the data
	 * stored in the given Json data
	 * 
	 * \param a_root The root of the Json file that contains the Component data
	 */
	GEAR_API virtual void Serialize(Json::Value a_root) {}
	/*!
	 * Saves the Component data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that Component data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API virtual void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false) {}
	/*!
	 * Reacts to the given Event
	 * 
	 * \param a_pEvent The Event that the Component is to react to
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent) = 0;
	/*!
	 * Updates the values of the component
	 * when a parent Entity is set
	 *
	 */
	GEAR_API virtual void OnSetParent() {}
	/*!
	 * Updates the values of the component
	 * when a parent Entity is removed
	 *
	 */
	GEAR_API virtual void OnRemoveParent() {}
	/*!
	 * Sets the type of the Component
	 * 
	 * \param a_type The new type of the Component
	 */
	GEAR_API void SetType(std::string a_type);
	/*!
	 * Gets the type of the Component
	 *
	 * \return Type of the Component
	 */
	GEAR_API std::string GetType();
	/*!
	 * Sets the ID of the Entity that owns
	 * this Component
	 * 
	 * \param a_ownerID The ID of the Entity that owns this Component
	 */
	GEAR_API void SetOwnerID(int a_ownerID);
	/*!
	 * Gets the ID of the Entity that owns
	 * this Component
	 * 
	 * \return ID of the Entity that owns this Component
	 */
	GEAR_API int GetOwnerID();
	/*!
	 * Gets the flag that specifies if the Component is paused
	 * 
	 * \return Flag that is true if the Component is paused, false otherwise
	 */
	GEAR_API bool IsPaused();
	/*!
	 * Sets the Component's Dirty flag to true
	 *
	 */
	GEAR_API virtual void SetDirty(int a_dirtyBits = ~0);
	/*!
	 * Gets a flag indicating whether the
	 * specified bit is dirty
	 *
	 * \param a_dirtyBits The bits being checked for dirty-ness
	 * \return Flag that is true if the specified bits are dirty, false otherwise
	 */
	GEAR_API bool IsDirty(int a_dirtyBits);

	GEAR_API virtual void Clone(int objID) = 0;

protected:
	std::string mType; /*!< Type of the Component */
	int mOwnerID; /*!< ID of the Entity that owns this Component */
	bool mPaused; /*!< Flag that specifies if the Component is paused */
	int mDirty; /*!< Flag that specifies whether the state of the Component has been changed */
};

