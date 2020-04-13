/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: ivan Cazares ivan.c 60004418
- End Header --------------------------------------------------------*/

#pragma once
/*!
 * 
 * 
 */
class FrameRateManager

{
private:
	GEAR_API FrameRateManager();
	GEAR_API ~FrameRateManager();

public:
	/*!
	 * 
	 * 
	 * \return 
	 */
	GEAR_API static FrameRateManager* GetInstance();
	/*!
	 * returns start frame time
	 * 
	 */
	GEAR_API void FrameStart();
	/*!
	 * returns end frame time 
	 * 
	 */
	GEAR_API void FrameEnd();
	/*!
	 * return current frame time
	 * 
	 * \return 
	 */
	GEAR_API double GetFrameTime();
	/*!
	 * 
	 * 
	 */
	GEAR_API void Cleanup();
	/*!
	 * used to set max framerate support by the game (initially set to 60)
	 * 
	 * \param MaxFrameRate
	 */
	GEAR_API void SetMaxFrameRate(Uint32 aMaxFrameRate);

private:
	static FrameRateManager* mInstance;/*!<    */
	Uint32 mTickStart;/*!<    */
	Uint32 mTickEnd;/*!<    */
	Uint32 mFrameTime;/*!<    */
	Uint32 mNeededTicksPerFrame;/*!<    */

	
};