#pragma once
#include <mutex>
#include <cassert>

#include "LIWTypes.h"
#include "LIWConstants.h"
#include "Memory/LIWMemory.h"

enum LIWDArrayExpandType {
	LIWDArrayExpand_Double,		// 2x capacity
	LIWDArrayExpand_Half,		// 1.5x capacity
	LIWDArrayExpand_Quater,		// 1.25x capacity
	LIWDArrayExpand_Constant,	// by a constant number
	LIWDArrayExpand_Max
};

template<class T, LIWMemAllocation AllocType = LIWMem_Default, 
	LIWDArrayExpandType ExpandType = LIWDArrayExpand_Half>
class LIWDArray {
public:
	typedef LIWDArray<T, AllocType, ExpandType> this_type;
	typedef std::mutex mtx_type;
	typedef std::lock_guard<mtx_type> lkgd_type;
public:
	class CIterator {
		friend class this_type;
	public:
		CIterator(const this_type& container, size_t idx = 0) : m_container(&container), m_idxCur(idx) { }
		CIterator(const CIterator& other) = default;
		CIterator(CIterator&& other) = default;
		CIterator& operator=(const CIterator& other) = default;
		CIterator& operator=(CIterator && other) = default;

		//
		// Compare
		//
		inline bool operator==(const CIterator& other) const {
			assert(m_container == other.m_container);
			return m_idxCur == other.m_idxCur;
		}
		inline bool operator!=(const CIterator& other) const {
			return !(*this == other);
		}
		inline bool operator<(const CIterator& other) const {
			assert(m_container == other.m_container);
			return m_idxCur < other.m_idxCur;
		}
		inline bool operator>(const CIterator& other) const {
			assert(m_container == other.m_container);
			return m_idxCur > other.m_idxCur;
		}
		inline bool operator<=(const CIterator& other) const {
			return !(*this > other);
		}
		inline bool operator>=(const CIterator& other) const {
			return !(*this < other);
		}
		
		//
		// Step
		//
		inline CIterator& operator++() {
			assert(m_idxCur < m_container->get_size());
			m_idxCur++;
			return *this;
		}
		inline CIterator operator++(int) {
			assert(m_idxCur < m_container->get_size());
			CIterator tmp{ *this };
			m_idxCur++;
			return tmp;
		}
		inline CIterator& operator--() {
			assert(m_idxCur > 0);
			m_idxCur--;
			return *this;
		}
		inline CIterator operator--(int) {
			assert(m_idxCur > 0);
			CIterator tmp{ *this };
			m_idxCur--;
			return tmp;
		}
		
		//
		// Access
		//
		inline const T& operator*() {
			return (*m_container)[m_idxCur];
		}
		inline const T& operator*() const {
			return (*m_container)[m_idxCur];
		}
		inline const T* operator->() {
			return &(*m_container)[m_idxCur];
		}
		inline const T* operator->() const {
			return &(*m_container)[m_idxCur];
		}

	private:
		size_t m_idxCur = 0;
		const this_type* m_container;
	};

	class Iterator : public CIterator {
	public:
		Iterator(this_type& container, size_t idx = 0) : CIterator(container, idx) { }
		Iterator(const Iterator& other) = default;
		Iterator(Iterator&& other) = default;
		Iterator& operator=(const Iterator& other) = default;
		Iterator& operator=(Iterator&& other) = default;

		//
		// Access
		//
		inline T& operator*() {
			return const_cast<T&>(CIterator::operator*());
		}
		inline const T& operator*() const {
			return CIterator::operator*();
		}
		inline T* operator->() {
			return const_cast<T*>(CIterator::operator->());
		}
		inline const T* operator->() const {
			return CIterator::operator->();
		}
	};

public:
	LIWDArray(size_t capacity = 4): m_capacity(capacity), m_size(0) {
		assert(capacity != 0); // Cannot allocate a DArray with 0 capacity. 
		m_dataBuffer = liw_malloc<AllocType>(m_capacity * sizeof(T));
	}
	template<class ... Args>
	LIWDArray(size_t capacity, size_t size, Args&&... args): m_capacity(capacity), m_size(size) {
		assert(capacity != 0); // Cannot allocate a DArray with 0 capacity. 

		m_dataBuffer = liw_malloc<AllocType>(m_capacity * sizeof(T));
		T* ptr = get_data();
		for (size_t i = 0; i < m_size; i++) {
			new (&ptr[i]) T(std::forward<Args>(args)...);
		}
	}
	LIWDArray(const LIWDArray& other) {
		*this = other;
	}
	LIWDArray(LIWDArray&& other) = default;
	LIWDArray& operator=(const LIWDArray& other) {
		if (m_dataBuffer != liw_c_nullhdl) {
			destroy_elements();
			liw_free<AllocType>(m_dataBuffer);
		}
		assert(other.m_capacity != 0); // Cannot copy a DArray with 0 capacity. NOTE: This shouldn't be possible
		m_dataBuffer = liw_malloc<AllocType>(other.m_capacity * sizeof(T));
		m_capacity = other.m_capacity;
		void* ptrDst = get_data();
		const void* ptrOther = other.get_data();
		memcpy_s(ptrDst, m_capacity * sizeof(T), ptrOther, other.m_capacity * sizeof(T));
		m_size = other.m_size;
		return *this;
	}
	LIWDArray& operator=(LIWDArray&& other) = default;

	void init_expand_size(size_t sizeExpand) {
		m_sizeExpand = sizeExpand;
	}

	~LIWDArray() {
		destroy_elements();
		liw_free<AllocType>(m_dataBuffer);
	}

	//
	// Compare
	//
	inline bool operator==(const LIWDArray& other) const {
		return m_dataBuffer == other.m_dataBuffer;
	}
	inline bool operator!=(const LIWDArray& other) const {
		return !(*this == other);
	}

	//
	// Capacity
	//
	inline size_t get_capacity() const {
		return m_capacity;
	}
	inline size_t get_size() const {
		return m_size;
	}
	inline size_t is_empty() const {
		return !m_size;
	}

	//
	// Access
	//
	inline liw_hdl_type get_data_handle() const {
		return m_dataBuffer;
	}
	inline T* get_data() const {
		return (T*)liw_maddr<AllocType>(m_dataBuffer);
	}
	inline T& operator[](size_t idx) {
		T* ptr = get_data();
		return ptr[idx];
	}
	inline const T& operator[](size_t idx) const {
		const T* ptr = (const T*)get_data();
		return ptr[idx];
	}

	//
	// Iterator
	//
	inline Iterator get_beg() {
		return Iterator(*this, 0);
	}
	inline Iterator get_end() {
		return Iterator(*this, m_size);
	}
	inline CIterator get_cbeg() const {
		return CIterator(*this, 0);
	}
	inline CIterator get_cend() const {
		return CIterator(*this, m_size);
	}

	//
	// Modifer
	//
	void set_capacity(size_t capacity) {
		assert(capacity >= m_size); // Cannot set capacity smaller than size. 
		if (capacity == m_capacity)
			return;
		liw_hdl_type handleNew = liw_malloc<AllocType>(capacity * sizeof(T));
		void* ptrDst = liw_maddr<AllocType>(handleNew);
		void* ptrOrg = get_data();
		memcpy_s(ptrDst, capacity * sizeof(T), ptrOrg, m_size * sizeof(T));
		liw_free<AllocType>(m_dataBuffer);
		m_dataBuffer = handleNew;
		m_capacity = capacity;
	}
	inline void set_capacity_thds(size_t capacity) {
		lkgd_type lk(m_mtx);
		set_capacity(capacity);
	}

	inline void push_back(const T& val) {
		const size_t sizeNew = m_size + 1;
		if (m_capacity < sizeNew) {
			expand();
		}
		T* ptr = get_data();
		new (&ptr[m_size]) T(std::forward<const T>(val));
		m_size = sizeNew;
	}
	inline void push_back(T&& val) {
		const size_t sizeNew = m_size + 1;
		if (m_capacity < sizeNew) {
			expand();
		}
		T* ptr = get_data();
		new (&ptr[m_size]) T(std::forward<T>(val));
		m_size = sizeNew;
	}
	inline void push_back_thds(const T& val) {
		lkgd_type lk(m_mtx);
		push_back(std::forward<const T>(val));
	}
	inline void push_back_thds(T&& val) {
		lkgd_type lk(m_mtx);
		push_back(std::forward<T>(val));
	}

	inline void pop_back() {
		assert(m_size != 0); // Cannot pop from an empty DArray. 
		const size_t sizeNew = m_size - 1;
		T* ptr = get_data();
		ptr[m_size].~T();
		m_size = sizeNew;
	}
	inline void pop_back_thds() {
		lkgd_type lk(m_mtx);
		pop_back();
	}

	Iterator insert(const Iterator& pos, const T& val) {
		assert(pos.m_container == this); // pos must be from this container. 
		assert(pos.m_idxCur <= m_size); // pos invalid. 
		const size_t sizeNew = m_size + 1;
		if (m_capacity < sizeNew) {
			expand();
		}
		T* ptr = get_data();
		const size_t idx = pos.m_idxCur;
		T* ptrSrc = &ptr[idx];
		T* ptrDst = &ptr[idx + 1];
		size_t size = m_size - idx;
		memmove_s(ptrDst, (m_capacity - idx - 1) * sizeof(T), ptrSrc, size * sizeof(T));
		new (ptrSrc) T(std::forward<const T>(val));
		m_size = sizeNew;
		return Iterator(*this, idx);
	}
	Iterator insert(const Iterator& pos, T&& val) {
		assert(pos.m_container == this); // pos must be from this container. 
		assert(pos.m_idxCur <= m_size); // pos invalid. 
		const size_t sizeNew = m_size + 1;
		if (m_capacity < sizeNew) {
			expand();
		}
		T* ptr = get_data();
		const size_t idx = pos.m_idxCur;
		T* ptrSrc = &ptr[idx];
		T* ptrDst = &ptr[idx + 1];
		size_t size = m_size - idx;
		memmove_s(ptrDst, (m_capacity - idx - 1) * sizeof(T), ptrSrc, size * sizeof(T));
		new (ptrSrc) T(std::forward<T>(val));
		m_size = sizeNew;
		return Iterator(*this, idx);
	}
	template<class Iterator1>
	Iterator insert(const Iterator& pos, const Iterator1& beg, const Iterator1& end) {
		assert(pos.m_container == this); // pos must be from this container. 
		assert(pos.m_idxCur <= m_size); // pos invalid. 
		assert(beg.m_container == end.m_container); // beg and end must be from the same container. 
		assert(beg.m_idxCur < beg.m_container->get_size()); // beg invalid. 
		assert(end.m_idxCur <= end.m_container->get_size()); // end invalid. 
		assert(beg <= end); // beg must preceed than end. 
		
		size_t sizeInsert = end.m_idxCur - beg.m_idxCur;
		if (sizeInsert == 0) return Iterator(pos);

		const size_t sizeNew = m_size + sizeInsert;
		while (m_capacity < sizeNew) {
			expand();
		}
		T* ptr = get_data();
		const size_t idx = pos.m_idxCur;
		// Move away org
		T* ptrSrc = &ptr[idx];
		T* ptrDst = &ptr[idx + sizeInsert];
		size_t sizeMove = m_size - idx;
		memmove_s(ptrDst, (m_capacity - idx - sizeInsert) * sizeof(T), ptrSrc, sizeMove * sizeof(T));
		// Insert new
		ptrDst = ptrSrc;
		const T* ptrSrc1 = &(*beg);
		memcpy_s(ptrDst, sizeInsert * sizeof(T), ptrSrc1, sizeInsert * sizeof(T));

		m_size = sizeNew;
		return Iterator(*this, idx);
	}
	inline Iterator insert_thds(const Iterator& pos, const T& val) {
		lkgd_type lk(m_mtx);
		return std::move(insert(std::forward<const Iterator>(pos), std::forward<const T>(val)));
	}
	inline Iterator insert_thds(const Iterator& pos, T&& val) {
		lkgd_type lk(m_mtx);
		return std::move(insert(std::forward<const Iterator>(pos), std::forward<T>(val)));
	}
	template<class Iterator1>
	inline Iterator insert_thds(const Iterator& pos, const Iterator1& beg, const Iterator1& end) {
		lkgd_type lk(m_mtx);
		return std::move(insert(std::forward<const Iterator>(pos), std::forward<const Iterator1&>(beg), std::forward<const Iterator1&>(end)));
	}

	Iterator erase(const Iterator& pos) {
		assert(pos.m_container == this); // pos must be from this container. 
		assert(pos.m_idxCur < m_size); // pos invalid. 
		T* ptr = get_data();
		const size_t idx = pos.m_idxCur;
		const size_t size = m_size - idx - 1;
		T* ptrDst = &ptr[idx];
		T* ptrSrc = &ptr[idx + 1];
		ptrDst->~T();
		memmove_s(ptrDst, (m_capacity - idx) * sizeof(T), ptrSrc, size * sizeof(T));
		m_size--;
		return Iterator(*this, idx);
	}
	Iterator erase(const Iterator& beg, const Iterator& end) {
		assert(beg.m_container == this && end.m_container == this); // beg and end must be from this container. 
		assert(beg.m_idxCur < m_size); // beg invalid. 
		assert(end.m_idxCur <= m_size); // end invalid. 
		assert(beg <= end); // beg must preceed than end. 
		
		const size_t idxBeg = beg.m_idxCur;
		const size_t idxEnd = end.m_idxCur;
		size_t sizeErase = idxEnd - idxBeg;
		if(sizeErase==0) return Iterator(beg);

		T* ptr = get_data();
		const size_t size = m_size - idxEnd;
		T* ptrDst = &ptr[idxBeg];
		T* ptrSrc = &ptr[idxEnd];
		for (T* ptr = ptrDst; ptr < ptrSrc; ptr++) {
			ptr->~T();
		}
		memmove_s(ptrDst, (m_capacity - idxBeg) * sizeof(T), ptrSrc, size * sizeof(T));
		m_size -= sizeErase;
		return Iterator(*this, idxBeg);
	}
	Iterator erase_thds(const Iterator& pos) {
		lkgd_type lk(m_mtx);
		return std::move(erase(std::forward<const Iterator>(pos)));
	}
	Iterator erase_thds(const Iterator& beg, const Iterator& end) {
		lkgd_type lk(m_mtx);
		return std::move(erase(std::forward<const Iterator>(beg), std::forward<const Iterator>(end)));
	}

	inline void clear() {
		destroy_elements();
		m_size = 0;
	}
	inline void clear_thds() {
		lkgd_type lk(m_mtx);
		clear();
	}

private:
	inline void destroy_elements() {
		T* ptr = get_data();
		for (size_t i = 0; i < m_size; i++) {
			ptr[i].~T();
		}
	}

	inline void expand() {
		static_assert(ExpandType < LIWDArrayExpand_Max, "Must use a valid LIWDArrayExpandType enum. ");
		size_t capacityNew = m_capacity;
		switch (ExpandType)
		{
		case LIWDArrayExpand_Double:
			capacityNew = 2 * m_capacity; break;
		case LIWDArrayExpand_Half:
			capacityNew = 1.5 * m_capacity; break;
		case LIWDArrayExpand_Quater:
			capacityNew = 1.25 * m_capacity; break;
		case LIWDArrayExpand_Constant: {
				assert(m_sizeExpand != 0); // When ExpandType is LIWDArrayExpand_Constant, m_sizeExpand must be set to non-zero
				capacityNew = m_capacity + m_sizeExpand;
			} break;
		}
		set_capacity(capacityNew);
	}

private:
	liw_hdl_type m_dataBuffer{ liw_c_nullhdl };
	size_t m_capacity{ 0 };
	size_t m_size{ 0 };
	size_t m_sizeExpand{ 4 };
	mutable mtx_type m_mtx;
};