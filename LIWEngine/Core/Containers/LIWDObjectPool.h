#pragma once
#include <stdexcept>

#include "Memory/LIWMemory.h"
#include "LIWDArray.h"

namespace LIW {
	template<class T, LIWMemAllocation AllocType = LIWMem_Default>
	class LIWDObjectPool {
	public:
		static const uint32_t sc_handleValid = liw_c_nullobjhdl - 1;
	private:
		struct PoolObject {
			liw_objhdl_type m_offsetNextFree;
			T m_object;
		};
	public:
		template<class ... Args>
		LIWDObjectPool(uint32_t initCapacity, Args&&... args) :
			m_objects{ initCapacity }, m_size{ initCapacity }
		{
			for (uint32_t i = 0; i < initCapacity; i++) {
				m_objects.push_back(PoolObject{ i + 1 , T(std::forward<Args>(args)...) });
			}
			m_objects[initCapacity - 1].m_offsetNextFree = liw_c_nullobjhdl;
			m_freeList = 0;
		}
		// Copy construct is not allowed for pool
		LIWDObjectPool(const LIWDObjectPool& other) = delete; 
		LIWDObjectPool(LIWDObjectPool&& other) = default;
		LIWDObjectPool& operator=(const LIWDObjectPool& other) = delete;
		LIWDObjectPool& operator=(LIWDObjectPool&& other) = default;

		~LIWDObjectPool() {
#ifdef _DEBUG
			if (m_size != (uint32_t)m_objects.get_capacity())
				printf("WARNING: not all objects has been returned to pool. \n");
#endif
		}

		//
		// Compare
		// No compare: no copy is allowed

		//
		// Capacity
		//
		inline size_t get_capacity() const {
			return m_objects.get_capacity();
		}
		inline size_t get_size() const {
			return m_size;
		}
		inline bool is_empty() const {
			return m_size == 0;// m_objects.get_capacity();
		}

		//
		// Access
		// 
		inline T& get_object(liw_objhdl_type handle) {
			return m_objects[handle].m_object;
		}
		inline bool is_handle_valid(liw_objhdl_type handle) {
			return m_objects[handle].m_offsetNextFree == sc_handleValid;
		}

		//
		// Modifer
		//
		template<class ... Args>
		void set_capacity(uint32_t capacity, Args&&... args) { //Not thread safe!
			const uint32_t curCapacity = (uint32_t)m_objects.get_capacity();
			assert(capacity > curCapacity); //Pool can only expand
			m_objects.set_capacity(capacity);
			const uint32_t newCapacity = capacity;
			for (uint32_t i = curCapacity; i < newCapacity; i++) {
				m_objects.push_back(PoolObject{ i + 1 , T(std::forward<Args>(args)...) });
			}
			m_objects[newCapacity - 1].m_offsetNextFree = m_freeList;
			m_size += newCapacity - curCapacity;
			m_freeList = curCapacity;
		}

		inline liw_objhdl_type fetch_object() {
			assert(m_freeList != liw_c_nullobjhdl); // Pool is all occupied
			const liw_objhdl_type handle = m_freeList;
			m_freeList = m_objects[handle].m_offsetNextFree;
			m_objects[handle].m_offsetNextFree = sc_handleValid;
			m_size--;
			return handle;
		}

		inline void return_object(liw_objhdl_type handle) {
			assert(handle < get_capacity()); // Handle does not belong to pool
			assert(is_handle_valid(handle)); // Handle must be valid
			m_objects[handle].m_offsetNextFree = m_freeList;
			m_freeList = handle;
			m_size++;
		}

	private:
		LIWDArray<PoolObject, AllocType> m_objects;
		liw_objhdl_type m_freeList{ liw_c_nullobjhdl };
		uint32_t m_size{ 0 };
	};
}