#pragma once
#include <typeinfo>


using namespace std;

typedef size_t T5TypeID;

/*
* class: LIWComponentType
* author: tigerccx
* description:	A class to support dynamic type check. 
*/
class LIWComponentType {
public:
	const T5TypeID m_id = 0;
	inline bool operator==(LIWComponentType& other) { return m_id == other.m_id; }

protected:
	LIWComponentType(const T5TypeID id) : m_id(id) {}
	LIWComponentType(const LIWComponentType& other) = delete;
	LIWComponentType(LIWComponentType&& other) = delete;
	LIWComponentType& operator=(const LIWComponentType& other) = delete;
	LIWComponentType& operator=(LIWComponentType&& other) = delete;
};

/*
* class: LIW__ComponentType<T>
* author: tigerccx
* description:	A template to create a dynamic type instance for type T.
*/
template<class T>
class LIW__ComponentType :
	public LIWComponentType
{
public:
	static LIW__ComponentType<T> type;
protected:
	LIW__ComponentType() :
		LIWComponentType(typeid(T).hash_code()) {}
};

template<class T> LIW__ComponentType<T>  LIW__ComponentType<T>::type;

template<class T>
LIWComponentType& GetType() { return LIW__ComponentType<T>::type; }