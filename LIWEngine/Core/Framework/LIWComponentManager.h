#pragma once
#include <vector>
#include <cassert>
#include <functional>

#include "LIWComponent.h"

/*
* class: LIWComponentManager<T>
* author: tigerccx
* description:	LIWComponentManager<T> is used to hold all type T components. 
*				It can offer systems API to iterate through all of the same type of components. 
*/
template<class T>
class LIWComponentManager
{
	static_assert(std::is_base_of_v<LIWComponent, T>, "Only components can use LIWComponentManager");
public:
	LIWComponentManager() { }
	~LIWComponentManager() {
		for (auto itr = m_list.begin(); itr != m_list.end(); ++itr) {
			MarkToRemove(*itr);
		}
		ApplyChange();
	}

	//
	// Modification
	//
	//Mark component to be removed. Component marked will be ignored by ApplyIteration.
	inline void MarkToRemove(T* component) { component->MarkToRemove(); }
	//Mark component to be added. Component marked will be added during ApplyChange.
	void MarkToDeactivate(T* component) { component->MarkToDeactivate(); }
	//Apply marked changes. 
	void ApplyChange() {
		for (auto itr = m_list.begin(); itr != m_list.end();) {
			if ((*itr)->m_isDeleting) {
				delete* itr;
				itr = m_list.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		for (auto itr = m_listAddBuffer.begin(); itr != m_listAddBuffer.end();) {
			m_list.emplace_back(*itr);
			itr = m_listAddBuffer.erase(itr);
		}
	}
	//Add component to manager.
	void AddObj(T* component) {
		m_list.emplace_back(component);
	}
	//Remove component to manager.
	void RemoveObj(T* component) {
		auto itr = std::find(m_list.begin(), m_list.end(), component);
		assert(itr != m_list.end());
		m_list.erase(itr);
	}

	//
	// Capacity
	//
	size_t GetComponentCount() const { return m_list.size(); }

	//
	// Access
	//
	auto GetIterBeg() { return m_list.begin(); }
	auto GetCIterBeg() const { return m_list.begin(); }
	auto GetIterEnd() { return m_list.end(); }
	auto GetCIterEnd() const { return m_list.end(); }

	std::vector<T*> m_list;

protected:
	std::vector<T*> m_listAddBuffer;
public:
	static LIWComponentManager<T> s_ins;
};

template<class T>
LIWComponentManager<T> LIWComponentManager<T>::s_ins;