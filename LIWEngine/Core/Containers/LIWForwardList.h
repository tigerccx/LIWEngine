#pragma once

#include "LIWTypes.h"
#include "LIWConstants.h"
#include "Memory/LIWMemory.h"

//TODO: Maybe a preallocated pool is better for this. 

template<class T, LIWMemAllocation AllocType = LIWMem_Default>
class LIWFowardList {
public:
	struct Element;
public:
	typedef LIWFowardList<T, AllocType> this_type;
	typedef LIWPointer<Element, AllocType> element_ptr_type;
private:
	struct Element {
		typename element_ptr_type m_next{ liw_c_nullhdl };
		T m_val;
	};
public:
	class CIterator {
		friend class this_type;
	public:
		CIterator(element_ptr_type ptr) : m_ptrCur(ptr) { }
		CIterator(const CIterator& other) = default;
		CIterator(CIterator&& other) = default;
		CIterator& operator=(const CIterator& other) = default;
		CIterator& operator=(CIterator&& other) = default;

		//
		// Compare
		//
		// Compare ops: <, >, <=, >= are not allowed for List iterator. 
		inline bool operator==(const CIterator& other) const {
			return m_ptrCur == other.m_ptrCur;
		}
		inline bool operator!=(const CIterator& other) const {
			return !(*this == other);
		}

		//
		// Step
		//
		// Step ops: --itr, itr-- is not allowed for ForwardList iterator. 
		inline CIterator& operator++() {
			assert(!m_ptrCur.is_null());
			m_ptrCur = m_ptrCur->m_next;
			return *this;
		}
		inline CIterator operator++(int) {
			assert(!m_ptrCur.is_null());
			CIterator tmp{ *this };
			m_ptrCur = m_ptrCur->m_next;
			return tmp;
		}

		//
		// Access
		//
		inline const T& operator*() {
			return m_ptrCur->m_val;
		}
		inline const T& operator*() const {
			return m_ptrCur->m_val;
		}
		inline const T* operator->() {
			return &(m_ptrCur->m_val);
		}
		inline const T* operator->() const {
			return &(m_ptrCur->m_val);
		}

	private:
		element_ptr_type m_ptrCur{ liw_c_nullhdl };
	};

	class Iterator : public CIterator {
	public:
		Iterator(element_ptr_type ptr) : CIterator(ptr) { }
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
	LIWFowardList() : m_size(0) {
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, T{} }
		);
	}
	template<class ... Args>
	LIWFowardList(size_t size, Args&&... args) : m_size(size) {
		assert(size != 0); // What r u trying to do???
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, std::forward<Args>(args)... }
		);
		element_ptr_type ptrPrev = m_prehead;
		for (size_t i = 0; i < size; i++) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, std::forward<Args>(args)... }
			);
			ptrPrev->m_next = ptr;
			ptrPrev = ptr;
		}
	}
	LIWFowardList(const LIWFowardList& other) {
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, T{} }
		);
		*this = other;
	}
	LIWFowardList(LIWFowardList&& other) {
		*this = std::forward<LIWFowardList>(other);
	}
	LIWFowardList& operator=(const LIWFowardList& other) {
		if (!m_prehead->m_next.is_null()) { //Clean out old data
			destroy_elements();
		}
		m_size = other.m_size;
		element_ptr_type ptrOther = other.m_prehead->m_next;
		element_ptr_type ptrPrev = m_prehead;
		for (size_t i = 0; i < m_size; i++) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, *ptrOther }
			); 
			ptrPrev->m_next = ptr;
			ptrPrev = ptr;
			ptrOther = ptrOther->m_next;
		}
		return *this;
	}
	LIWFowardList& operator=(LIWFowardList&& other) {
		if (!m_prehead.is_null()) {
			if (!m_prehead->m_next.is_null()) { //Clean out old data
				destroy_elements();
			}
			liw_delete<AllocType>(m_prehead);
		}
		m_size = other.m_size;
		m_prehead = other.m_prehead;
		return *this;
	}

	~LIWFowardList() {
		if (!m_prehead->m_next.is_null()) {
			destroy_elements();
		}
		liw_delete<AllocType>(m_prehead);
	}

	//
	// Compare
	//
	inline bool operator==(const LIWFowardList& other) const {
		if (m_prehead == other.m_prehead) {
			return true;
		}
		if (m_size != other.m_size) {
			return false;
		}
		auto ptrThis = m_prehead->m_next;
		auto ptrOther = other.m_prehead->m_next;
		for (size_t i = 0; i < m_size; i++) {
			if (ptrThis->m_val != ptrOther->m_val) {
				return false;
			}
			ptrThis = ptrThis->m_next;
			ptrOther = ptrOther->m_next;
		}
		return true;
	}
	inline bool operator!=(const LIWFowardList& other) const {
		return !(*this == other);
	}

	//
	// Capacity
	//
	inline size_t get_size() const {
		return m_size;
	}
	inline size_t is_empty() const {
		return !m_size;
	}

	//
	// Access
	//
	inline element_ptr_type get_prehead() const {
		return m_prehead;
	}
	inline element_ptr_type get_head() const {
		return m_prehead->m_next;
	}
	inline T get_head_val() const {
		return m_prehead->m_next->m_val;
	}

	//
	// Iterator
	//
	inline Iterator get_prebeg() {
		return Iterator(m_prehead);
	}
	inline Iterator get_beg() {
		return Iterator(m_prehead->m_next);
	}
	inline Iterator get_end() {
		return Iterator(element_ptr_type{ liw_c_nullhdl });
	}
	inline Iterator get_cprebeg() {
		return CIterator(m_prehead);
	}
	inline CIterator get_cbeg() const {
		return CIterator(m_prehead->m_next);
	}
	inline CIterator get_cend() const {
		return CIterator(element_ptr_type{ liw_c_nullhdl });
	}

	//
	// Modifer
	//
	inline void push_front(const T& val) {
		push_front_(std::forward<const T>(val));
	}
	inline void push_front(T&& val) {
		push_front_(std::forward<T>(val));
	}

	inline void pop_front() {
		assert(m_size != 0); // Cannot pop from an empty List. 
		auto ptr = m_prehead->m_next;
		m_prehead->m_next = ptr->m_next;
		liw_delete(ptr);
		m_size--;
	}

	inline Iterator insert_after(Iterator pos, const T& val) {
		return insert_after_(pos, std::forward<const T>(val));
	}
	inline Iterator insert_after(Iterator pos, T&& val) {
		return insert_after_(pos, std::forward<T>(val));
	}
	template<class Iterator1>
	Iterator insert_after(Iterator pos, Iterator1 beg, Iterator1 end) {
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		
		auto ptrPrev = pos.m_ptrCur;
		for (auto itr = beg; itr != end; ++itr) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, *itr }
			);
			ptrPrev->m_next = ptr;
			ptrPrev = ptr;
			m_size++;
		}
		return Iterator(ptrPrev);
	}

	Iterator erase_after(Iterator pos) {
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		assert(!pos.m_ptrCur->m_next.is_null()); // next must be valid
		auto ptr = pos.m_ptrCur->m_next;
		pos.m_ptrCur->m_next = ptr->m_next;
		liw_delete(ptr);
		m_size--;
		return Iterator(pos.m_ptrCur->m_next);
	}
	Iterator erase_after(Iterator beg, Iterator end) {
		assert(!beg.m_ptrCur.is_null()); // beg must be valid
		assert(!beg.m_ptrCur->m_next.is_null()); // next must be valid

		const auto ptrEnd = end.m_ptrCur;
		for (auto ptr = beg.m_ptrCur->m_next; ptr != ptrEnd; ) {
			auto ptrNext = ptr->m_next;
			liw_delete(ptr);
			ptr = ptrNext;
			m_size--;
		}
		beg.m_ptrCur->m_next = ptrEnd;
		return end;
	}

	inline void clear() {
		destroy_elements();
	}

private:
	template<class TVal>
	inline void push_front_(TVal&& val) {
		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, std::forward<TVal>(val) }
		);
		ptr->m_next = m_prehead->m_next;
		m_prehead->m_next = ptr;
		m_size++;
	}

	template<class TVal>
	Iterator insert_after_(Iterator pos, TVal&& val) {
		// Skipping check for if pos is in this container...
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, std::forward<TVal>(val) }
		);
		ptr->m_next = pos.m_ptrCur->m_next;
		pos.m_ptrCur->m_next = ptr;
		m_size++;
		return Iterator(ptr);
	}

	inline void destroy_elements() {
		while (!m_prehead->m_next.is_null())
		{
			auto ptr = m_prehead->m_next;
			m_prehead->m_next = ptr->m_next;
			liw_delete(ptr);
		}
		m_size = 0;
	}
private:
	size_t m_size{ 0 };
	element_ptr_type m_prehead{ liw_c_nullhdl }; // Preceeding head. For easier operation.  
												 // For any constructed LIWFowardList, m_prehead should never be null. 
};