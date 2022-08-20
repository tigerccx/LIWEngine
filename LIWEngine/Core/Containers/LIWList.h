#pragma once

#include "LIWTypes.h"
#include "LIWConstants.h"
#include "Memory/LIWMemory.h"

//TODO: Maybe a preallocated pool is better for this. 

template<class T, LIWMemAllocation AllocType = LIWMem_Default>
class LIWList {
public:
	struct Element;
public:
	typedef LIWList<T, AllocType> this_type;
	typedef LIWPointer<Element, AllocType> element_ptr_type;
private:
	struct Element {
		typename element_ptr_type m_next{ liw_c_nullhdl };
		typename element_ptr_type m_prev{ liw_c_nullhdl };
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
		inline CIterator& operator--() {
			assert(!m_ptrCur.is_null());
			m_ptrCur = m_ptrCur->m_prev;
			return *this;
		}
		inline CIterator operator--(int) {
			assert(!m_ptrCur.is_null());
			CIterator tmp{ *this };
			m_ptrCur = m_ptrCur->m_prev;
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
	LIWList() : m_size(0) {
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, T{} }
		);
		m_prehead->m_prev = m_prehead->m_next = m_prehead;
	}
	template<class ... Args>
	LIWList(size_t size, Args&&... args) : m_size(size) {
		assert(size != 0); // What r u trying to do???
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<Args>(args)... }
		);
		element_ptr_type ptrPrev = m_prehead;
		for (size_t i = 0; i < size; i++) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<Args>(args)... }
			);
			ptrPrev->m_next = ptr;
			ptr->m_prev = ptrPrev;
			ptrPrev = ptr;
		}
		ptrPrev->m_next = m_prehead;
		m_prehead->m_prev = ptrPrev;
	}
	LIWList(const LIWList& other) {
		m_prehead = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, T{} }
		);
		m_prehead->m_prev = m_prehead->m_next = m_prehead;
		*this = other;
	}
	LIWList(LIWList&& other) {
		*this = std::forward<LIWList>(other);
	}
	LIWList& operator=(const LIWList& other) {
		if (m_prehead->m_next != m_prehead) { //Clean out old data
			destroy_elements();
		}
		m_size = other.m_size;
		element_ptr_type ptrOther = other.m_prehead->m_next;
		element_ptr_type ptrPrev = m_prehead;
		for (size_t i = 0; i < m_size; i++) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, liw_c_nullhdl, *ptrOther }
			); 
			ptrPrev->m_next = ptr;
			ptr->m_prev = ptrPrev;
			ptrPrev = ptr;
			ptrOther = ptrOther->m_next;
		}
		ptrPrev->m_next = m_prehead;
		m_prehead->m_prev = ptrPrev;
		return *this;
	}
	LIWList& operator=(LIWList&& other) {
		if (!m_prehead.is_null()) {
			if (!m_prehead->m_next != m_prehead) { //Clean out old data
				destroy_elements();
			}
			liw_delete<AllocType>(m_prehead);
		}
		m_size = other.m_size;
		m_prehead = other.m_prehead;
		return *this;
	}

	~LIWList() {
		if (!m_prehead->m_next.is_null()) {
			destroy_elements();
		}
		liw_delete<AllocType>(m_prehead);
	}

	//
	// Compare
	//
	inline bool operator==(const LIWList& other) const {
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
	inline bool operator!=(const LIWList& other) const {
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
	inline element_ptr_type get_tail() const {
		return m_prehead->m_prev;
	}
	inline T get_tail_val() const {
		return m_prehead->m_prev->m_val;
	}

	//
	// Iterator
	//
	//NOTE: for List, prebeg == end
	inline Iterator get_prebeg() {
		return Iterator(m_prehead);
	}
	inline Iterator get_beg() {
		return Iterator(m_prehead->m_next);
	}
	inline Iterator get_end() {
		return get_prebeg();
	}
	inline Iterator get_cprebeg() {
		return CIterator(m_prehead);
	}
	inline CIterator get_cbeg() const {
		return CIterator(m_prehead->m_next);
	}
	inline CIterator get_cend() const {
		return get_cprebeg();
	}

	//
	// Modifer
	//
	void push_front(const T& val) {
		push_front_(std::forward<const T>(val));
	}
	void push_front(T&& val) {
		push_front_(std::forward<T>(val));
	}

	void pop_front() {
		assert(m_size != 0); // Cannot pop from an empty List. 
		auto ptr = m_prehead->m_next;
		auto ptrNext = ptr->m_next;

		ptrNext->m_prev = m_prehead;
		m_prehead->m_next = ptrNext;

		liw_delete(ptr);
		m_size--;
	}

	void push_back(const T& val) {
		push_back_(std::forward<const T>(val));
	}
	void push_back(T&& val) {
		push_back_(std::forward<T>(val));
	}

	void pop_back() {
		assert(m_size != 0); // Cannot pop from an empty List. 
		auto ptr = m_prehead->m_prev;
		auto ptrPrev = ptr->m_prev;

		ptrPrev->m_next = m_prehead;
		m_prehead->m_prev = ptrPrev;

		liw_delete(ptr);
		m_size--;
	}

	inline Iterator insert(Iterator pos, const T& val) {
		return insert_(pos, std::forward<const T>(val));
	}
	inline Iterator insert(Iterator pos, T&& val) {
		return insert_(pos, std::forward<T>(val));
	}
	template<class Iterator1>
	Iterator insert(Iterator pos, Iterator1 beg, Iterator1 end) {
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		assert(pos.m_ptrCur != m_prehead); // pos must be valid

		auto ptrNext = pos.m_ptrCur;
		auto ptrPrev = pos.m_ptrCur->m_prev;
		auto ptrPrevTmp = ptrPrev;
		
		for (auto itr = beg; itr != end; ++itr) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, liw_c_nullhdl, *itr }
			);
			ptrPrev->m_next = ptr;
			ptr->m_prev = ptrPrev;
			ptrPrev = ptr;
			m_size++;
		}
		ptrPrev->m_next = ptrNext;
		ptrNext->m_prev = ptrPrev;
		return Iterator(ptrPrevTmp->m_next);
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
		auto ptrNext = ptrPrev->m_next;
		for (auto itr = beg; itr != end; ++itr) {
			auto ptr = liw_new<AllocType, Element>(
				Element{ liw_c_nullhdl, liw_c_nullhdl, *itr }
			);
			ptrPrev->m_next = ptr;
			ptr->m_prev = ptrPrev;
			ptrPrev = ptr;
			m_size++;
		}
		ptrPrev->m_next = ptrNext;
		ptrNext->m_prev = ptrPrev;
		return Iterator(ptrPrev);
	}

	Iterator erase(Iterator pos) {
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		assert(pos.m_ptrCur != m_prehead); // pos must be valid

		auto ptrPrev = pos.m_ptrCur->m_prev;
		auto ptrNext = pos.m_ptrCur->m_next;

		ptrNext->m_prev = ptrPrev;
		ptrPrev->m_next = ptrNext;

		liw_delete(pos.m_ptrCur);
		m_size--;
		return Iterator(ptrNext);
	}
	Iterator erase(Iterator beg, Iterator end) {
		assert(!beg.m_ptrCur.is_null()); // beg must be valid
		assert(beg.m_ptrCur != m_prehead); // beg must be valid

		auto ptrEnd = end.m_ptrCur;
		auto ptrPrev = beg.m_ptrCur->m_prev;
		for (auto ptr = beg.m_ptrCur; ptr != ptrEnd; ) {
			auto ptrNext = ptr->m_next;
			liw_delete(ptr);
			ptr = ptrNext;
			m_size--;
		}
		ptrPrev->m_next = ptrEnd;
		ptrEnd->m_prev = ptrPrev;
		return end;
	}

	Iterator erase_after(Iterator pos) {
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		assert(!pos.m_ptrCur->m_next.is_null()); // next must be valid
		assert(pos.m_ptrCur->m_next != m_prehead); // next must be valid

		auto ptr = pos.m_ptrCur->m_next;
		auto ptrNext = ptr->m_next;
		
		pos.m_ptrCur->m_next = ptrNext;
		ptrNext->m_prev = pos.m_ptrCur;

		liw_delete(ptr);
		m_size--;
		return Iterator(pos.m_ptrCur->m_next);
	}
	Iterator erase_after(Iterator beg, Iterator end) {
		assert(!beg.m_ptrCur.is_null()); // beg must be valid
		assert(!beg.m_ptrCur->m_next.is_null()); // next must be valid
		assert(beg.m_ptrCur->m_next != m_prehead); // next must be valid

		auto ptrEnd = end.m_ptrCur;
		for (auto ptr = beg.m_ptrCur->m_next; ptr != ptrEnd; ) {
			auto ptrNext = ptr->m_next;
			liw_delete(ptr);
			ptr = ptrNext;
			m_size--;
		}
		beg.m_ptrCur->m_next = ptrEnd;
		ptrEnd->m_prev = beg.m_ptrCur;
		return end;
	}

	inline void clear() {
		destroy_elements();
	}

private:
	template<class TVal>
	void push_front_(TVal&& val) {
		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<TVal>(val) }
		);
		auto ptrNext = m_prehead->m_next;

		ptr->m_next = ptrNext;
		ptrNext->m_prev = ptr;
		m_prehead->m_next = ptr;
		ptr->m_prev = m_prehead;

		m_size++;
	}

	template<class TVal>
	void push_back_(TVal&& val) {
		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<TVal>(val) }
		);
		auto ptrPrev = m_prehead->m_prev;

		ptr->m_next = m_prehead;
		m_prehead->m_prev = ptr;
		ptrPrev->m_next = ptr;
		ptr->m_prev = ptrPrev;

		m_size++;
	}
	
	template<class TVal>
	Iterator insert_(Iterator pos, TVal&& val) {
		// Skipping check for if pos is in this container...
		assert(!pos.m_ptrCur.is_null()); // pos must be valid
		assert(pos.m_ptrCur != m_prehead); // pos must be valid

		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<TVal>(val) }
		);
		auto ptrPrev = pos.m_ptrCur->m_prev;

		ptr->m_next = pos.m_ptrCur;
		pos.m_ptrCur->m_prev = ptr;
		ptrPrev->m_next = ptr;
		ptr->m_prev = ptrPrev;

		m_size++;
		return Iterator(ptr);
	}

	template<class TVal>
	Iterator insert_after_(Iterator pos, TVal&& val) {
		// Skipping check for if pos is in this container...
		assert(!pos.m_ptrCur.is_null()); // pos must be valid

		auto ptr = liw_new<AllocType, Element>(
			Element{ liw_c_nullhdl, liw_c_nullhdl, std::forward<TVal>(val) }
		);
		auto ptrNext = pos.m_ptrCur->m_next;

		ptr->m_next = ptrNext;
		ptrNext->m_prev = ptr;
		pos.m_ptrCur->m_next = ptr;
		ptr->m_prev = pos.m_ptrCur;

		m_size++;
		return Iterator(ptr);
	}

	inline void destroy_elements() {
		while (m_prehead->m_next != m_prehead)
		{
			auto ptr = m_prehead->m_next;
			m_prehead->m_next = ptr->m_next;
			liw_delete(ptr);
		}
		m_prehead->m_prev = m_prehead;
		m_size = 0;
	}

private:
	size_t m_size{ 0 };
	element_ptr_type m_prehead{ liw_c_nullhdl }; // Preceeding head. For easier operation.  
												 // For any constructed LIWList, m_prehead should never be null. 
};