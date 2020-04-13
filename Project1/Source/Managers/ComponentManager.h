#pragma once

#define COMPONENT_MANAGER ComponentManager::GetInstance()

class Component;
class Entity;

/*!
 * Creates, updates, and deletes
 * Components at start up, or during
 * runtime
 * 
 */
class ComponentManager
{
public:
	/*!
	 * Get the ComponentManager Singleton reference
	 * 
	 * \return The ComponentManager reference
	 */
	GEAR_API static ComponentManager* GetInstance();
	/*!
	 * Update the Components in the list of valid Components
	 * 
	 * \param a_deltaTime The time between frames
	 * \param a_ignoreEnabled A flag indicating whether or not to only update enabled Components
	 */
	GEAR_API void Update(float a_deltaTime, bool a_ignoreEnabled = false);
	/*!
	 * Add a Component of the specified type to
	 * the Entity that matches the given Entity
	 * ID
	 * 
	 * \param a_entityID The ID of the Entity that the Component is being added to
	 * \param a_type The type of the Component that is being added to the specified Entity
	 * \return The Component that was added to the specified Entity
	 */
	GEAR_API Component* AddComponent(int a_entityID, std::string a_type);
	/*!
	 * Add a Component of the specified type at
	 * runtime to the Entity that matches the given 
	 * Entity ID
	 * 
	 * \param a_entityID The ID of the Entity that the Component is being added to
	 * \param a_type The type of the Component that is being added to the specified Entity
	 * \return The Component that was added to the specified Entity
	 */
	GEAR_API Component* AddRuntimeComponent(int a_entityID, std::string a_type);
	/*!
	 * Removes the Component of the specified type
	 * from the Entity whose ID matches the given
	 * ID
	 * 
	 * \param a_entityID The ID of the Entity that the Component is being removed from
	 * \param a_type The type of the Component that is being removed
	 */
	template<class T>
	void RemoveComponent(int a_entityID, std::string a_type)
	{
		T* pDeleteComponent = GetComponent<T>(a_entityID, a_type);

		// Check if component to be deleted exists
		if (pDeleteComponent) {
			// Add the specified component to the delete list
			mComponentDeleteList.push_back(pDeleteComponent);
		}
	}
	/*!
	 * Removes all the Components from the Entity
	 * whose ID matches the given ID
	 * 
	 * \param a_entityID The ID of the Entity whose Components are being removed
	 */
	GEAR_API void ClearComponents(int a_entityID);
	/*!
	 * Gets the Components of the specified type
	 * from the Entity whose ID matches the
	 * the given ID
	 * 
	 * \param a_entityID The ID of the Entity whose Component will be returned
	 * \param a_type The type of the Component being requested
	 * \param a_ignoreEnabled A flag indicating whether the requested Component must be enabled or not
	 * \return The Component of the specified type from the Entity with the given ID
	 */
	template<class T>
    T* GetComponent(int a_entityID, const std::string& a_type, bool a_ignoreEnabled = false)
	{
		// Get the list of the specified entity's components
		std::vector<Component*> entityComponents = mEnabledEntityComponentMap[a_entityID];
		
		// Search the enabled component list for a component with matching type
		for (Component* pComponent : entityComponents)
			if (pComponent->GetType() == a_type)
				return dynamic_cast<T*>(pComponent);

		entityComponents = mDisabledEntityComponentMap[a_entityID];

		// Search the disabled component list for a component with matching type
		if (a_ignoreEnabled)
			for (Component* pComponent : entityComponents)
				if (pComponent->GetType() == a_type)
					return dynamic_cast<T*>(pComponent);
		
		return nullptr;
	}
	/*!
	 * Gets the runtime Component of the specified
	 * type from the Entity whose ID matches the
	 * the given ID
	 *
	 * \param a_entityID The ID of the Entity whose Component will be returned
	 * \param a_type The type of the Component being requested
	 * \return The Component of the specified type from the Entity with the given ID
	 */
	template<class T>
	T* GetRuntimeComponent(int a_entityID, const std::string& a_type)
	{
		// Search the runtime component list for a component with matching type and owner ID
		for (Component* pComponent : mComponentCreateList)
			if (pComponent->GetType() == a_type && pComponent->GetOwnerID() == a_entityID)
				return dynamic_cast<T*>(pComponent);

		return nullptr;
	}
	/*!
	 * Get a vector of valid Components of
	 * the specified type
	 * 
	 * \param a_type The type of Components being returned 
	 * \param a_ignoreEnabled A flag indicating whether the requested Components must be enabled or not
	 * \return A vector of Components with the specified type
	 */
	GEAR_API std::vector<Component*> GetComponents(std::string a_type, bool a_ignoreEnabled = false);
	/*!
	 * Get a vector of valid Components whose
	 * corresponding Entity has the given ID
	 * 
	 * \param a_entityID The ID of the Entity whose Components are being returned
	 * \param a_ignoreEnabled A flag indicating whether the requested Components must be enabled or not
	 * \return A vector of the valid Components from the specified Entity
	 */
	GEAR_API std::vector<Component*> GetComponents(int a_entityID, bool a_ignoreEnabled = false);
	/*!
	 * Get a vector of valid runtime Components 
	 * whose corresponding Entity has the given 
	 * ID
	 *
	 * \param a_entityID The ID of the Entity whose Components are being returned
	 * \return A vector of the valid Components from the specified Entity
	 */
	GEAR_API std::vector<Component*> GetRuntimeComponents(int a_entityID);
	/*!
	 * Get a vector of all the valid runtime Components
	 *
	 * \return The vector of all the valid runtie Components
	 */
	GEAR_API std::vector<Component*> GetAllRuntimeComponents();
	/*!
	 * Get a vector of all the valid Components
	 * 
	 * \param a_ignoreEnabled A flag indicating whether the requested Components must be enabled or not
	 * \return The vector of all the valid Components
	 */
	GEAR_API std::vector<Component*> GetAllComponents(bool a_ignoreEnabled = false);
	/*!
	 * Update the Component maps so that they
	 * represent the now enabled/disabled 
	 * Entity
	 *
	 * \param a_entityID The ID of the Entity whose Components are being enabled/disabled
	 * \param a_isEnabled A flag representing whether the specified entity is enabled or disabled
	 */
	GEAR_API void SetComponentsEnabled(int a_entityID, bool a_isEnabled);
	/*!
	 * Get the total number of component types
	 *
	 * \param a_ignoreEnabled A flag indicating whether the requested number of Component types should include disabled Components
	 * \return The number of component types
	 */
	GEAR_API unsigned int GetNumTypes(bool a_ignoreEnabled = false);
	/*!
	 * Adds the runtime Components and deletes the delete Components 
	 * to/from their corresponding Entites 
	 * 
	 */
	GEAR_API void ClearRuntimeLists();
	/*!
	 * Deletes teh ComponentManager Singleton
	 * 
	 */
	GEAR_API void Cleanup();

private:
	GEAR_API ComponentManager();
	GEAR_API ~ComponentManager();
	GEAR_API Component* InitializeComponent(Entity* a_pEntityOwner, std::string a_type);

private:
	static ComponentManager* mInstance;
	std::unordered_map<int, std::vector<Component*>> mEnabledEntityComponentMap;
	std::unordered_map<int, std::vector<Component*>> mDisabledEntityComponentMap;
	std::unordered_map<std::string, std::vector<Component*>> mEnabledComponentMap;
	std::unordered_map<std::string, std::vector<Component*>> mDisabledComponentMap; // May need to order components and entities so parents are first
	std::vector<Component*> mComponentCreateList;
	std::vector<Component*> mComponentDeleteList;
};
