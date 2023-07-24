#pragma once
#include <stdexcept>

#include "LIWConfig.h"
#include "Memory/LIWMemory.h"
#include "LIWForwardList.h"
#include "LIWArray.h"

namespace LIW {
	template<class T, size_t Size>
	class LIWObjectPool {
	public:
		static const size_t c_size = Size;
	public:
		static const uint32_t sc_handleValid = liw_c_nullobjhdl - 1;
	private:
		struct PoolObject {
			liw_objhdl_type m_offsetNextFree;
			T m_object;
		};
	public:
		template<class ... Args>
		LIWObjectPool(Args&&... args) :
			m_size{ Size }
		{
			for (uint32_t i = 0; i < Size; i++) {
				m_poolObjects[i] = PoolObject{ i + 1 , T(std::forward<Args>(args)...) };
			}
			m_poolObjects[Size - 1].m_offsetNextFree = liw_c_nullobjhdl;
			m_freeList = 0;
		}
		// Copy construct is not allowed for pool
		LIWObjectPool(const LIWObjectPool& other) = delete;
		LIWObjectPool(LIWObjectPool&& other) = default;
		LIWObjectPool& operator=(const LIWObjectPool& other) = delete;
		LIWObjectPool& operator=(LIWObjectPool&& other) = default;

		~LIWObjectPool() {
#ifdef LIW_DEBUG
			if (m_size != c_size)
				printf("WARNING: not all objects has been returned to pool. \n");
#endif
		}

		//
		// Compare
		// No compare: comparison doesn't make sense...

		//
		// Capacity
		//
		inline size_t get_capacity() const {
			return c_size;
		}
		inline size_t get_size() const {
			return m_size;
		}
		inline bool is_empty() const {
			return m_size == 0;// m_poolObjects.get_capacity();
		}

		//
		// Access
		// 

		//
		// Modifer
		//
		inline LIWPointer<T, LIWMem_System> fetch_object() {
			assert(m_freeList != liw_c_nullobjhdl); // Pool is all occupied
			const liw_objhdl_type handle = m_freeList;
			m_freeList = m_poolObjects[handle].m_offsetNextFree;
			m_poolObjects[handle].m_offsetNextFree = sc_handleValid;
			auto ptrObject = LIWPointer<T, LIWMem_System>((liw_hdl_type)(&(m_poolObjects[handle].m_object)));
			m_size--;
			return ptrObject;
		}

		inline void return_object(LIWPointer<T, LIWMem_System> ptrObject) {
			char* ptrRaw = (char*)ptrObject.get_handle();
			ptrRaw -= (ptrdiff_t)(offsetof(PoolObject, m_object));
			liw_objhdl_type handle = (liw_objhdl_type)((PoolObject*)ptrRaw - m_poolObjects.get_data());
			assert(handle < get_capacity()); // Handle does not belong to pool
			assert(is_handle_valid(handle)); // Handle must be valid
			m_poolObjects[handle].m_offsetNextFree = m_freeList;
			m_freeList = handle;
			m_size++;
		}

	private:
		inline bool is_handle_valid(liw_objhdl_type handle) {
			return m_poolObjects[handle].m_offsetNextFree == sc_handleValid;
		}

	private:
		LIWArray<PoolObject, Size> m_poolObjects;
		liw_objhdl_type m_freeList{ liw_c_nullobjhdl };
		uint32_t m_size{ 0 };
	};


	template<class T, size_t Size, LIWMemAllocation AllocType = LIWMem_Default>
	class LIWObjectPool_Heap {
	public:
		static const size_t c_size = Size;
	public:
		static const uint32_t sc_handleValid = liw_c_nullobjhdl - 1;
	private:
		struct PoolObject {
			liw_objhdl_type m_offsetNextFree;
			T m_object;
		};
	public:
		template<class ... Args>
		LIWObjectPool_Heap(Args&&... args) :
			m_size{ Size }
		{
			for (uint32_t i = 0; i < Size; i++) {
				m_poolObjects[i] = PoolObject{ i + 1 , T(std::forward<Args>(args)...) };
			}
			m_poolObjects[Size - 1].m_offsetNextFree = liw_c_nullobjhdl;
			m_freeList = 0;
		}
		// Copy construct is not allowed for pool
		LIWObjectPool_Heap(const LIWObjectPool_Heap& other) = delete;
		LIWObjectPool_Heap(LIWObjectPool_Heap&& other) = default;
		LIWObjectPool_Heap& operator=(const LIWObjectPool_Heap& other) = delete;
		LIWObjectPool_Heap& operator=(LIWObjectPool_Heap&& other) = default;

		~LIWObjectPool_Heap() {
#ifdef LIW_DEBUG
			if (m_size != c_size)
				printf("WARNING: not all objects has been returned to pool. \n");
#endif
		}

		//
		// Compare
		// No compare: comparison doesn't make sense...

		//
		// Capacity
		//
		inline size_t get_capacity() const {
			return c_size;
		}
		inline size_t get_size() const {
			return m_size;
		}
		inline bool is_empty() const {
			return m_size == 0;// m_poolObjects.get_capacity();
		}

		//
		// Access
		// 

		//
		// Modifer
		//
		inline LIWPointer<T, AllocType> fetch_object() {
			assert(m_freeList != liw_c_nullobjhdl); // Pool is all occupied
			const liw_objhdl_type handle = m_freeList;
			m_freeList = m_poolObjects[handle].m_offsetNextFree;
			m_poolObjects[handle].m_offsetNextFree = sc_handleValid;
			auto ptrObject = LIWPointer<T, AllocType>(m_poolObjects.get_ptr().get_handle(), handle * sizeof(PoolObject) + offsetof(PoolObject, m_object));
			m_size--;
			return ptrObject;
		}

		inline void return_object(LIWPointer<T, AllocType> ptrObject) {
			liw_hdl_type handleObject = liw_moffset<AllocType>(ptrObject.get_handle(), -(ptrdiff_t)offsetof(PoolObject, m_object));
			liw_objhdl_type handle = (liw_objhdl_type)(liw_mdiff<AllocType>(handleObject, m_poolObjects.get_ptr().get_handle())/sizeof(PoolObject));
			assert(handle < get_capacity()); // Handle does not belong to pool
			assert(is_handle_valid(handle)); // Handle must be valid
			m_poolObjects[handle].m_offsetNextFree = m_freeList;
			m_freeList = handle;
			m_size++;
		}

	private:
		inline bool is_handle_valid(liw_objhdl_type handle) {
			return m_poolObjects[handle].m_offsetNextFree == sc_handleValid;
		}

	private:
		LIWArray_Heap<PoolObject, Size, AllocType> m_poolObjects;
		liw_objhdl_type m_freeList{ liw_c_nullobjhdl };
		uint32_t m_size{ 0 };
	};
}