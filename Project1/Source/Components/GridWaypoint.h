#pragma once

#include "Component.h"

/*!
 * Manages the GridWaypoint dependent data
 *
 */
class GridWaypoint : public Component
{
public:
	/*!
	 * Grid Default Constructor
	 *
	 */
	GEAR_API GridWaypoint();
	/*!
	 * Grid Default Destructor
	 *
	 */
	GEAR_API virtual ~GridWaypoint();
	/*!
	 * Resets the GridWaypoint to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new GridWaypoint
	 *
	 * \return A pointer to a new GridWaypoint
	 */
	GEAR_API GridWaypoint* Create();
	/*!
	 * Does nothing
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the GridWaypoint so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the GridWaypoint with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the GridWaypoint data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Saves the GridWaypoint data into the given
	 * Json root
	 *
	 * \param root The root of the Json file that the GridWaypoint data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API void HandleEvent(Event* a_pEvent);
	/*!
	 * Clone the GridWaypoint data into the object associated
	 * with the given ID
	 *
	 * \param a_otherID The id of the object being writen into
	 */
	GEAR_API void Clone(int a_otherID);
	/*!
	 * Get the maximum duration of the waypoint
	 *
	 * \return The maximum duration
	 */
	GEAR_API int GetMaxDuration();
	/*!
	 * Get the minimum duration of the waypoint
	 *
	 * \return The minimum duration
	 */
	GEAR_API int GetMinDuration();
	/*!
	 * Get the order of the waypoint
	 *
	 * \return The order
	 */
	GEAR_API int GetOrder();

private:
	int mMaxDuration;
	int mMinDuration;
	int mOrder;
};

