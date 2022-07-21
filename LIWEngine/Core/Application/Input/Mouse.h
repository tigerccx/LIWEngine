/******************************************************************************
Class:Mouse
Implements:InputDevice
Author:Rich Davison
Description:Windows RAW input mouse, with a couple of game-related enhancements

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
/*
* Revision
* Author: tigerccx
* Description: This class is modified to act as an API layer.
*/

#pragma once

#include "InputDevice.h"
#include <map>
#include "Maths/LIWMaths.h"

#include "Mousecode.h"
namespace LIW {
	namespace App {
		enum MouseButtonActions {
			MOUSE_BUTTON_ACTION_UNKNOWN = -1,
			MOUSE_BUTTON_ACTION_PRESS = 0,
			MOUSE_BUTTON_ACTION_RELEASE = 1
		};

		class Mouse :
			public InputDevice {
		public:
			friend class Window;

			//Is this mouse button currently pressed down?
			bool	ButtonDown(MouseButtons button);
			//Has this mouse button been held down for multiple frames?
			bool	ButtonHeld(MouseButtons button);
			//Is this the first update the button has been released?
			bool	ButtonReleased(MouseButtons button);
			//Is this the first update the button has been pressed for?
			bool	ButtonClicked(MouseButtons button);
			//Has this mouse button been double clicked?
			bool	DoubleClicked(MouseButtons button);

			//Get how much this mouse has moved since last frame
			Maths::LIWVector2	GetRelativePosition();
			//Get the window position of the mouse pointer
			Maths::LIWVector2	GetAbsolutePosition();

			//Determines the maximum amount of ms that can pass between
			//2 mouse presses while still counting as a 'double click'
			//void	SetDoubleClickLimit(float msec);

			/*Note: Wheel movement only supports Y scroll since it's the most common one
			(though X scroll is provided, no api is exposed just yet)*/
			//Has the mouse wheel moved since the last update?
			bool	WheelMoved();
			//Get the mousewheel movement. Positive means scroll up,
			//negative means scroll down, 0 means no movement.
			int		GetWheelMovement();

			//Sets the mouse sensitivity. Currently only affects the 'relative'
			//(i.e FPS-style) mouse movement. Students! Maybe you'd like to
			//implement a 'MenuSensitivity' for absolute movement?
			void	SetMouseSensitivity(float amount);

		protected:
			Mouse();
			~Mouse(void) {}

			//OnUpdate mouse buttons... callback!
			virtual void	OnUpdateButton(MouseButtons button, MouseButtonActions action);
			//OnUpdate mouse movements... callback!
			virtual void	OnUpdateMovement(double xpos, double ypos);
			//OnUpdate mouse scroll... callback!
			virtual void	OnUpdateScroll(double xoffset, double yoffset);

			//UpdateFrame
			virtual void UpdateFrame(float dt) override;

			//UpdateFrame the holdButtons array. Call once per frame!
			virtual void	UpdateHolds();
			//UpdateFrame the releaseButtons array...call this each frame!
			virtual void	UpdateReleases();
			//UpdateFrame the frameWheel...call this each frame!
			virtual void	UpdateScroll();
			//UpdateFrame the relativePosition...call this each frame!
			virtual void	UpdatePosition();
			//Sends the mouse to sleep (i.e window has been alt-tabbed away etc)
			virtual void	Sleep();

			//Updates the doubleclicks array. Call once per frame!
			void			UpdateDoubleClick(float dt);

			//Set the mouse's current screen position. Maybe should be public?
			void			SetAbsolutePosition(unsigned int x, unsigned int y);

			//Set the absolute screen bounds (<0 is always assumed dissallowed). Used
			//by the window resize routine...
			void			SetAbsolutePositionBounds(unsigned int maxX, unsigned int maxY);

			//Current mouse absolute position
			Maths::LIWVector2		absolutePosition;
			//Current mouse absolute position maximum bounds
			Maths::LIWVector2		absolutePositionBounds;
			//How much as the mouse moved since the last raw packet?
			Maths::LIWVector2		relativePosition;
			//Current button down state for each button
			bool					buttons[MOUSE_BUTTON_MAX];
			//Current button held state for each button
			bool					holdButtons[MOUSE_BUTTON_MAX];
			//Current button release state for each button
			bool					releaseButtons[MOUSE_BUTTON_MAX];
			//Current doubleClick counter for each button
			bool					doubleClicks[MOUSE_BUTTON_MAX];
			//Counter to remember when last mouse click occured
			float					lastClickTime[MOUSE_BUTTON_MAX];

			//Current mousewheel updated position
			Maths::LIWVector2		frameWheel;

			//Max amount of ms between clicks count as a 'double click'
			float					clickLimit;

			//Mouse pointer sensitivity. Set this negative to get a headache!
			float					sensitivity;

			bool					setAbsolute;
		};
	}
}


