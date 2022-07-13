#pragma once
#include "LIWObject.h"
#include "LIWReflection.h"

namespace LIW {
	/*
	* Class: LIWReflectionObject
	* Author: tigerccx
	* Description:	Baseclass for LIWObjects holding reflectable attributes.
	*				(Simply used to provide better API for reflection)
	*				Note: Please use macro LIWReflectAttr to declare reflectable attributes. 
	*				eg. 
	*				class SomeComponent: public LIWComponent
	*				{
	*					LIWReflectInt(m_MyInt0);
	*					LIWReflectFloat(m_MyFloat0);
	*					LIWReflectFloatVal(m_MyFloat1, 1.0f);
	*					//...
	*				}
	*/
	class LIWReflectionObject:
		public LIWObject
	{
	public:
		//
		// Reflection
		//
		template<class T> bool Set(const std::string& attrName, const T& val) {
			return LIWReflectHelper::Set(this, attrName, val);
		}

		template<class T> bool Get(const std::string& attrName, T& valOut) const {
			return LIWReflectHelper::Get(this, attrName, valOut);
		}

		template<class T> T* GetPt(const std::string& attrName) const {
			return LIWReflectHelper::GetPt(this, attrName);
		}

		void* GetPtVoid(const std::string& attrName) const {
			return LIWReflectHelper::GetPtVoid(this, attrName);
		}

		bool GetAttrs(ReflectAttrBook& attrs) const {
			return LIWReflectHelper::GetAttrs(this->GetObjectType(), attrs);
		}

		bool GetAttrsR(ReflectAttrBook& attrs) const {
			return LIWReflectHelper::GetAttrsR(this->GetObjectType(), attrs);
		}
	};
}