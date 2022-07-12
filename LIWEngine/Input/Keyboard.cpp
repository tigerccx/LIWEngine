#include "Keyboard.h"

LIW::App::Keyboard::Keyboard()
{
	//Initialise the arrays to false!
	memset(keyStates, 0, KEYBOARD_MAX * sizeof(bool));
	memset(holdStates, 0, KEYBOARD_MAX * sizeof(bool));
	memset(releaseStates, 0, KEYBOARD_MAX * sizeof(bool));
}

void LIW::App::Keyboard::UpdateFrame(float dt)
{
	UpdateHolds();
	UpdateReleases();
}

/*
Updates variables controlling whether a keyboard key has been
held for multiple frames.
*/
void LIW::App::Keyboard::UpdateHolds()	{
	memcpy(holdStates, keyStates, KEYBOARD_MAX * sizeof(bool));
}

/*
Updates variables controlling whether a keyboard key has been released (to false, because release is only a one frame operation).
*/
void LIW::App::Keyboard::UpdateReleases()
{
	memset(releaseStates, 0, KEYBOARD_MAX * sizeof(bool));
}

/*
Sends the keyboard to sleep, so it doesn't process any
keypresses until it receives a Wake()
*/
void LIW::App::Keyboard::Sleep()	{
	isAwake = false;	//Night night!
	//Prevents incorrectly thinking keys have been held / pressed when waking back up
	memset(keyStates, 0, KEYBOARD_MAX * sizeof(bool));
	memset(holdStates, 0, KEYBOARD_MAX * sizeof(bool));
	memset(releaseStates, 0, KEYBOARD_MAX * sizeof(bool));
}

/*
Returns if the key is down. Doesn't need bounds checking - 
a KeyboardKeys enum is always in range
*/
bool LIW::App::Keyboard::KeyDown(KeyboardKeys key)	{
	return keyStates[key];
}

/*
Returns if the key is down, and has been held down for multiple updates. 
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool LIW::App::Keyboard::KeyHeld(KeyboardKeys key)	{
	if(KeyDown(key) && holdStates[key]) {
		return true;
	}
	return false;
}

/*
Returns true only if the key is down, but WASN't down last update.
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool LIW::App::Keyboard::KeyTriggered(KeyboardKeys key)	 {
	return (KeyDown(key) && !KeyHeld(key));
}

/*
Returns true only if the key was down, but isn't down this update.
Doesn't need bounds checking - a KeyboardKeys enum is always in range
*/
bool LIW::App::Keyboard::KeyReleased(KeyboardKeys key)
{
	return releaseStates[key];
}

/*
Updates the keyboard state with data received from the OS.
*/
void LIW::App::Keyboard::OnUpdateKey(KeyboardKeys key, KeyboardActions action)
{
	if (isAwake) {
		//We should do bounds checking!
		if (key < 0 || key > KEYBOARD_MAX) {
			return;
		}

		if (action == KEYBOARD_ACTION_UNKNOWN) {
			return;
		}

		if (action == KEYBOARD_ACTION_PRESS) {
			keyStates[key] = true;
		}
		else if (action == KEYBOARD_ACTION_RELEASE) {
			holdStates[key] = false;
			keyStates[key] = false;
			releaseStates[key] = true;
		}
	}
}