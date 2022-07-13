#pragma once
#include <typeinfo>
#include <string>

namespace LIW {
	typedef size_t LIWTypeID;
	
	class LIWBaseType {
	public:
		virtual const LIWTypeID& GetID() const = 0;
		virtual const std::string& GetName() const = 0;
		const LIWBaseType* GetBase() const { return baseType; }
		void __SetBase(LIWBaseType* base) { baseType = base; }
	protected:
		LIWBaseType* baseType = nullptr;
	};

	template<class T>
	class LIWType :
		public LIWBaseType
	{
	public:
		static LIWType type;
		const LIWTypeID& GetID() const override { return m_id; }
		const std::string& GetName() const override { return m_name; }
		template<class T> void __SetBase() { LIWBaseType::__SetBase(&LIWType<T>::type); }
	protected:
		LIWType() {
			m_name = typeid(std::decay_t<T>).name();
			m_id = typeid(std::decay_t<T>).hash_code();
		}
		std::string m_name;
		LIWTypeID m_id;
	};
	template<class T> LIWType<T> LIWType<T>::type;
	
	/*
	* Func: GetBase
	* Description: Get base type (LIWBaseType) of type T
	*/
	template<class T>const LIWBaseType* GetBase() {
		return LIWType<std::decay_t<T>>::type.GetBase();
	}

	/*
	* Func: GetType
	* Description: Get type (LIWBaseType) of type T
	*/
	template<class T>const LIWBaseType* GetType() {
		return &LIWType<std::decay_t<T>>::type;
	}

	/*
	* Func: GetTypeID
	* Description: Get type id of type T
	*/
	template<class T> const LIWTypeID& GetTypeID() {
		return LIWType<std::decay_t<T>>::type.GetID();
	}

	/*
	* Func: GetTypeName
	* Description: Get type name of type T
	*/
	template<class T> const std::string& GetTypeName() {
		return LIWType<std::decay_t<T>>::type.GetName();
	}
}