#pragma once
enum PLAYER_COUNT {
	PLAYER_1 = 0,
	PLAYER_2 = 1,
	PLAYER_3 = 2,
	PLAYER_4 = 3,
	MAX_PLAYERS = 4
};

/*!
 * \class Gamepad class updates the state of a given gampad by sdl controller
 * \brief  class for  handling the gamepad inputs
 * 
 */
class GamePad
{
public:
	/*!
	 * Create a gamepad for input
	 * 
	 * \param _controller default nullptr
	 */
	GEAR_API GamePad(SDL_GameController* _controller=nullptr);
	/*!
	 * 
	 *  Delete all relevent data to the game pad
	 */
	GEAR_API ~GamePad();
	/*!
	 * Get the SDL Controller pointer
	 * 
	 * \return SDL Game Controller
	 */
	GEAR_API SDL_GameController* GetController();
	/*!
	 * Set this gampad's controller
	 * 
	 * \param _controller pointer to SDL controller
	 */
	GEAR_API void SetController(SDL_GameController* _controller);
	/*!
	 * Update the controller setting the current state to 
	 * the previous state
	 * 
	 */
	GEAR_API void Update();
	/*!
	 * 
	 * Reset the gamepade
	 */
	GEAR_API void Reset();
	/*!
	 * Check if a button was pressed
	 * 
	 * \param button SDL Button Code
	 * \return if button was activated last frame and this frame
	 */
	GEAR_API bool IsPressed(SDL_GameControllerButton  button);
	/*!
	 * Check if a button was triggered
	 * 
	 * \param button SDL Button Code
	 * \return if button was not activate last frame and was this frame
	 */
	GEAR_API bool IsTriggered(SDL_GameControllerButton  button);
	/*!
	 * Check if a button was released
	 * 
	 * \param button SDL Button Code
	 * \return if button was activated last frame and not this frame
	 */
	GEAR_API bool IsReleased(SDL_GameControllerButton  button);
	/*!
	 * Check if this GamePad is in use and active
	 * 
	 * \return true if a valid controller pointer exists for this Gamepad
	 */
	GEAR_API bool IsActive();
	/*!
	 * Check the Y axis of either stick to see if the desired axis was triggered up
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was not activated up last frame and is this frame
	 */
	GEAR_API bool IsStickTriggeredUp(SDL_GameControllerAxis axis);
	/*!
	 * Check the Y axis of either stick to see if the desired axis was triggered down
	 * 
	 * \param axis SDL Axis either right or left
	 * \return if the stick was not activated down last frame and is this frame
	 */
	GEAR_API bool IsStickTriggeredDown(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was triggered Left
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was not activated left last frame and is this frame
	 */
	GEAR_API bool IsStickTriggeredLeft(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was triggered right
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was not activated right last frame and is this frame
	 */
	GEAR_API bool IsStickTriggeredRight(SDL_GameControllerAxis axis);
	/*!
	 * Check the Y axis of either stick to see if the desired axis was pressed up
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated up last frame and is this frame
	 */
	GEAR_API bool IsStickPressedUp(SDL_GameControllerAxis axis);
	/*!
	 * Check the Y axis of either stick to see if the desired axis was pressed down
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated down last frame and is this frame
	 */
	GEAR_API bool IsStickPressedDown(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was pressed left
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated left last frame and is this frame
	 */
	GEAR_API bool IsStickPressedLeft(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was pressed right
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated right last frame and is this frame
	 */
	GEAR_API bool IsStickPressedRight(SDL_GameControllerAxis axis);
	/*!
	 * Check the Y axis of either stick to see if the desired axis was released from up
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated up last frame and is not this frame
	 */
	GEAR_API bool IsStickReleasedUp(SDL_GameControllerAxis axis);
	/*!
	 * Check the Y axis of either stick to see if the desired axis was released from down
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated down last frame and is not this frame
	 */
	GEAR_API bool IsStickReleasedDown(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was released from left
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated left last frame and is not this frame
	 */
	GEAR_API bool IsStickReleasedLeft(SDL_GameControllerAxis axis);
	/*!
	 * Check the X axis of either stick to see if the desired axis was released from right
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated right last frame and is not this frame
	 */
	GEAR_API bool IsStickReleasedRight(SDL_GameControllerAxis axis);
	/*!
	 * Get the X axis value of either stick to see if the desired axis was released from right
	 *
	 * \param axis SDL Axis either right or left
	 * \return if the stick was activated right last frame and is not this frame
	 */
	GEAR_API float GetAxisValue(SDL_GameControllerAxis axis);

protected:
	SDL_GameController* controller;
	Sint16 prevLeftX, prevLeftY, prevRightX, prevRightY;//previous axis variables
	Sint16 currentLeftX, currentLeftY, currentRightX, currentRightY;//current axis variables	
	Uint8  previousStatePad[SDL_CONTROLLER_BUTTON_MAX];
	Uint32 mButtonTime[512];
	Uint8  currentStatePad[SDL_CONTROLLER_BUTTON_MAX];
};

