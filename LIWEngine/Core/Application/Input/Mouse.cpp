#include "Mouse.h"

LIW::App::Mouse::Mouse()	{
	memset( buttons, 0,	 sizeof(bool) * MOUSE_BUTTON_MAX );
	memset( holdButtons, 0, sizeof(bool) * MOUSE_BUTTON_MAX );

	memset( doubleClicks, 0,  sizeof(bool)  * MOUSE_BUTTON_MAX );
	memset( lastClickTime, 0, sizeof(float) * MOUSE_BUTTON_MAX );

	frameWheel  = Vector2::zero;
	sensitivity = 0.07f;	//Chosen for no other reason than it's a nice value for my Deathadder ;)
	clickLimit  = 0.2f;

	setAbsolute = false;
}

void LIW::App::Mouse::OnUpdateButton(MouseButtons button, MouseButtonActions action)
{
	if (isAwake) {
		//We should do bounds checking!
		if (button < 0 || button > MOUSE_BUTTON_MAX) {
			return;
		}

		if (action == MOUSE_BUTTON_ACTION_PRESS) {
			//The button was pressed!
			buttons[button] = true;

			/*
			If it wasn't too long ago since we last clicked, we trigger a double click!
			*/
			if (lastClickTime[button] > 0) {
				doubleClicks[button] = true;
			}

			/*
			No matter whether the mouse was double clicked or not, we reset the clicklimit
			*/
			lastClickTime[button] = clickLimit;
		}
		else if (action == MOUSE_BUTTON_ACTION_RELEASE) {
			buttons[button] = false;
			holdButtons[button] = false;
			releaseButtons[button] = true;
		}
	}
}

void LIW::App::Mouse::OnUpdateMovement(double xpos, double ypos)
{
	if (isAwake) {
		Vector2 prevAbsolute = absolutePosition;
		absolutePosition.x = float(xpos);
		absolutePosition.y = float(ypos);

		if (setAbsolute) {
			relativePosition.x = (absolutePosition.x - prevAbsolute.x) * sensitivity;
			relativePosition.y = (absolutePosition.y - prevAbsolute.y) * sensitivity;
		}
		setAbsolute = true;
	}
}

void LIW::App::Mouse::OnUpdateScroll(double xoffset, double yoffset)
{
	frameWheel.x = float(xoffset);
	frameWheel.y = float(yoffset);
}

/*
Sets the mouse sensitivity (higher = mouse pointer moves more!)
Lower bounds checked.
*/
void LIW::App::Mouse::SetMouseSensitivity(float amount)	{
	if(amount == 0.0f) {
		amount = 1.0f;
	}
	sensitivity = amount;	
}

void LIW::App::Mouse::UpdateFrame(float dt)
{
	UpdateDoubleClick(dt);
	UpdateHolds();
	UpdateReleases();
	UpdateScroll();
	UpdatePosition();
}

/*
Updates variables controlling whether a mouse button has been
held for multiple frames. Also updates relative movement.
*/
void LIW::App::Mouse::UpdateHolds()	{
	memcpy(holdButtons,buttons,	MOUSE_BUTTON_MAX * sizeof(bool));
}

void LIW::App::Mouse::UpdateReleases()
{
	memset(releaseButtons, 0, MOUSE_BUTTON_MAX * sizeof(bool));
}

void LIW::App::Mouse::UpdateScroll()
{
	frameWheel.ToZero();
}

void LIW::App::Mouse::UpdatePosition()
{
	relativePosition.ToZero();
}

/*
Sends the mouse to sleep, so it doesn't process any
movement or buttons until it receives a Wake()
*/
void LIW::App::Mouse::Sleep()	{
	isAwake		= false;	//Bye bye for now
	clickLimit	= 0.2f;
	memset(holdButtons,	0,  MOUSE_BUTTON_MAX * sizeof(bool) );
	memset(releaseButtons, 0, MOUSE_BUTTON_MAX * sizeof(bool));
	memset(buttons,	0,	MOUSE_BUTTON_MAX * sizeof(bool) );
}

/*
Forces the mouse pointer to a specific point in absolute space.
*/
void LIW::App::Mouse::SetAbsolutePosition(unsigned int x, unsigned int y)	{
	absolutePosition.x = (float)x;
	absolutePosition.y = (float)y;
}

/*
Returns if the button is down. Doesn't need bounds checking - 
an INPUT_KEYS enum is always in range
*/
bool LIW::App::Mouse::ButtonDown(MouseButtons b)	{
	return buttons[b];
}

/*
Returns if the button is down, and has been held down for multiple updates. 
Doesn't need bounds checking - an INPUT_KEYS enum is always in range
*/
bool LIW::App::Mouse::ButtonHeld(MouseButtons b)	{
	return holdButtons[b];
}

/*
Returns true only if the button was down, but isn't down this update.
Doesn't need bounds checking - a MouseButtons enum is always in range
*/
bool LIW::App::Mouse::ButtonReleased(MouseButtons button)
{
	return releaseButtons[button];
}

/*
Returns true only if the button is down, but WASN't down last update.
Doesn't need bounds checking - a MouseButtons enum is always in range
*/
bool LIW::App::Mouse::ButtonClicked(MouseButtons button)
{
	return (ButtonDown(button) && !ButtonHeld(button));
}

/*
Returns how much the mouse has moved by since the last frame.
*/
Vector2	LIW::App::Mouse::GetRelativePosition()	{
	return relativePosition;
}

/*
Returns the mouse pointer position in absolute space.
*/
Vector2 LIW::App::Mouse::GetAbsolutePosition()	{
	return absolutePosition;
}

/*
Returns how much the mouse has moved by since the last frame.
*/
void LIW::App::Mouse::SetAbsolutePositionBounds(unsigned int maxX, unsigned int maxY)	{
	absolutePositionBounds.x = (float)maxX;
	absolutePositionBounds.y = (float)maxY;	
}

/*
Has the mousewheel been moved since the last frame?
*/
bool LIW::App::Mouse::WheelMoved()	{
	return frameWheel.y != 0;
};

/*
Returns whether the button was double clicked in this frame
*/
bool LIW::App::Mouse::DoubleClicked(MouseButtons button)	{
	return (buttons[button] && doubleClicks[button]);
}

/*
Get the mousewheel movement. Positive values mean the mousewheel
has moved up, negative down. Can be 0 (no movement)
*/
int	LIW::App::Mouse::GetWheelMovement()	{
	return (int)frameWheel.y;
}

/*
Updates the double click timers for each mouse button. dt is seconds
since the last UpdateDoubleClick call. Timers going over the double click
limit set the relevant double click value to false.
*/
void LIW::App::Mouse::UpdateDoubleClick(float dt)	{
	for(int i = 0; i < MOUSE_BUTTON_MAX; ++i) {
		if (doubleClicks[i]) { //If double clicked, clear state. 
			lastClickTime[i] = 0.0f;
			doubleClicks[i] = false;
		}
		else
		if(lastClickTime[i] > 0) { //else if double click dur over, clear state
			lastClickTime[i] -= dt;
			if(lastClickTime[i] <= 0.0f) {
				doubleClicks[i]  = false;
				lastClickTime[i] = 0.0f;
			}
		}
	}	
}