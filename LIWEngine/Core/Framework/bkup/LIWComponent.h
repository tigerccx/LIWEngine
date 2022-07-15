#pragma once
#include <string>
#include "Reflection/LIWReflection.h"

namespace LIW{
#define LIW_SET_COMPONENT_NAME(component) \
inline virtual std::string GetName() override{ return #component; }

	class LIWEntity;

	/*
	* Class: LIWComponent
	* Author: tigerccx
	* Description:	This class is used as a baseclass Model part of a MVC-like framework. 
	* 				Note: Please use macro LIWReflectAttr to declare reflectable attributes. 
	*				eg. 
	*				class SomeComponent: public LIWComponent
	*				{
	*					LIWReflectInt(m_MyInt0);
	*					LIWReflectFloat(m_MyFloat0);
	*					LIWReflectFloatVal(m_MyFloat1, 1.0f);
	*					//...
	*				}
	*/
	LIW_REFLECT_CLASS_INHERIT_BEG(LIWComponent, LIWObject)
	public:
		LIWComponent() :
			LIWObject() { }
		virtual ~LIWComponent() { }

		/*
		* Func: SetEntity
		* Description:	Set the Entity this Component is attached to.
		*				Note: Should only be called by the Entity.
		*/
		inline void SetEntity(LIWEntity* entity) { m_entity = entity; }
		/*
		* Func: GetEntity
		* Description:	Get the Entity this Component is attached to.
		*/
		inline LIWEntity* GetEntity() const { return m_entity; }
		/*
		* Func: GetName
		* Description: Get the name of Component.
		*/
		inline virtual std::string GetName() = 0;

	//public:
	//	//
	//	// Reflection
	//	//
	//	template<class T> bool Set(const std::string& attrName, const T& val) {
	//		return LIWReflectHelper::Set(this, attrName, val);
	//	}

	//	template<class T> bool Get(const std::string& attrName, T& valOut) const {
	//		return LIWReflectHelper::Get(this, attrName, valOut);
	//	}

	//	template<class T> T* GetPt(const std::string& attrName) const {
	//		return LIWReflectHelper::GetPt(this, attrName);
	//	}

	//	void* GetPtVoid(const std::string& attrName) const {
	//		return LIWReflectHelper::GetPtVoid(this, attrName);
	//	}

	//	bool GetAttrs(ReflectAttrBook& attrs) const {
	//		return LIWReflectHelper::GetAttrs(this->GetObjectType(), attrs);
	//	}

	//	bool GetAttrsR(ReflectAttrBook& attrs) const {
	//		return LIWReflectHelper::GetAttrsR(this->GetObjectType(), attrs);
	//	}


	protected:
		LIWEntity* m_entity{ nullptr };
	LIW_REFLECT_CLASS_INHERIT_END
}
