#pragma once
#include "LIWEntityAttachment.h"

namespace LIW{
	class LIWEntity;

	/*
	* Class: LIWModel
	* Author: tigerccx
	* Description:	This class is used as a baseclass Model part of a MVC-like framework. 
	*/
	class LIWModel:
		public LIWEntityAttachment
	{
	public:
		LIWModel() : LIWEntityAttachment() {}
		virtual ~LIWModel() {}
	protected:

	};
}
