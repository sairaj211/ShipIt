#pragma once

#include "Component.h"

/*!
 * A Struct defining a comparision function
 * that orders Entities according to their
 * distance to the Entity corresponding to
 * the specified ID
 *
 */
struct Closest
{
	glm::vec3 comparePosition; /*!< The position of the Entity which the given Entities will use to compute their distance */

	/*!
	 * Closest Overloaded Constructor
	 *
	 * \param a_compareID The ID of the Entity which the given Entities will use to compute their distance
	 */
	Closest(int a_compareID);
	/*!
	 * Get a flag indicating whether the first Entity is 
	 * closer than the second to the compare Entity
	 *
	 * \param a_entityID0 The ID of the first Entity being compared
	 * \param a_entityID1 The ID of the second Entity being compared
	 * \return A flag indicating whether the first Entity is closer than the second to the compare Entity
	 */
	bool operator()(int a_entityID0, int a_entityID1);
};

/*!
 * Manages/searches for references to a set 
 * of Entities that the Entity is searching
 * for and has found
 *
 */
class AreaSearch : public Component
{
public:
	/*!
	 * AreaSearch Default Constructor
	 *
	 */
	GEAR_API AreaSearch();
	/*!
	 * AreaSearch Default Destructor
	 *
	 */
	GEAR_API virtual ~AreaSearch();
	/*!
	 * Resets the AreaSearch to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new AreaSearch
	 *
	 * \return A pointer to a new AreaSearch
	 */
	GEAR_API AreaSearch* Create();
	/*!
	 * Search for the desired Entities in a
	 * specified area
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the AreaSearch so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the AreaSearch with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the AreaSearch data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the AreaSearch data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the AreaSearch data is being saved to
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
	 * Get a sorted list of all the IDs of the Entities
	 * that meet the search area constraints
	 *
	 * \return A sorted list of all the IDs of the Entities that meet the search area constraints
	 */
	GEAR_API std::vector<int> GetFound();
	/*!
	 * Get a sorted list of all the IDs of the Entities
	 * of the given type that meet the search area 
	 * constraints
	 *
	 * \param a_type The type of the Entities corresponding to the IDs being returned
	 * \return A sorted list of all the IDs of the Entities of the given type that meet the search area constraints
	 */
	GEAR_API std::vector<int> GetFound(std::string a_type);

	GEAR_API void Clone(int objID);
private:
	GEAR_API void SortContacts(std::vector<int>& a_contantIDs);

private:
	std::vector<int> mFoundIDs;
	std::unordered_map<std::string, std::vector<int>> mFoundIDsTypeMap;
	std::string mCompareFunctionName;
	std::vector<std::string> mSearchTypes;
	std::vector<int> mContactIDs;
	std::vector<std::string> mCompareFunctionsNames;
};

