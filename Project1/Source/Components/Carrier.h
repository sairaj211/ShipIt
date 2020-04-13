#pragma once

#include "Component.h"

/*!
 * Manages a list of items that are
 * being "carried" by the Entity
 *
 */
class Carrier : public Component
{
public:
	/*!
	 * Carrier Default Constructor
	 *
	 */
	GEAR_API Carrier();
	/*!
	 * Carrier Default Destructor
	 *
	 */
	GEAR_API virtual ~Carrier();
	/*!
	 * Resets the Carrier to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Initializes the list of items with the
	 * starting items
	 *
	 */
	GEAR_API virtual void Init();
	/*!
	 * Creates a pointer to a new Carrier
	 *
	 * \return A pointer to a new Carrier
	 */
	GEAR_API Carrier* Create();
	/*!
	 * Literally Nothing
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Carrier so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the Carrier with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Carrier data
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the Carrier data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Carrier data is being saved to
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
	 * Add the given Entity ID to the list
	 * of items
	 *
	 * \param a_itemID The Entity ID to be added to the list of items
	 */
	GEAR_API void AddItem(int a_itemID);
	/*!
	 * Remove the given Entity ID from the list
	 * of items
	 *
	 * \param a_itemID The Entity ID to be removed to the list of items
	 */
	GEAR_API void RemoveItem(int a_itemID);
	/*!
	 * Remove the Entity IDs whose type matches
	 * the specified type from the list of items
	 *
	 * \param a_type The type of the Entities being removed
	 */
	GEAR_API void RemoveItems(std::string a_type);
	/*!
	 * Removes all Entity IDs from the list
	 * of items
	 *
	 */
	GEAR_API void Void();
	/*!
	 * Get a flag that indicates whether the
	 * given Entity ID is in the list of items
	 *
	 * \param a_itemID The ID of the Entity that is being checked if it exists in the list of items
	 * \return A flag that indicates whether the given Entity ID is in the list of items
	 */
	GEAR_API bool IsItem(int a_itemID);
	/*!
	 * Get a list of items in the
	 * Carrier
	 *
	 * \return A list of items in the Carrier
	 */
	GEAR_API std::vector<int> GetItems();
	/*!
	 * Get a list of items whose type matches
	 * the specified type
	 *
	 * \param a_type The type of the items being returned
	 * \return A list of items whose type matches the specified type
	 */
	GEAR_API std::vector<int> GetItems(std::string a_type);

	GEAR_API void Clone(int objID);

private:
	std::vector<int> mItems;
	std::vector<std::pair<int, int>> mStartingItemMap;
};

