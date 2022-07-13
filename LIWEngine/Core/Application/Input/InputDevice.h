/******************************************************************************
Class:InputDevice
Implements:
Author:Rich Davison
Description:Abstract base class for Windows RAW keyboard / mouse input

Input devices can be temporarily sent to sleep (so keyboard input doesn't work
when the game is minimised etc), and obviously woken up again.

Input devices may also keep track of 'holds' - i.e keys or buttons pressed for
more than one frame. This allows you to have both things that trigger once,
and are continuously active as long as a key / button is pressed.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""      
 
*//////////////////////////////////////////////////////////////////////////////
/*
* Revision
* Author: tigerccx
* Description: This class is modified to integrate with GLFW.
*/

#pragma once
//#include "..\common.h"
//#include <algorithm>
#include "..\LIWDevice.h"

namespace LIW {
	namespace App {
		class InputDevice:
			public LIWDevice
		{
		protected:
			friend class Window;
			virtual void UpdateFrame(float dt) = 0; //This should be called every frame
		};
	}
}
