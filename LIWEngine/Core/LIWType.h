#pragma once
#include <typeinfo>
#include <string>
#include <iostream>
#include <set>

namespace LIW {
	class LIWObject;

	/*
	* Class: LIWReflectMember
	* Author: tigerccx
	* Description:	This class is used to represent a reflectable attribute,
	*				which record the name and offset from class instance pointer.
	*/
	class LIWReflectMember {
	public:
		LIWReflectMember() = default;
		LIWReflectMember(const std::string& name, const std::string type, size_t offset) :
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
		size_t m_offset{ 0 };
	};

	typedef std::set<LIWReflectMember> LIWReflectAttrBook;

	typedef size_t LIWTypeID;
	
	class LIWBaseType {
	public:
		inline virtual const LIWTypeID& GetID() const = 0;
		inline virtual const std::string& GetName() const = 0;
		inline LIWBaseType* GetBase() { return baseType; }
		void __SetBase(LIWBaseType* base) { baseType = base; m_book.insert(base->m_book.begin(), base->m_book.end()); }
	protected:
		LIWBaseType* baseType = nullptr;
		LIWReflectAttrBook m_book;

	public:
		template<class Base>
		const short __SetUpBaseType();

		const short __Record(LIWObject* object, void* data, const std::string& type, const std::string& name) {
			m_book.insert(std::move(LIWReflectMember(name, type, (char*)(data)-(char*)(object))));
			return 0;
		}

	public:
		/*
		* Func: Set
		* Description:	Set reflectable attribute.
		*/
		template<class T> bool Set(void* object, const std::string& memberName, const T& value) {
			LIWReflectMember member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return false;
			}
			*(T*)((char*)(object)+member.m_offset) = value;
			return true;
		}

		/*
		* Func: Get
		* Description:	Get reflectable attribute value.
		*/
		template<class T> bool Get(const void* object, const std::string& memberName, T& value) {
			LIWReflectMember member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return false;
			}
			value = *(T*)((char*)(object)+member.m_offset);
			return true;
		}

		/*
		* Func: GetPt
		* Description:	Get pointer to reflectable attribute value.
		*/
		template<class T> T* GetPt(const void* object, const std::string& memberName) {
			LIWReflectMember member;
			if (!this->GetReflectMember<T>(memberName, member)) {
				return nullptr;
			}
			return (T*)((char*)(object)+member.m_offset);
		}

		/*
		* Func: GetVoidPt
		* Description:	Get void pointer to reflectable attribute value.
		*/
		void* GetPtVoid(const void* object, const std::string& memberName) {
			LIWReflectMember member;
			if (!this->GetReflectMemberTypeless(memberName, member)) {
				return nullptr;
			}
			return (void*)((char*)(object)+member.m_offset);
		}

		/*
		* Func: GetAttrs
		* Description:	Get table to different types of reflectable attributes.
		*/
		LIWReflectAttrBook& GetAttrs() {
			return m_book;
		}

	protected:
		/*
		* Func: GetReflectMember
		* Description:	Get a reflectable member.
		*/
		template<class T> bool GetReflectMember(const std::string& memberName, LIWReflectMember& member);

		bool GetReflectMemberTypeless(const std::string& memberName, LIWReflectMember& member) {
			auto itr1 = m_book.find(std::move(LIWReflectMember(memberName, "", 0)));
			if (itr1 == m_book.end()) return false;
			member = *itr1;
			return true;
		}
	};

	template<class T>
	class LIWType :
		public LIWBaseType
	{
	public:
		static LIWType type;
		inline const LIWTypeID& GetID() const override { return m_id; }
		inline const std::string& GetName() const override { return m_name; }
		template<class T> void __SetBase() { LIWBaseType::__SetBase(&LIWType<T>::type); }
		
	protected:
		LIWType() {
			m_name = typeid(std::decay_t<T>).name();
			m_id = typeid(std::decay_t<T>).hash_code();
			//std::cout << m_name << std::endl;
		}
		std::string m_name;
		LIWTypeID m_id;
	};
	template<class T> LIWType<T> LIWType<T>::type;
	
	/*
	* Func: GetBase
	* Description: Get base type (LIWBaseType) of type T
	*/
	template<class T> inline LIWBaseType* GetBase() {
		return LIWType<std::decay_t<T>>::type.GetBase();
	}

	/*
	* Func: GetType
	* Description: Get type (LIWBaseType) of type T
	*/
	template<class T> inline LIWBaseType* GetType() {
		return &LIWType<std::decay_t<T>>::type;
	}

	/*
	* Func: GetTypeID
	* Description: Get type id of type T
	*/
	template<class T> inline const LIWTypeID& GetTypeID() {
		return LIWType<std::decay_t<T>>::type.GetID();
	}

	/*
	* Func: GetTypeName
	* Description: Get type name of type T
	*/
	template<class T> inline const std::string& GetTypeName() {
		return LIWType<std::decay_t<T>>::type.GetName();
	}

	template<class Base>
	inline const short LIWBaseType::__SetUpBaseType()
	{
		__SetBase(&LIWType<Base>::type);
		return 0;
	}

	template<class T> bool LIWBaseType::GetReflectMember(const std::string& memberName, LIWReflectMember& member)
	{
		std::string typeName = GetTypeName<T>();
		auto itr1 = m_book.find(std::move(LIWReflectMember(memberName, "", 0)));
		if (itr1 == m_book.end() || itr1->m_type != typeName) return false;
		member = *itr1;
		return true;
	}
}