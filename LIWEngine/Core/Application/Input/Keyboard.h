/******************************************************************************
Class:Keyboard
Implements:InputDevice
Author:Rich Davison
Description:RAW Input keyboard class. I've made absolutely no attempt to ensure
that this class works for multiple keyboards attached to a single system.

You shouldn't really have to mess with this class too much, there's nothing too
interesting about it!

STUDENT CHALLENGE! You could have a function pointer per key, that can be
automatically called when a key is triggered / held? (Checked from within the
UpdateFrame function)

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
#include "string.h"
#include "map"

#include "InputDevice.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GL.h>

#include "Keycode.h"

namespace LIW {
	namespace App {
		enum KeyboardActions {
			KEYBOARD_ACTION_UNKNOWN = -1,
			KEYBOARD_ACTION_PRESS = 0,
			KEYBOARD_ACTION_RELEASE = 1
		};

		class Keyboard :
			public InputDevice {
		public:
			friend class Window;

			//Is this key currently pressed down?
			bool KeyDown(KeyboardKeys key);
			//Has this key been held down for multiple frames?
			bool KeyHeld(KeyboardKeys key);
			//Is this the first update the key has been pressed for?
			bool KeyTriggered(KeyboardKeys key);
			//Is this the first update the key has been pressed for?
			inline bool KeyPressed(KeyboardKeys key) { return KeyTriggered(key); }
			//Is this the first update the key has been released?
			bool KeyReleased(KeyboardKeys key);

		protected:
			Keyboard();
			~Keyboard(void) {}

			//UpdateFrame (holdStates and releaseStates)
			virtual void UpdateFrame(float dt) override;

			//UpdateFrame the holdStates array...call this each frame!
			virtual void UpdateHolds();
			//UpdateFrame the releaseStates array...call this each frame!
			virtual void UpdateReleases();
			//OnUpdate the keyStates array etc...input callback!
			virtual void OnUpdateKey(KeyboardKeys key, KeyboardActions action);
			//Sends the keyboard to sleep
			virtual void Sleep();

			bool keyStates[KEYBOARD_MAX];		//Is the key down?
			bool releaseStates[KEYBOARD_MAX];		//Is the key released in this update?
			bool holdStates[KEYBOARD_MAX];		//Has the key been down for multiple updates?
		};
	}
}





