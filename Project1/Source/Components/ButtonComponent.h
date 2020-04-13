#pragma once
#include "Component.h"
class Collider;
class Transform;

/*!
 * \class ButtonComponent
 *  \brief manages whether or not this object is pressed byt the player
 *
 */
class ButtonComponent :public Component
{
public:
	/*!
	 * Default Ctor
	 *
	 */
	GEAR_API ButtonComponent();
	/*!
	 * Default Dtor
	 * 
	 */
	GEAR_API ~ButtonComponent();
	/*!
	 * Create a new ButtonComponent 
	 * 
	 * \return new ButtonComponent
	 */
	GEAR_API ButtonComponent *Create();
	/*!
	 * Update if the button is pressed or not,
	 * will generate a new contact
	 * 
	 * \param aDt
	 */
	GEAR_API void Update(float aDt);
	/*!
	 *  Display debug info, simply press or unpress the button
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the component gathering any dependencies
	 *
	 * \return
	 */
	GEAR_API virtual void Init();
	/*!
	 * Reset all values to defaul
	 *
	 */
	GEAR_API void SelfReset();
	/*!
	 * Load all of the features from a json root
	 *
	 * \param root to read from
	 */
	GEAR_API void Serialize(Json::Value);
	/*!
	 * Write all of the features from a json root
	 * 
	 * \param root to write to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * 
	 * \return whether of not the button is contacted with a player
	 */
	GEAR_API bool IsPressed();
	/*!
	 *
	 * \return whether of not the button is contacted with a player
	 */
	GEAR_API bool IsTriggered();
	/*!
	 *
	 * \return whether of not the button is contacted with a player
	 */
	GEAR_API bool IsReleased();

	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Reset();

	GEAR_API void Clone(int objID);

	bool mIsPressed; /*!< If the button has been pressed*/
	bool mIsTriggered; /*!< If the button has been pressed*/
	bool mIsReleased; /*!< If the button has been pressed*/
	int mPlayerId; /*!< Player Id for reference*/
private:
	bool mIMGUIHOLD; /*!< Hold down the button, only used by IMGUI*/
	Collider* mpPlayerCollider;
	Collider* mpMyCollider;
	Transform* mpPlayerTransform;
	Transform* mpMyTransform;
};

