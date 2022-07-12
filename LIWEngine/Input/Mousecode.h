#pragma once

namespace LIW {
	// Use GLFW standard mouse button code
// https://www.glfw.org/docs/latest/group__buttons.html#ga3e2f2cf3c4942df73cc094247d275e74
	enum MouseButtons {
		MOUSE_BUTTON_UNKNOWN = -1,
		MOUSE_BUTTON_1 = 0,
		MOUSE_BUTTON_2 = 1,
		MOUSE_BUTTON_3 = 2,
		MOUSE_BUTTON_4 = 3,
		MOUSE_BUTTON_5 = 4,
		MOUSE_BUTTON_6 = 5,
		MOUSE_BUTTON_7 = 6,
		MOUSE_BUTTON_8 = 7,
		MOUSE_BUTTON_MAX = 8
	};

#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3
}
