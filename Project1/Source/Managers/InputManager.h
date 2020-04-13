#pragma once

#include "../Input/GamePad.h"
#include "Input/Mouse.h"

/*!
 * 
 * 
 */
class InputManager
{
private:
	GEAR_API InputManager() {}
	GEAR_API ~InputManager();

public:
	/*!
	 * 
	 * 
	 * \return 
	 */
	GEAR_API static InputManager* GetInstance();
	/*!
	 * 
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * 
	 * 
	 */
	GEAR_API void Update();
	/*!
	 * 
	 * 
	 */
	GEAR_API void Cleanup();
	
	//KEYBOARD convert to string
	/*!
	 * return a bool if key is pressed
	 *\param (int)KeyScanCode
	 */
	GEAR_API bool IsPressed(unsigned int KeyScanCode); //From SDL_Scancode
	/*!
	 * 
	 * return a bool if key is triggered
	 * \param KeyScanCode
	 * \return 
	 */
	GEAR_API bool IsTriggered(unsigned int KeyScanCode);
	/*!
	 * 
	 * return a bool if key is released
	 * \param KeyScanCode
	 * \return 
	 */
	GEAR_API bool IsReleased(unsigned int KeyScanCode);
	/*!
	 * return a bool if key is pressed based on which player is making is request
	 * \param pressed
	 * \param _playerNum
	 */
	GEAR_API bool IsPressed(std::string pressed,int _playerNum = -1); //From SDL_Scancode
	/*!
	 * return a bool if key is Triggered based on which player is making is request
	 * \param pressed
	 * \param _playerNum
	 * \return 
	 */
	GEAR_API bool IsTriggered(std::string trigger, int _playerNum = -1);
	/*!
	 * 
	 * return a bool if key is released based on which player is making is request
	 * \param pressed
	 * \param _playerNum
	 * \return 
	 */
	GEAR_API bool IsReleased(std::string released, int _playerNum = -1);


	GEAR_API Uint32 GetTime(unsigned int KeyScanCode);
	
	/*!
	 * PollEVENT handler
	 *
	 * \param event
	 */
	GEAR_API void ConfirmGamepad(int aId, bool aConfirm);
	
	/*!
	 * PollEVENT handler
	 * 
	 * \param event
	 */
	GEAR_API void ProcessEvent(SDL_Event *event);

	GEAR_API std::list<int> GetPlayerGamePads();

private:
	static InputManager* mInstance;/*!<    */
	Uint8 mCurrentState[512];/*!<    */
	Uint32 mKeyTime[512];/*!<    */
	Uint8 mPreviousState[512];/*!<    */
	std::unordered_map<int,GamePad*> mGamePads;/*!<    */
	std::unordered_map<int, bool> mCofirmed;/*!<    */
	Mouse * mMouse;/*!<    */
};