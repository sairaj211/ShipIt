#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "GamePad.h"

GamePad::GamePad(SDL_GameController * _controller): controller(_controller)
{
	SDL_memset(currentStatePad, 0, SDL_CONTROLLER_BUTTON_MAX * sizeof(Uint8));
	SDL_memset(previousStatePad, 0, SDL_CONTROLLER_BUTTON_MAX * sizeof(Uint8));
}

GamePad::~GamePad()
{

	if (controller != nullptr) {
		SDL_GameControllerClose(controller);
	}
	controller = nullptr;
}

void GamePad::Reset()
{
	controller = nullptr;
	SDL_memset(currentStatePad, 0, SDL_CONTROLLER_BUTTON_MAX * sizeof(Uint8));
	SDL_memset(previousStatePad, 0, SDL_CONTROLLER_BUTTON_MAX * sizeof(Uint8));
	currentLeftX = currentRightX = currentLeftY = currentRightY = 0;
}

SDL_GameController * GamePad::GetController()
{
	return controller;
}

void GamePad::SetController(SDL_GameController * _controller)
{
	controller = _controller;
}

void GamePad::Update()
{
	if (controller == nullptr) return;
	if(SDL_GameControllerGetAttached(controller)) {
		memcpy(previousStatePad, currentStatePad, 512);
		
		SDL_GameControllerUpdate();

		for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
			currentStatePad[i] = SDL_GameControllerGetButton(controller, SDL_GameControllerButton(i));

		prevLeftX = currentLeftX; prevLeftY = currentLeftY; 
		prevRightX = currentRightX; prevRightY = currentRightY;

		currentLeftX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
		if (currentLeftX < 3*SHRT_MIN / 4)  
			currentLeftX = SHRT_MIN;
		else if (currentLeftX > 3 * SHRT_MAX / 4) 
			currentLeftX = SHRT_MAX;
		else if (currentLeftX < SHRT_MAX / 4 && currentLeftX > SHRT_MIN / 4) 
			currentLeftX = 0;
		else if (currentLeftX < 0)
			currentLeftX = SHRT_MIN / 2;
		else
			currentLeftX = SHRT_MAX / 2;

		currentLeftY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
		if (currentLeftY < 3*SHRT_MIN / 4)  
			currentLeftY = SHRT_MIN;
		else if (currentLeftY > 3*SHRT_MAX / 4) 
			currentLeftY = SHRT_MAX;
		else if (currentLeftY < SHRT_MAX / 4 && currentLeftY > SHRT_MIN / 4) 
			currentLeftY = 0;
		else if(currentLeftY < 0)
			currentLeftY = SHRT_MIN/2;
		else
			currentLeftY = SHRT_MAX/2;

		currentRightX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
		if (currentRightX < 3*SHRT_MIN / 4)  
			currentRightX = SHRT_MIN;
		else if (currentRightX > 3*SHRT_MAX / 4) 
			currentRightX = SHRT_MAX;
		else if(currentRightX < SHRT_MAX / 4 && currentRightX > SHRT_MIN / 4) 
			currentRightX = 0;
		else if (currentRightX < 0)
			currentRightX = SHRT_MIN / 2;
		else
			currentRightX = SHRT_MAX / 2;

		currentRightY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
		if (currentRightY < 3*SHRT_MIN / 4) 
			currentRightY = SHRT_MIN;
		else if (currentRightY > 3*SHRT_MAX / 4) 
			currentRightY = SHRT_MAX;
		else if(currentRightY < SHRT_MAX / 4 && currentRightY > SHRT_MIN / 4) 
			currentRightY = 0;
		else if (currentRightY < 0)
			currentRightY = SHRT_MIN / 2;
		else
			currentRightY = SHRT_MAX / 2;
	}
	else {
		Reset();
	}
}

bool GamePad::IsPressed( SDL_GameControllerButton button)
{
	if (button > SDL_CONTROLLER_BUTTON_MAX || controller == nullptr) return false;
	return (previousStatePad[button] == 1) && (currentStatePad[button] == 1);
}

bool GamePad::IsTriggered( SDL_GameControllerButton button)
{
	if (button > SDL_CONTROLLER_BUTTON_MAX || controller == nullptr) return false;
	return (previousStatePad[button] == 0) && (currentStatePad[button] == 1);
}

bool GamePad::IsReleased(SDL_GameControllerButton button)
{
	if (button > SDL_CONTROLLER_BUTTON_MAX || controller == nullptr) return false;
	return (previousStatePad[button] == 1) && (currentStatePad[button] == 0);
}



bool GamePad::IsStickTriggeredUp(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY < prevLeftY && prevLeftY == 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY < prevRightY && prevRightY == 0);
}

bool GamePad::IsStickTriggeredDown(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY > prevLeftY && prevLeftY == 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY > prevRightY && prevRightY == 0);
}

bool GamePad::IsStickTriggeredLeft(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX < prevLeftX && prevLeftX == 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX < prevRightX && prevRightX == 0);
}

bool GamePad::IsStickTriggeredRight(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX > prevLeftX && prevLeftX == 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX > prevRightX && prevRightX == 0);
}

//

bool GamePad::IsStickPressedUp(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY == prevLeftY && currentLeftY < 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY == prevRightY && currentRightY < 0);
}

bool GamePad::IsStickPressedDown(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY == prevLeftY && currentLeftY > 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY == prevRightY && currentRightY > 0);
}

bool GamePad::IsStickPressedLeft(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX == prevLeftX && currentLeftX < 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX == prevRightX && currentRightX < 0);
}

bool GamePad::IsStickPressedRight(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX == prevLeftX && currentLeftX > 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX == prevRightX && prevRightX > 0);
}

//

bool GamePad::IsStickReleasedUp(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY > prevLeftY && prevLeftY < 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY > prevRightY && prevRightY < 0);
}

bool GamePad::IsStickReleasedDown(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTX == axis ||
		SDL_CONTROLLER_AXIS_RIGHTX == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTY == axis && currentLeftY < prevLeftY && prevLeftY > 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTY == axis && currentRightY < prevRightY && prevRightY > 0);
}

bool GamePad::IsStickReleasedLeft(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX > prevLeftX && prevLeftX < 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX > prevRightX && prevRightX < 0);
}

bool GamePad::IsStickReleasedRight(SDL_GameControllerAxis axis)
{
	if (SDL_CONTROLLER_AXIS_LEFTY == axis ||
		SDL_CONTROLLER_AXIS_RIGHTY == axis) return false;

	return (SDL_CONTROLLER_AXIS_LEFTX == axis && currentLeftX < prevLeftX && prevLeftX > 0)
		|| (SDL_CONTROLLER_AXIS_RIGHTX == axis && currentRightX < prevRightX && prevRightX > 0);
}

GEAR_API float GamePad::GetAxisValue(SDL_GameControllerAxis axis)
{
	Sint16 val = 0;
	if (SDL_CONTROLLER_AXIS_LEFTY == axis)
		val = currentLeftY;
	if (SDL_CONTROLLER_AXIS_RIGHTY == axis)
		val = currentRightY;
	if (SDL_CONTROLLER_AXIS_LEFTX == axis)
		val =  currentLeftX;
	if (SDL_CONTROLLER_AXIS_RIGHTX == axis)
		val =  currentRightX;
	
	if (val > 0) return val / float(SHRT_MAX);
	if (val > 0) return val / float(SHRT_MIN);
	return val;
}


bool GamePad::IsActive()
{
	return nullptr != controller;
}

