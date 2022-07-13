#pragma once
#include "LIWObject.h"

namespace LIW {
	/*
	* Class: Controller
	* Author: tigerccx
	* Description:	This class is used as a baseclass Controller part of a MVC-like framework.
	*/
	class LIWController :
		public LIWObject
	{
	public:
		LIWController():LIWObject(){}
		virtual ~LIWController(){}

		virtual void InjectData(void* data) = 0;

		virtual void Start() {}; //Called the first frame of functioning
		virtual void Update(float dt) {};   //Called each frame
		virtual void FixedUpdate(float fdt) {};		//Called each frame at a fixed rate
		virtual void PreRender() {};	//Called each time just before rendering stage
		virtual void PostRender() {};	//Called each time just after rendering stage
		virtual void PreDestroy() {};	//Called before destroyed
	};
}
