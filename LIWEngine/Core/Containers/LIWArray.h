#pragma once
#include "LIWTypes.h"
#include "Memory/LIWMemory.h"

template<class T, size_t Size>
class LIWArray {
public:
	typedef LIWArray<T, Size> this_type;
public:
	static const size_t c_size = Size;
public:
	class CIterator {
		friend class this_type;
	public:
		CIterator(const this_type& container, size_t idx = 0) : m_container(&container), m_idxCur(idx) { }
		CIterator(const CIterator& other) = default;
		CIterator(CIterator&& other) = default;
		CIterator& operator=(const CIterator& other) = default;
		CIterator& operator=(CIterator&& other) = default;

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

	public:
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
	LIWArray() {}
	LIWArray(const LIWArray&) = delete;
	LIWArray& operator=(const LIWArray&) = delete;

	~LIWArray() {}

	//
	// Compare
	//
	inline bool operator==(const LIWArray& other) const {
		return !memcmp(m_dataBuffer, other.m_dataBuffer, c_size*sizeof(T));
	}
	inline bool operator!=(const LIWArray& other) const {
		return !(*this == other);
	}

	//
	// Capacity
	//
	constexpr inline size_t get_size() const {
		return c_size;
	}

	//
	// Iterator
	//
	inline Iterator get_beg() {
		return Iterator(*this, 0);
	}
	inline Iterator get_end()  {
		return Iterator(*this, c_size);
	}
	inline CIterator get_cbeg() const {
		return CIterator(*this, 0);
	}
	inline CIterator get_cend() const {
		return CIterator(*this, c_size);
	}

	//
	// Access
	//
	constexpr inline const T* get_data() const {
		return m_dataBuffer;
	}
	constexpr inline LIWPointer<T, LIWMem_System> get_ptr() const {
		return LIWPointer<T, LIWMem_System>(m_dataBuffer);
	}
	inline T& operator[](size_t idx) {
		return m_dataBuffer[idx];
	}
	inline const T& operator[](size_t idx) const {
		return m_dataBuffer[idx];
	}

private:
	T m_dataBuffer[c_size]{ 0 };
};

template<class T, size_t Size, LIWMemAllocation AllocType = LIWMem_Default>
class LIWArray_Heap {
public:
	typedef LIWArray_Heap<T, Size, AllocType> this_type;
public:
	static const size_t c_size = Size;
public:
	class CIterator {
		friend class this_type;
	public:
		CIterator(const this_type& container, size_t idx = 0) : m_container(&container), m_idxCur(idx) { }
		CIterator(const CIterator& other) = default;
		CIterator(CIterator&& other) = default;
		CIterator& operator=(const CIterator& other) = default;
		CIterator& operator=(CIterator&& other) = default;

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

	public:
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
	template<class ... Args>
	LIWArray_Heap(Args&&... args) {
		m_dataBuffer = liw_anew<AllocType, T>(c_size, std::forward<Args>(args)...);
	}
	LIWArray_Heap(const LIWArray_Heap&) = delete;
	LIWArray_Heap& operator=(const LIWArray_Heap&) = delete;

	~LIWArray_Heap() {
		liw_delete(m_dataBuffer);
	}

	//
	// Compare
	//
	inline bool operator==(const LIWArray_Heap& other) const {
		return !memcmp(m_dataBuffer.get_ptr(), other.m_dataBuffer.get_ptr(), c_size * sizeof(T));
	}
	inline bool operator!=(const LIWArray_Heap& other) const {
		return !(*this == other);
	}

	//
	// Capacity
	//
	constexpr inline size_t get_size() const {
		return c_size;
	}

	//
	// Iterator
	//
	inline Iterator get_beg() {
		return Iterator(*this, 0);
	}
	inline Iterator get_end() {
		return Iterator(*this, c_size);
	}
	inline CIterator get_cbeg() const {
		return CIterator(*this, 0);
	}
	inline CIterator get_cend() const {
		return CIterator(*this, c_size);
	}

	//
	// Access
	//
	constexpr inline const T* get_data() const {
		return m_dataBuffer.get_ptr();
	}
	constexpr inline LIWPointer<T, AllocType> get_ptr() const {
		return m_dataBuffer;
	}
	inline T& operator[](size_t idx) {
		return m_dataBuffer.get_ptr()[idx];
	}
	inline const T& operator[](size_t idx) const {
		return m_dataBuffer.get_ptr()[idx];
	}

private:
	LIWPointer<T, AllocType> m_dataBuffer{ liw_c_nullhdl };
};