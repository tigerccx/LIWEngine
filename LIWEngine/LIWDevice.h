#pragma once


namespace LIW{
	namespace App {
		class LIWDevice {
		protected:
			friend class Window;
			LIWDevice(void) {};
			~LIWDevice(void) {};

		protected:
			virtual void Sleep() { isAwake = false; }
			virtual void Wake() { isAwake = true; }

			bool			isAwake;		//Is the device awake...
		};
	}
}