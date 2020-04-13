#pragma once

#include "Event.h"
/*!
 * The EventMangare which sends, 
 * broadcasts events, subscribes for
 * to enities and the SceneManager
 * 
 */

class EventManager
{
public:
	/*!
	 * Broadcast an event to all off 
	 * the active entities and components
	 * 
	 * \param apEvent the event to be broadcast
	 */
	GEAR_API void BroadcastEvent(Event * apEvent);
	/*!
	 * Add a timeed evetn to be updated during the 
	 * update call
	 * 
	 * \param apEvent pointer evetn to be stored, time should be > 0
	 */
	GEAR_API void AddTimedEvent(Event *apEvent);
	/*!
	 * Update the current timed events 
	 * if an event runs out of time it will
	 * be broadcast
	 * 
	 * \param aDt
	 */
	GEAR_API void Update(float aDt);
	/*!Update
	 * Subscribe an enity to a specific event type
	 * 
	 * \param aType string name of the event 
	 * \param aId the entity id 
	 */
	GEAR_API void Subscribe(std::string aType, int aId);
	/*!
	 * Send an event to objects that have registered to recieve
	 * the specific event
	 * 
	 * \param apEvent pointer to event we are broadcasting
	 */
	GEAR_API void BroadCastToSubs(Event *apEvent);
	/*!
	 * Send an event to an enity for processing
	 * 
	 * \param apEvent the pointer to the event we are sending
	 * \param id Entity Id we are sending the event to
	 */
	GEAR_API void SendEvent(Event* apEvent, int id);
	/*!
	 * Initialize the Event manager 
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Call to delete the current instance of event manager and 
	 * clean up all of the events still on the 
	 * timer 
	 * 
	 */
	GEAR_API void CleanUp();

	/*!
	 * 
	 * 
	 * \param apEvent
	 */
	GEAR_API void SendEventToScene(Event* apEvent);

	/*!
	 * This function gets the current instance of 
	 * Event Mangager and if one has not been created
	 * creates a new one in line with the singleton
	 * pattern
	 * 
	 * \return the current instance
	 */
	GEAR_API static EventManager* GetInstance()
	{
		if (mEventManager == nullptr) {
			mEventManager = new EventManager();
		}
		return mEventManager;
	}
	
	GEAR_API void Clean();
private:
	GEAR_API EventManager();
	GEAR_API ~EventManager();
	
private:
	std::list<Event*>mTimedEvents;/*!<    */
	std::unordered_map<std::string, std::list<int> > mSubs;/*!<    */
	std::unordered_set< Event*> mUsedEvents;/*!<    */
	static EventManager* mEventManager;/*!<    */
};

