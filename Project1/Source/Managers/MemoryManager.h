#pragma once
#include "Entity.h"
#include "Event.h"
#include "Events/UpdatedScriptEvent.h"
#include "Events/AttackEvent.h"
#include "Events/PauseEvent.h"
#include "Events/ContactEvent.h"
#include "Events/OrphanEvent.h"
#include "Events/ResetEntityEvent.h"
#include "Events/RecipeMatchEvent.h"
#include "Events/PlayerPickupEvent.h"
#include "Events/PlayerDropEvent.h"
#include "Events/ContainerInputEvent.h"
#include "Events/ContainerOutputEvent.h"
#include "Events/ContainerMatchEvent.h"
#include "Events/RequestFilledEvent.h"
#include "Events/RequestMadeEvent.h"
#include "Events/GeneratorOutputEvent.h"
#include "Events/GeneratorInputEvent.h"
#include "Events/InputEvent.h"
#include "Events/SetCellEvent.h"
#include "Events/CellUpdateEvent.h"
#include "Events/CellEjectEvent.h"
#include "Events/CreateGameEvent.h"
#include "Events/InitLevelEvent.h"
#include "Events/UpdateLEvelEvent.h"
#include "Events/UIActionEvent.h"
#include "Events/UpdateSliderPositionEvent.h"
#include "Events/ResetGridEvent.h"
#include "Events/ChangeAnimationStateEvent.h"
#include "Events/HighlightEvent.h"
#include "Events/LevelCompleteEvent.h"
//#include "Events/BuildingEvent.h"

#include "Component.h"
#include "Components/Transform.h"
#include "Components/Body.h"
#include "Components/Collider.h"
#include "Components/ModelComponent.h"
#include "Components/LocalLightComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SpotLight.h"
#include "Components/ScriptComponent.h"
#include "Components/Controller.h"
#include "Components/ParticlesComponent.h"
#include "Components/Player.h"
#include "Components/Animator.h"
#include "Components/Audio.h"
#include "Components/ButtonComponent.h"
#include "Components/Camera.h"
#include "Components/Observer.h"
#include "Components/AreaSearch.h"
#include "Components/Carrier.h"
#include "Components/Processor.h"
#include "Components/Bullet.h"
#include "Components/Enemy.h"
#include "Components/Wall.h"
#include "Components/Grid.h"
#include "Components/UIComponent.h"
#include "Components/GridWaypoint.h"
#include "Components/AffineSequence.h"
#include "Components/Block.h"
#include "Components/Teleport.h"
#include "Components/LevelButton.h"
#include "Components/Building.h"

#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)

#define REGISTER_COMPONENT(Comp) \
{ \
	RegisterComponent<Comp>(STRINGIFY(Comp)); \
} \

#define REGISTER_EVENT(Evnt) \
{ \
	RegisterEvent<Evnt>(STRINGIFY(Evnt)); \
} \

/*!
 * 
 * 
 */
class MemoryManager {

public:
	/*!
	 * Get the Memory Manger sigleto reference
	 * 
	 * \return the MemoryManager reference
	 */
	GEAR_API static MemoryManager* GetInstance();
	/*!
	 * Initialize the MemoryManager , allocate memory pool for all GameObject,
	 * Components, and Events
	 * Default size memory size for each pool is 2048
	 */
	GEAR_API void Init();
	/*!
	 * Delete all the allocate memory for gameObjects, Components and events
	 * 
	 */
	GEAR_API void Cleanup();

	/*!
	 * this method returns memory block for Entity from memory if available
	 * 
	 * \return 
	 */
	GEAR_API Entity* GetEntityFromMemory();
	/*!
	 * this method returns memory block for Component from memory if available
	 * 
	 * \param componentType
	 * \return 
	 */
	GEAR_API Component* GetComponentFromMemory(std::string componentType);
	/*!
	 *   this method returns memory block for Event from memory if available
	 * 
	 * \param _type
	 * \return 
	 */
	GEAR_API Event* GetEventFromMemory(std::string _type);
	/*!
	 * Free the entity and adds it back to EntityMemory map for reuse
	 * 
	 * \param  Entity*
	 */
	GEAR_API void FreeEntity(Entity*);
	/*!
	 * Free the entity and adds it back to ComponentMemory map for reuse
	 * 
	 * \param Component*
	 */
	GEAR_API void FreeComponent(Component*);
	/*!
	 * Free the entity and adds it back to EventMemory map for reuse
	 * 
	 * \param Evemnt*
	 */
	GEAR_API void FreeEvent(Event*);
	/*!
	 * Used to register the component, based on this registration memory map for component is created
	 *
	 * \param (std::string)componentName
	 */
	template <typename T>
	GEAR_API void RegisterComponent(const char* componentName);
	/*!
	 *
	 * Used to register the component, based on this registration memory map for Events is created
	* \param (std::string)_type
	*/
	template <typename T>
	GEAR_API void RegisterEvent(std::string _type);
private:

		GEAR_API MemoryManager();
		GEAR_API ~MemoryManager();
private:
	static MemoryManager* mInstance;/*!<    */
	std::list<Entity*> mEntityMemory;/*!<    */

	std::unordered_map<std::string, Component*> mComponentMap;/*!<    */
	std::unordered_map<std::string, std::list<Component*>> mComponentMemory;/*!<    */

	std::unordered_map<std::string,Event*> mEventMap;/*!<    */
	std::unordered_map<std::string, std::list<Event*>> mEventMemory;/*!<    */

	size_t mNumberofGameObjects;/*!<    */
	size_t mNumberofComponents;/*!<    */
	size_t mNumberofEvents;/*!<    */
};