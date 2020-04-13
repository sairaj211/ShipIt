#pragma once
#include "Managers/ComponentManager.h"
#include "Managers/EntityManager.h"
#include "Component.h"

/*!
 * A game object used to couple components
 * together
 * 
 */
class Entity
{
public:
	/*!
	 * Entity Default Constructor
	 * 
	 */
	GEAR_API Entity();
	/*!
	 * Entity Default Destructor
	 * 
	 */
	GEAR_API ~Entity();
	/*!
	 * Resets the Entity to its defualt state
	 * 
	 */
	GEAR_API void Reset();
	/*!
	 * Sets the type of the Entity
	 *
	 * \param a_type of the Entity
	 */
	GEAR_API void SetType(std::string a_type);
	/*!
	 * Gets the type of the Entity
	 * 
	 * \return Type of the Entity
	 */
	GEAR_API std::string GetType();
	/*!
	 * Sets the ID of the Entity
	 * 
	 * \param a_id The new ID for the Entity
	 */
	GEAR_API void SetID(int a_id);
	/*!
	 * Gets the ID of the Entity
	 * 
	 * \return ID of the Entity
	 */
	GEAR_API int GetID();
	/*!
	 * Sets the data file that the Entity
	 * was initialized from
	 * 
	 * \param a_fileName The file that the Entity was initialized from
	 */
	GEAR_API void SetDataFile(std::string a_fileName);
	/*!
	 * Gets the data file that the Entity
	 * was initialized from
	 * 
	 * \return The data file that the Entity was initialized from
	 */
	GEAR_API std::string GetDataFile();
	/*!
	 * Toggles the flag that indicates whether the Entity and its
	 * Components should be used/updated
	 *
	 * \param a_enable A flag indicating whether the Entity is being enabled or not
	 */
	GEAR_API void SetEnabled(bool a_enable);
	/*!
	 * Toggles the flag that indicates whether the Entity, its children, and
	 * their Components should be used/updated
	 *
	 * \param a_enable A flag indicating whether the Entity is being enabled or not
	 */
	GEAR_API void SetAllEnabled(bool a_enable);
	/*!
	 * Gets the flag that indicates whether the Entity and its
	 * Components should be used/updated
	 *
	 * \return The flag is true if the Entity and its Components should be used/updated, false otherwise
	 */
	GEAR_API bool IsEnabled();
	/*!
	 * Sets the name of the Entity
	 *
	 * \param a_name The new name for the Entity
	 */
	GEAR_API void SetName(std::string a_name);
	/*!
	 * Gets the name of the Entity
	 *
	 * \return Name of the Entity
	 */
	GEAR_API std::string GetName();
	/*!
	 * Gets the ID of the Entity's parent Entity
	 *
	 * \return The ID of the Entity's parent Entity
	 */
	GEAR_API int GetParentID();
	/*!
	 * Sets the ID of the Entity's parent Entity
	 *
	 * \param a_parentID The new ID for the Entity's parent Entity
	 */
	GEAR_API void SetParentID(int a_parentID);
	/*!
	 * Gets the list of the Entity's Child Entity IDs
	 *
	 * \return The list of the Entity's Child Entity IDs
	 */
	GEAR_API std::vector<int> GetChildIDs();
	/*!
	 * Gets the list of the Entity's Child Entity IDs
	 * whose type matches the specified string
	 *
	 * \param a_type The type of the Entities whose IDs are being returned
	 * \return The list of the Entity's Child Entity IDs whose type matches the specified type
	 */
	GEAR_API std::vector<int> GetChildIDs(std::string a_type);
	/*!
	 * Initializes the Entity with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Entity data
	 * \param a_isRuntime A flag indicating whether or not the Entity is being serialized during runtime
	 */
	GEAR_API void Serialize(Json::Value a_root, bool a_isRuntime);
	/*!
	 * Saves the Entity data into the given Json
	 * root
	 * 
	 * \param a_root The root of the Json file that Entity data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Set the specified components of the child
	 * Entities to dirty
	 *
	 * \param a_type The type of the Component being set to dirty
	 */
	template<class T>
	void SetChildrenDirty(const std::string& a_type, int a_dirtyBits)
	{
		for (int id : mChildIDs) {
			T* pComponent = COMPONENT_MANAGER->GetComponent<T>(id, a_type, true);
			if (!pComponent)
				pComponent = COMPONENT_MANAGER->GetRuntimeComponent<T>(id, a_type);
			if (pComponent) {
				if (!pComponent->IsDirty(a_dirtyBits)) {
					pComponent->SetDirty(a_dirtyBits);
					Entity* pChildEntity = ENTITY_MANAGER->GetEntity(id, true);
					if (!pChildEntity)
						pChildEntity = ENTITY_MANAGER->GetRuntimeEntity(id);
					pChildEntity->SetChildrenDirty<T>(a_type, a_dirtyBits);
				}
			}
		}
	}

private:
	int mID; /*!< ID of the Entity */
	std::string mType; /*!< Type of the Entity */
	std::string mDataFile; /*!< Data file that the Entity was initialized from */
	bool mIsEnabled; /*!< Flag that indicates whether the Entity and its Components should be used/updated */
	std::string mName; /*!< Name of the Entity instance */
	int mParentID; /*!< ID of the Entity's parent Entity */
	std::vector<int> mChildIDs; /*!< IDs of the Entity's child Entities */
};