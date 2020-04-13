#pragma once

enum MOUSEBUTTON {
	LEFT,
	RIGHT,
	MIDDLE
};
struct Drag {
	glm::vec2 mInit;
	glm::vec2 mCur;
};

const float SENSITIVITY = 0.05f;
/*!
 * \class Mouse interface used for gathering the change in mouse position and screen position
 * \brief Interface for gathering data on the mouse position
 * 
 */
class Mouse
{
public:
	/*!
	 * 
	 * 
	 * \return instance of the mouse
	 */
	GEAR_API static Mouse* GetInstance();
	/*!
	 * Set the mouse variables
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Update mouse values
	 * 
	 */
	GEAR_API void Update();
	/*!
	 * Delete relevent data 
	 * 
	 */
	GEAR_API void CleanUp();
	
	GEAR_API void ProcessMouseEvents(SDL_Event *aMouseEvent);
	/*!
	 * Get the mouse's previous screen position
	 *
	 * \return x,y relative to the screen
	 */
	GEAR_API glm::vec2 GetPrevMouseScreenPos();
	/*!
	 * Get the mouse's screen position
	 * 
	 * \return x,y relative to the screen
	 */
	GEAR_API glm::vec2 GetMouseScreenPos();
	/*!
	 * Get the mouse's screen position
	 * 
	 * \param x will be set
	 * \param y will be set
	 */
	GEAR_API void GetMouseScreenPos(float& x, float& y);

	GEAR_API std::pair<float, float> GetMousePos();

	/*!
	 * Get the change in mouse position
	 * 
	 * \return vec2 of change in mouse position
	 */
	GEAR_API glm::vec2 GetMouseDeltas();
	/*!
	 *  Get the change in mouse position
	 * 
	 * \param dx will be set
	 * \param dy will be set
	 */
	GEAR_API void GetMouseDeltas111(float& dx, float& dy);
	/*!
	 * Get if a mouse button is triggered
	 *
	* \param aClick mouse button int
	* \return if aCLick is triggered
	*/
	GEAR_API bool IsTriggered(int aClick);
	/*!
	 * Get if a mouse button is released
	 *
	* \param aClick mouse button int
	* \return if aCLick is relaesed
	*/
	GEAR_API bool IsReleased(int aClick);
	/*!
	 * Get if a mouse button is pressed
	 *
	 * \param aClick mouse button int
	 * \return if aCLick is Pressed
	 */
	GEAR_API bool IsPressed(int aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 *
	 * \param aClick mouse button string
	 * \return amount changed in X while aCLick was down
	 */
	GEAR_API float MouseDragX(std::string aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 *
	 * \param aClick mouse button integer
	 * \return amount changed in Y while aCLick was down
	 */
	GEAR_API float MouseDragX(int aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 *
	 * \param aClick mouse button string
	 * \return amount changed in Y while aCLick was down
	 */
	GEAR_API float MouseDragY(std::string aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 *
	 * \param aClick mouse button integer
	 * \return amount changed in Y while aCLick was down
	 */
	GEAR_API float MouseDragY(int aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 *
	 * \param aClick mouse button string
	 * \return amount changed while aCLick was down
	 */
	GEAR_API std::pair<float, float> MouseDrag(std::string aClick);
	/*!
	 * Get the change in mouse position depending on mouse button
	 * 
	 * \param aClick mouse button
	 * \return amount changed while aCLick was down
	 */
	GEAR_API std::pair<float,float> MouseDrag(int aClick);
	/*!
	 * Get teh change in mouse x
	 *
	 * \return x change
	 */
	GEAR_API float GetMouseDeltaX();
	/*!
	 * Get teh change in mouse y
	 * 
	 * \return y change
	 */
	GEAR_API float GetMouseDeltaY();
	/*!
	 * Get teh mouse sensitivity variable
	 * 
	 * \return mouse senistivitly multiplier
	 */
	GEAR_API float GetSensitivity();
	/*!
	 * Check if the mouse scrolled up
	 * 
	 * \return if scrolled up
	 */
	GEAR_API bool ScrollUp();
	/*!
	 * Check if the mouse scrolled down
	 *
	 * \return if scrolled down
	 */
	GEAR_API bool ScrollDown();

	
private:
	GEAR_API Mouse();
	GEAR_API ~Mouse();
private:
	static Mouse* mInstance;

	Drag mDragState[3];

	bool mPrevMouseState[3];
	bool mCurrentMouseState[3];

	//mouse
	int mPrev_mouse_pos_x_;
	int mPrev_mouse_pos_y_;
	int mMouse_pos_x_;
	int mMouse_pos_y_;
	int mMouse_dx_;
	int mMouse_dy_;

	float mMouseSensitivity;

	bool m_prev_mouse_button_state_[3];
	bool m_curr_mouse_button_state_[3];

	int m_prevScrollValue;
	int m_currScrollValue;

};

