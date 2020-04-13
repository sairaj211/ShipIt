#pragma once

#define ENTITY_MANAGER EntityManager::GetInstance()

class Entity;

/*!
 * Creates, updates, and deletes
 * Entities at start up, or during
 * runtime
 * 
 */
class EntityManager
{
public:
	/*!
	 * Get the EntityManager Singleton reference
	 * 
	 * \return The EntityManager reference
	 */
	GEAR_API static EntityManager* GetInstance();
	/*!
	 * Get the Entity whose ID matches the input
	 * ID
	 * 
	 * \param a_entityID The ID of the Entity to be returned
	 * \param a_ignoreEnabled A flag indicating whether the requested Entity must be enabled or not
	 * \return The Entity whose ID matches the input ID.  A null pointer will be returned if that entity doesn't exist
	 */
	GEAR_API Entity* GetEntity(int a_entityID, bool a_ignoreEnabled = false);
	/*!
	 * Get the specified Entity from the list of 
	 * Entities that have not yet been added to 
	 * the list of valid Entities
	 * 
	 * \param a_entityID The ID of the Entity to be returned
	 * \return The Entity whose ID matches the input ID.  A null pointer will be returned if that entity doesn't exist
	 */
	GEAR_API Entity* GetRuntimeEntity(int a_entityID);
	/*!
	 * Gets a vector of Entities of the specified
	 * type
	 * 
	 * \param a_type The type of the Entities that should be in the returned vector
	 * \param a_ignoreEnabled A flag indicating whether the requested Entities must be enabled or not
	 * \return A vector of Entities of the specified type
	 */
	GEAR_API std::vector<Entity*> GetEntities(std::string a_type, bool a_ignoreEnabled = false);
	/*!
	 * Creates the Entity from the given file and
	 * adds it to a list of valid Entities
	 * 
	 * \param a_fileName The name of the file that the Entity will be created from
	 * \param a_parentID An Entity ID that corresponds to the new Entity's parent Entity
	 * \return The Entity that was created from the given file
	 */
	GEAR_API Entity* CreateEntity(std::string a_fileName, int a_parentID = -1, std::string a_scene = "");
	/*!
	 * Creates the Entity from the given file and
	 * adds it to a list of runtime Entities
	 * 
	 * \param a_fileName The name of the file that the Entity will be created
	 * \param a_parentID An Entity ID that corresponds to the new Entity's parent Entity
	 * \return The Entity that was created from the given file
	 */
	GEAR_API Entity* CreateRuntimeEntity(std::string a_fileName, int a_parentID = -1, std::string a_scene = "");
	/*!
	 * Adds the Entity corresponding to the given
	 * ID to a list of Entities that should be deleted
	 * 
	 * \param a_entityID The ID of the Entity that will be deleted
	 */
	GEAR_API void DeleteEntity(int a_entityID);
	/*!
	 * Toggles the flag on the specified Entity
	 * that indicates whether the Entity is enabled
	 * or not
	 *
	 * \param a_entityID The ID of the Entity whose Enabled flag will be toggled
	 * \param a_enable A flag indicating whether the Entity is being enabled or not
	 */
	GEAR_API void EntityEnable(int a_entityID, bool a_enable);
	/*!
	 * Adds the runtime Entities to the list of
	 * valid Entities, and deletes the delete 
	 * Entities from the list of valid Entities
	 * 
	 */
	GEAR_API void ClearRuntimeLists();
	/*!
	 * Deletes the EntityManager Singleton
	 * 
	 */
	GEAR_API void Cleanup();
	/*!
	 * Gets the size of the list of valid
	 * Entities
	 * 
	 * \return The size of the list of valid Entities 
	 */
	GEAR_API int ReturnID();

private:
	GEAR_API EntityManager();
	GEAR_API ~EntityManager();
	GEAR_API Entity* InitializeNewEntity(std::string a_fileName);
	GEAR_API void InitializeNewComponents(Entity* a_pNewEntity, bool a_isRuntime);

public:
	std::unordered_map<int, Entity*> mEntityMap; /*!< List of valid Entities mapped to their ID */

private:
	std::unordered_map<std::string, std::vector<Entity*>> mEntityTypeMap;
	std::vector<Entity*> mEntityDeleteList;
	std::vector<Entity*> mEntityCreateList;
	std::vector<std::pair<Entity*, bool>> mEntityEnableList;
	int mMasterID;
	static EntityManager* mInstance;
};