#pragma once
/*
* Module: LIWReflection
* Author: tigerccx
* Description:	In this module, a semi dynamic reflection mechanism is implemented,
*				allowing code to query
*/
#include <map>
#include <set>
#include <string>
#include "LIWObject.h"
#include "LIWType.h"

//TODO: Convert into fully static reflection?

//
// Member Reflection
//

#define __LIWRegisterReflectAttr(type, name) \
const LIW::LIWReflectHelper __##name = LIW::LIWReflectHelper::__Record(this, &this->name, LIW::GetTypeName<type>(), #name);

/*
* Macro: LIWReflectAttr
* Description: This macro is used to declare reflectable attributes.
*/
#define LIWReflectAttr(type, name)	\
type name;							\
__LIWRegisterReflectAttr(type, name)

/*
* Macro: LIWReflectAttrVal
* Description: This macro is used to declare reflectable attributes and set default value.
*/
#define LIWReflectAttrVal(type, name, val)	\
type name = val;							\
__LIWRegisterReflectAttr(type, name)

/* Define some macros to make things easier */
#define LIWReflectInt(name) LIWReflectAttr(int, name)
#define LIWReflectBool(name) LIWReflectAttr(bool, name)
#define LIWReflectFloat(name) LIWReflectAttr(float, name)
#define LIWReflectDouble(name) LIWReflectAttr(double, name)
#define LIWReflectString(name) LIWReflectAttr(std::string, name)

#define LIWReflectIntVal(name, val) LIWReflectAttrVal(int, name, val)
#define LIWReflectBoolVal(name, val) LIWReflectAttrVal(bool, name, val)
#define LIWReflectFloatVal(name, val) LIWReflectAttrVal(float, name, val)
#define LIWReflectDoubleVal(name, val) LIWReflectAttrVal(double, name, val)
#define LIWReflectStringVal(name, val) LIWReflectAttrVal(std::string, name, val)


//
// Type Reflection
//

/*Set up class hierarchy*/
#define __LIW_REFLECT_SET_UP_BASE(className, classBase) \
const LIW::LIWReflectHelper __##className = LIW::LIWReflectHelper::__SetUpBaseType<classBase, className>(); \

/*Set up dynamic type info*/
/*Note: Don't know if there's a better way to do this.*/
#define __LIW_REFLECT_GET_TYPE_BLOCK(className) \
virtual const LIW::LIWTypeID GetObjectTypeID() const override { return LIW::GetTypeID<className>(); } \
virtual const std::string GetObjectTypeName() const override { return LIW::GetTypeName<className>(); } \
virtual const LIW::LIWBaseType* GetObjectType() const override { return LIW::GetType<className>(); }

/*
* Macro: LIW_REFLECT_CLASS_INHERIT_BEG, LIW_REFLECT_CLASS_INHERIT_END
* Description:	This pair of macros are used to define a class (className) capable of using LIW Reflection
*				derived from a base class (classBase).
*				eg.
*				LIW_REFLECT_CLASS_INHERIT_BEG(MyClassDerive, MyClassBase)
*				// Good-old-normal class definition ...
*				// Or not-so-good-old-normal LIWReflectAttr attribute definition ...
*				LIW_REFLECT_CLASS_INHERIT_END
*/
#define LIW_REFLECT_CLASS_INHERIT_BEG(className, classBase) \
static_assert(std::is_base_of_v<LIW::LIWObject, classBase>, "Base class has to be derived from LIWObject to support LIW Reflection."); \
class className: public classBase { \
public: \
	using typeBase = classBase; \
	using type = className; \
private: \
	__LIW_REFLECT_SET_UP_BASE(className, classBase) \
public: \
	__LIW_REFLECT_GET_TYPE_BLOCK(className) \
private: 
#define LIW_REFLECT_CLASS_INHERIT_END };

/*
* Macro: LIW_REFLECT_CLASS, LIW_REFLECT_CLASS_END
* Description:	This pair of macros are used to define a class capable of using LIW Reflection
*				derived from LIWObject.
*				eg.
*				LIW_REFLECT_CLASS(MyClassDerive, MyClassBase)
*				// Good-old-normal class definition ...
*				// Or not-so-good-old-normal LIWReflectAttr attribute definition ...
*				LIW_REFLECT_CLASS_END
*/
#define LIW_REFLECT_CLASS(className) LIW_REFLECT_CLASS_INHERIT_BEG(className, LIW::LIWObject)
#define LIW_REFLECT_CLASS_END LIW_REFLECT_CLASS_INHERIT_END


#define LIW_REFLECT_TYPE(className) className::classType
#define LIW_REFLECT_BASETYPE(className) className::baseType

namespace LIW {

	/*
	* Class: LIWReflectMember
	* Author: tigerccx
	* Description:	This class is used to represent a reflectable attribute,
	*				which record the name and offset from class instance pointer.
	*/
	class LIWReflectMember {
	public:
		LIWReflectMember() = default;
		LIWReflectMember(const std::string& name, const std::string type, int offset) :
			m_name(name), m_type(type), m_offset(offset) {}
		LIWReflectMember(const LIWReflectMember& other) = default;
		LIWReflectMember(LIWReflectMember&& other) = default;
		LIWReflectMember& operator=(const LIWReflectMember& other) = default;
		LIWReflectMember& operator=(LIWReflectMember&& other) = default;

		bool operator<(const LIWReflectMember& rec) const { return m_name < rec.m_name; }
		bool operator>(const LIWReflectMember& rec) const { return m_name > rec.m_name; }
		bool operator==(const LIWReflectMember& rec) const { return m_name == rec.m_name; }

		std::string m_name;
		std::string m_type;
		int m_offset;
	};

	typedef std::set<LIWReflectMember> ReflectAttrBook;

	/*
	* Class: LIWReflectHelper
	* Author: tigerccx
	* Decription:	A helper class to register, set and get reflectable members.
	*/
	class LIWReflectHelper {
	public:
		//LIWReflectHelper(LIWObject* object, void* data, const std::string& type, const std::string& name) {
		//	std::string objectTypeName = object->GetObjectTypeName();
		//	auto& memSet = record[objectTypeName];
		//	memSet.insert(std::move(LIWReflectMember(name, type,(char*)(data)-(char*)(object))));
		//}
		LIWReflectHelper() {};
		
		template<class Base, class Derived>
		static LIWReflectHelper __SetUpBaseType() {
			LIWType<Derived>::type.__SetBase<Base>();
			return std::move(LIWReflectHelper());
		}

		static LIWReflectHelper __Record(LIWObject* object, void* data, const std::string& type, const std::string& name) {
			std::string objectTypeName = object->GetObjectTypeName();
			auto& memSet = record[objectTypeName];
			memSet.insert(std::move(LIWReflectMember(name, type, (char*)(data)-(char*)(object))));
			return std::move(LIWReflectHelper());
		}

		static std::map<std::string, ReflectAttrBook> record;

		/*
		* Func: Set
		* Description:	Set reflectable attribute.
		*/
		template<class T> static bool Set(LIWObject* object, const std::string& memberName, const T& value) {
			return Set(object, object->GetObjectType(), memberName, value);
		}
		template<class T> static bool Set(void* object, const LIWBaseType* type, const std::string& memberName, const T& value) {
			LIWReflectMember member;
			const LIWBaseType* curType = type;
			bool succeed = false;
			while (curType != nullptr) { //Going through hierarchy
				const std::string& objectTypeName = curType->GetName();
				succeed = GetReflectMember<T>(objectTypeName, memberName, member);
				if (succeed) {
					break;
				}
				curType = curType->GetBase();
			}
			if (succeed) {
				*(T*)((char*)(object)+member.m_offset) = value;
				return true; //Found
			}
			else
			{
				return false; //Not found
			}
		}

		/*
		* Func: Get
		* Description:	Get reflectable attribute value.
		*/
		template<class T> static bool Get(const LIWObject* object, const std::string& memberName, T& value) {
			return Get(object, object->GetObjectType(), memberName, value);
		}
		template<class T> static bool Get(const void* object, const LIWBaseType* type, const std::string& memberName, T& value) {
			LIWReflectMember member;
			const LIWBaseType* curType = type;
			bool succeed = false;
			while (curType != nullptr) { //Going through hierarchy
				const std::string& objectTypeName = curType->GetName();
				succeed = GetReflectMember<T>(objectTypeName, memberName, member);
				if (succeed) {
					break;
				}
				curType = curType->GetBase();
			}
			if (succeed) {
				value = *(T*)((char*)(object)+member.m_offset);
				return true; //Found
			}
			else
			{
				return false; //Not found
			}
		}

		/*
		* Func: GetPt
		* Description:	Get pointer to reflectable attribute value.
		*/
		template<class T> static T* GetPt(const LIWObject* object, const std::string& memberName) {
			return GetPt(object, object->GetObjectType(), memberName);
		}
		template<class T> static T* GetPt(const void* object, const LIWBaseType* type, const std::string& memberName) {
			LIWReflectMember member;
			const LIWBaseType* curType = type;
			bool succeed = false;
			while (curType != nullptr) { //Going through hierarchy
				const std::string& objectTypeName = curType->GetName();
				succeed = GetReflectMember<T>(objectTypeName, memberName, member);
				if (succeed) {
					break;
				}
				curType = curType->GetBase();
			}
			if (succeed) {
				return (T*)((char*)(object)+member.m_offset); //Found
			}
			else
			{
				return nullptr; //Not found
			}
		}

		/*
		* Func: GetVoidPt
		* Description:	Get void pointer to reflectable attribute value.
		*/
		inline static void* GetPtVoid(const LIWObject* object, const std::string& memberName) {
			return GetPtVoid(object, object->GetObjectType(), memberName);
		}
		static void* GetPtVoid(const void* object, const LIWBaseType* type, const std::string& memberName) {
			LIWReflectMember member;
			const LIWBaseType* curType = type;
			bool succeed = false;
			while (curType != nullptr) { //Going through hierarchy
				const std::string& objectTypeName = curType->GetName();
				succeed = GetReflectMemberTypeless(objectTypeName, memberName, member);
				if (succeed) {
					break;
				}
				curType = curType->GetBase();
			}
			if (succeed) {
				return (void*)((char*)(object)+member.m_offset); //Found
			}
			else
			{
				return nullptr; //Not found
			}
		}

		/*
		* Func: GetAttrs
		* Description:	Get table to different types of reflectable attributes.
		*/
		static bool GetAttrs(const LIWBaseType* type, ReflectAttrBook& attrs) {
			const std::string& objectTypeName = type->GetName();
			auto itr0 = record.find(objectTypeName);
			if (itr0 == record.end()) return false;
			attrs.insert(itr0->second.begin(), itr0->second.end());
			return true;
		}

		/*
		* Func: GetAttrsR
		* Description:	Get table to different types of reflectable attributes.
		*				recursiveLevel:	How many levels up do you want to search. 
		*								If recursiveLevel==0, GetAttrsRecursive is the same as GetAttrs.
		*								If recursiveLevel==1, GetAttrsRecursive will search in the immediate base class.
		*								By default, recursiveLevel=INT_MAX, allowing the search to go through the whole hierarchy.
		*/
		/*
		* Note: I have not considered the situation where derived class can have members overwritten. (But like, who would do that..?)
		*/
		static bool GetAttrsR(const LIWBaseType* type, ReflectAttrBook& attrs, int recursiveLevel=INT_MAX) {
			int curLevel = recursiveLevel;
			const LIWBaseType* curType = type;
			while (curLevel >= 0 && curType) {
				GetAttrs(curType, attrs);
				--curLevel;
				curType = curType->GetBase();
			}
			if (attrs.size() == 0) {
				return false;
			}
			return true;
		}

	protected:
		/*
		* Func: GetReflectMember
		* Description:	Get a reflectable member.
		*/
		template<class T> static bool GetReflectMember(const std::string& objectTypeName, const std::string& memberName, LIWReflectMember& member) {
			std::string typeName = GetTypeName<T>();
			auto itr0 = record.find(objectTypeName);
			if (itr0 == record.end()) return false;
			auto& mems = itr0->second;
			auto itr1 = mems.find(std::move(LIWReflectMember(memberName, "", 0)));
			if (itr1 == mems.end() || itr1->m_type != typeName) return false;
			member = *itr1;
			return true;
		}

		static bool GetReflectMemberTypeless(const std::string& objectTypeName, const std::string& memberName, LIWReflectMember& member) {
			auto itr0 = record.find(objectTypeName);
			if (itr0 == record.end()) return false;
			auto& mems = itr0->second;
			auto itr1 = mems.find(std::move(LIWReflectMember(memberName, "", 0)));
			if (itr1 == mems.end()) return false;
			member = *itr1;
			return true;
		}
	};
}