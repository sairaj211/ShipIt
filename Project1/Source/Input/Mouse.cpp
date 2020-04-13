#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Mouse.h"
#include "Bindings.h"



#define BINDINGS Bindings::GetInstance()

Mouse * Mouse::mInstance = nullptr;

Mouse * Mouse::GetInstance()
{
	if (mInstance == nullptr) {
		mInstance = new Mouse();
	}
	return mInstance;
}

Mouse::~Mouse()
{
	// This might not be needed.  It occasionally causes crashes
	/*SDL_free(mCurrentMouseState);
	SDL_free(mPrevMouseState);*/
}

Mouse::Mouse()
{
	m_currScrollValue = 0;
	m_prevScrollValue = 0;
	mMouseSensitivity = SENSITIVITY;//gather from mouse data?
	SDL_memset(mCurrentMouseState, 0, 3 * sizeof(bool));
	SDL_memset(mPrevMouseState, 0, 3 * sizeof(bool));
}

void Mouse::Init()
{
	
	// Setup Mouse
	m_curr_mouse_button_state_[MOUSEBUTTON::LEFT] = m_curr_mouse_button_state_[MOUSEBUTTON::RIGHT] = m_curr_mouse_button_state_[MOUSEBUTTON::MIDDLE] = false;
	m_prev_mouse_button_state_[MOUSEBUTTON::LEFT] = m_prev_mouse_button_state_[MOUSEBUTTON::RIGHT] = m_prev_mouse_button_state_[MOUSEBUTTON::MIDDLE] = false;
}

void Mouse::Update()
{
	for (int i = MOUSEBUTTON::LEFT; i <= MOUSEBUTTON::MIDDLE; ++i) {
		m_prev_mouse_button_state_[i] = m_curr_mouse_button_state_[i];
	}

	mPrev_mouse_pos_x_ = mMouse_pos_x_;
	mPrev_mouse_pos_y_ = mMouse_pos_y_;

	Uint8 mouseState = SDL_GetMouseState(&mMouse_pos_x_, &mMouse_pos_y_);
	m_curr_mouse_button_state_[MOUSEBUTTON::LEFT] = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) == SDL_BUTTON(SDL_BUTTON_LEFT);
	m_curr_mouse_button_state_[MOUSEBUTTON::RIGHT] = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) == SDL_BUTTON(SDL_BUTTON_RIGHT);
	m_curr_mouse_button_state_[MOUSEBUTTON::MIDDLE] = (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) == SDL_BUTTON(SDL_BUTTON_MIDDLE);
	SDL_GetRelativeMouseState(&mMouse_dx_, &mMouse_dy_);

	//Scroll
	if (m_currScrollValue >= 256)
		m_currScrollValue = 0;
	m_prevScrollValue = m_currScrollValue;

	for (int button = 0; button < 3; ++button) {
		if (IsPressed(button)) {
			mDragState[button].mCur = GetMouseScreenPos();
		}
	}


}

void Mouse::CleanUp()
{
	delete mInstance;
	mInstance = nullptr;
}

void Mouse::ProcessMouseEvents(SDL_Event * aMouseEvent)
{
	switch (aMouseEvent->type) {
		/*case SDL_MOUSEBUTTONDOWN:
		{
			if (aMouseEvent->button.button == SDL_BUTTON_LEFT) m_curr_mouse_button_state_[MOUSEBUTTON::LEFT] = true;
			if (aMouseEvent->button.button == SDL_BUTTON_RIGHT) m_curr_mouse_button_state_[MOUSEBUTTON::RIGHT] = true;
			if (aMouseEvent->button.button == SDL_BUTTON_MIDDLE) m_curr_mouse_button_state_[MOUSEBUTTON::MIDDLE] = true;
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			if (aMouseEvent->button.button == SDL_BUTTON_LEFT) m_curr_mouse_button_state_[MOUSEBUTTON::LEFT] = false;
			if (aMouseEvent->button.button == SDL_BUTTON_RIGHT) m_curr_mouse_button_state_[MOUSEBUTTON::RIGHT] = false;
			if (aMouseEvent->button.button == SDL_BUTTON_MIDDLE) m_curr_mouse_button_state_[MOUSEBUTTON::MIDDLE] = false;
			break;
		}*/
		case SDL_MOUSEWHEEL:
		{
			if (aMouseEvent->wheel.y > 0) m_currScrollValue++;
			if (aMouseEvent->wheel.y < 0) m_currScrollValue--;
			break;
		}
	}
}


bool Mouse::IsReleased(int aClick)
{
	return !m_curr_mouse_button_state_[aClick] && m_prev_mouse_button_state_[aClick];
}

bool Mouse::IsTriggered(int aClick)
{
	if (m_curr_mouse_button_state_[aClick] && !m_prev_mouse_button_state_[aClick]) {
		mDragState[aClick].mInit = GetMouseScreenPos();
		return true;
	}
	return false;
}


bool Mouse::IsPressed(int aClick)
{
	return m_curr_mouse_button_state_[aClick] && m_prev_mouse_button_state_[aClick];
}



float Mouse::MouseDragX(std::string aClick)
{
	int mouse = BINDINGS->BindingToMouseButton(aClick);
	if (mouse > 0 && mouse < 4) return MouseDragX(mouse);
	return 0.0f;
}

float Mouse::MouseDragX(int aClick)
{
	return mDragState[aClick].mCur.x - mDragState[aClick].mInit.x;
}

float Mouse::MouseDragY(std::string aClick)
{
	int mouse = BINDINGS->BindingToMouseButton(aClick);
	if (mouse > 0 && mouse < 4) return MouseDragY(mouse);
	return 0.0f;
}

float Mouse::MouseDragY(int aClick)
{
	return mDragState[aClick].mCur.y - mDragState[aClick].mInit.y;
}

std::pair<float, float> Mouse::MouseDrag(std::string aClick)
{
	int mouse = BINDINGS->BindingToMouseButton(aClick);
	if (mouse > 0 && mouse < 4) return MouseDrag(mouse);
	return std::pair<float, float>(0.f,0.f);
}
std::pair<float, float> Mouse::MouseDrag(int aClick)
{
	return std::pair<float, float>(mDragState[aClick].mCur.x - mDragState[aClick].mInit.x,mDragState[aClick].mCur.y - mDragState[aClick].mInit.y);
}

float Mouse::GetMouseDeltaX()
{
	return float(mMouse_dx_);
}

float Mouse::GetMouseDeltaY()
{
	return float(mMouse_dy_);
}

float Mouse::GetSensitivity()
{
	return SENSITIVITY;
}

bool Mouse::ScrollUp()
{
	if (m_currScrollValue > m_prevScrollValue)
		return true;
	return false;
}

bool Mouse::ScrollDown()
{
	if (m_currScrollValue < m_prevScrollValue)
		return true;
	return false;
}

glm::vec2 Mouse::GetPrevMouseScreenPos()
{
	return glm::vec2((float)mPrev_mouse_pos_x_, (float)mPrev_mouse_pos_y_);
}

glm::vec2 Mouse::GetMouseScreenPos()
{
	return glm::vec2((float)mMouse_pos_x_, (float)mMouse_pos_y_);
}

void Mouse::GetMouseScreenPos(float& x, float& y)
{
	x = (float)mMouse_pos_x_;
	y = (float)mMouse_pos_y_;
}

 std::pair<float, float> Mouse::GetMousePos()
{
	return std::pair<float, float>((float)mMouse_pos_x_ , (float)mMouse_pos_y_);

}

glm::vec2 Mouse::GetMouseDeltas()
{
	return glm::vec2((float)mMouse_dx_, (float)mMouse_dy_);
}

void Mouse::GetMouseDeltas111(float& dx, float& dy)
{
	dx = (float)(mMouse_dx_);
	dy = (float)(mMouse_dy_);
}