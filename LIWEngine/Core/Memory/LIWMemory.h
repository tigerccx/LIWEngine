#pragma once
#include <memory>
#include <vector>
#include <array>
#include <list>
#include <deque>
#include <mutex>
#include <condition_variable>
#include "LIWTypes.h"
#include "LIWConstants.h"
#include "LIWExceptions.h"
#include "LIWLGStackAllocator.h"
#include "LIWLGGPAllocator.h"
#include "Threading/LIWThread.h"


enum LIWMemAllocation {
	LIWMem_System,
	LIWMem_Default,
	LIWMem_Static,
	LIWMem_Frame,
	LIWMem_DFrame,
	LIWMem_Max
};

inline const liw_memsize_type operator""_KB(liw_memsize_type const x) { return 1024 * x; }
inline const liw_memsize_type operator""_MB(liw_memsize_type const x) { return 1024 * 1024 * x; }
inline const liw_memsize_type operator""_GB(liw_memsize_type const x) { return 1024 * 1024 * 1024 * x; }


/*
* These are the APIs for LIW memory management subsystem. 
* LIW memory uses handles to represent its allocations. 
* 
* // Memory subsystem maintenance functions
* liw_minit_MODE:			initialize (once per run)
* liw_mupdate_MODE:			update (once per post frame)
* liw_mclnup_MODE:			cleanup (once per run)
* liw_minit_MODE_thd:		initialize (once per run per thread)
* liw_mupdate_MODE_thd:		update (once per post frame per thread)
* liw_mclnup_MODE_thd:		cleanup (once per run per thread)
* 
* // Memory access functions
* liw_maddr_MODE:			get raw address
* liw_mset_MODE:			set value
* liw_mget_MODE:			get value
* liw_malloc_MODE:			malloc
* liw_free_MODE:			free
* liw_new_MODE:				new
* liw_anew_MODE:			new array
* liw_delete_MODE:			delete
* liw_adelete_MODE:			delete array
*/

//
// Debug Print
//
//TODO: Make more functions
void liw_mdebug_print_def(int idxThread);

//
// Fighting with templates everyday...
//
#include <type_traits>

struct liw__mem_alloc_tag_sys {};
struct liw__mem_alloc_tag_def {};
struct liw__mem_alloc_tag_static {};
struct liw__mem_alloc_tag_frame {};
struct liw__mem_alloc_tag_dframe {};

template <LIWMemAllocation MemAlloc> struct liw__mem_alloc_tag;
template <> struct liw__mem_alloc_tag<LIWMem_System> { typedef liw__mem_alloc_tag_sys type; };
template <> struct liw__mem_alloc_tag<LIWMem_Default> { typedef liw__mem_alloc_tag_def type; };
template <> struct liw__mem_alloc_tag<LIWMem_Static> { typedef liw__mem_alloc_tag_static type; };
template <> struct liw__mem_alloc_tag<LIWMem_Frame> { typedef liw__mem_alloc_tag_frame type; };
template <> struct liw__mem_alloc_tag<LIWMem_DFrame> { typedef liw__mem_alloc_tag_dframe type; };

//
// System memory allocation
// 

inline void* liw_maddr_sys(liw_hdl_type handle) {
	return (void*)handle;
}

template<class T>
inline void liw_mset_sys(liw_hdl_type handle, const T& val) {
	*((T*)handle) = val;
}
template<class T>
inline void liw_mset_sys(liw_hdl_type handle, T&& val) {
	*((T*)handle) = val;
}

template<class T>
inline T liw_mget_sys(liw_hdl_type handle) {
	return std::move(*((T*)handle));
}

inline liw_hdl_type liw_malloc_sys(liw_memsize_type size) {
	return (liw_hdl_type)malloc(size);
}

inline void liw_free_sys(liw_hdl_type handle) {
	free((void*)handle);
}

//
// Default memory allocation
// 

const size_t SIZE_MEM_DEFAULT_BUFFER = size_t{ 1 } << 32; // 4GB
const size_t COUNT_MEM_DEFAULT_HANDLE = size_t{ 1 } << 20; // around 1M handles each thread
//const size_t COUNT_MEM_DEFAULT_HANDLE = size_t{ 1 } << 10; 
const size_t SIZE_MEM_DEFAULT_BUFFER_BLOCK = size_t{ 1 } << 24; // 16MB
typedef LIW::Util::LIWLGGPAllocator<SIZE_MEM_DEFAULT_BUFFER, COUNT_MEM_DEFAULT_HANDLE, SIZE_MEM_DEFAULT_BUFFER_BLOCK> DefaultBufferAllocator;
struct DefaultMemBuffer
{
	static DefaultBufferAllocator::GlobalGPAllocator s_defaultBufferGAllocator;
	static std::vector<DefaultBufferAllocator::LocalGPAllocator> s_defaultBufferLAllocators;
};

inline void liw_minit_def(int countThreads) {
	DefaultMemBuffer::s_defaultBufferGAllocator.Init();
	DefaultMemBuffer::s_defaultBufferLAllocators.reserve(countThreads);
	DefaultMemBuffer::s_defaultBufferLAllocators.resize(countThreads);
}

inline void liw_mupdate_def() {

}

inline void liw_mclnup_def() {
	DefaultMemBuffer::s_defaultBufferGAllocator.Cleanup();
}

inline void liw_minit_def_thd(int idxThread) {

	DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].Init(DefaultMemBuffer::s_defaultBufferGAllocator);
}

inline void liw_mupdate_def_thd(int idxThread) {
	printf("RunGC %d\n", idxThread);
	DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].GC_P1();
}

inline void liw_mclnup_def_thd(int idxThread) {
	DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].Cleanup();
}

inline void* liw_maddr_def(liw_hdl_type handle) {
#ifdef _DEBUG
	void* mem = DefaultBufferAllocator::GetAddressFromHandle(handle);
	DefaultBufferAllocator::GlobalGPAllocator& alloc = DefaultMemBuffer::s_defaultBufferGAllocator;
	if ((uintptr_t)mem< (uintptr_t)alloc.GetBegPtr() || (uintptr_t)mem>=(uintptr_t)alloc.GetEndPtr())
		throw "Access out of range! ";
	return mem;
#endif
	return DefaultBufferAllocator::GetAddressFromHandle(handle);
}

template<class T>
inline void liw_mset_def(liw_hdl_type handle, const T& val) {
	DefaultBufferAllocator::SetMem<T>(handle, std::forward<T>(val));
}
template<class T>
inline void liw_mset_def(liw_hdl_type handle, T&& val) {
	DefaultBufferAllocator::SetMem<T>(handle, std::forward<T>(val));
}

template<class T>
inline T liw_mget_def(liw_hdl_type handle) {
	return std::move(DefaultBufferAllocator::GetMem<T>(handle));
}

inline liw_hdl_type liw_malloc_def(liw_memsize_type size) {
	const int threadID = LIW::LIWThreadGetID();
	try {
		return DefaultMemBuffer::s_defaultBufferLAllocators[threadID].Allocate(size);
	}
	catch (const liwexcept_out_of_memory& e) {
		printf(e.what());
		liw_mdebug_print_def(threadID);
	}
	catch (const liwexcept_memory_corruption& e) {
		printf(e.what());
	}
	return liw_c_nullhdl;
}

inline void liw_free_def(liw_hdl_type handle) {
	DefaultMemBuffer::s_defaultBufferLAllocators[LIW::LIWThreadGetID()].Free(handle);
}


//
// Static buffer
//

const size_t SIZE_MEM_STATIC_BUFFER = size_t{ 1 } << 28; // 256MB
const size_t SIZE_MEM_STATIC_BUFFER_BLOCK = size_t{ 1 } << 16; // 64KB
typedef LIW::Util::LIWLGStackAllocator<SIZE_MEM_STATIC_BUFFER, SIZE_MEM_STATIC_BUFFER_BLOCK> StaticBufferAllocator;
struct StaticMemBuffer
{
	static StaticBufferAllocator::GlobalStackAllocator s_staticBufferGAllocator;
	static std::vector<StaticBufferAllocator::LocalStackAllocator> s_staticBufferLAllocators;
};

inline void liw_minit_static(int countThreads) {
	StaticMemBuffer::s_staticBufferGAllocator.Init();
	StaticMemBuffer::s_staticBufferLAllocators.reserve(countThreads);
	StaticMemBuffer::s_staticBufferLAllocators.resize(countThreads);
}

inline void liw_mupdate_static() {
	
}

inline void liw_mclnup_static() {
	StaticMemBuffer::s_staticBufferGAllocator.Cleanup();
}

inline void liw_minit_static_thd(int idxThread) {

	StaticMemBuffer::s_staticBufferLAllocators[idxThread].Init(StaticMemBuffer::s_staticBufferGAllocator);
}

inline void liw_mupdate_static_thd(int idxThread) {

}

inline void liw_mclnup_static_thd(int idxThread) {

}

inline void* liw_maddr_static(liw_hdl_type handle) {
	return (void*)handle;
}

template<class T>
inline void liw_mset_static(liw_hdl_type handle, const T& val) {
	*((T*)handle) = val;
}
template<class T>
inline void liw_mset_static(liw_hdl_type handle, T&& val) {
	*((T*)handle) = val;
}

template<class T>
inline T liw_mget_static(liw_hdl_type handle) {
	return std::move(*((T*)handle));
}

inline liw_hdl_type liw_malloc_static(liw_memsize_type size) {
	return (liw_hdl_type)StaticMemBuffer::s_staticBufferLAllocators[LIW::LIWThreadGetID()].Allocate(size);
}

inline void liw_free_static(liw_hdl_type handle) {

}

//
// Per frame buffer
//

const size_t SIZE_MEM_FRAME_BUFFER = size_t{ 1 } << 27; // 128MB
const size_t SIZE_MEM_FRAME_BUFFER_BLOCK = size_t{ 1 } << 16; // 64KB
typedef LIW::Util::LIWLGStackAllocator<SIZE_MEM_FRAME_BUFFER, SIZE_MEM_FRAME_BUFFER_BLOCK> FrameBufferAllocator;
struct FrameMemBuffer
{
	static FrameBufferAllocator::GlobalStackAllocator s_frameBufferGAllocator;
	static std::vector<FrameBufferAllocator::LocalStackAllocator> s_frameBufferLAllocators;
};

inline void liw_minit_frame(int countThreads) {
	FrameMemBuffer::s_frameBufferGAllocator.Init();
	FrameMemBuffer::s_frameBufferLAllocators.reserve(countThreads);
	FrameMemBuffer::s_frameBufferLAllocators.resize(countThreads);
}

inline void liw_mupdate_frame() {
	FrameMemBuffer::s_frameBufferGAllocator.Clear();
}

inline void liw_mclnup_frame() {
	FrameMemBuffer::s_frameBufferGAllocator.Cleanup();
}

inline void liw_minit_frame_thd(int idxThread) {
	
	FrameMemBuffer::s_frameBufferLAllocators[idxThread].Init(FrameMemBuffer::s_frameBufferGAllocator);
}

inline void liw_mupdate_frame_thd(int idxThread) {

}

inline void liw_mclnup_frame_thd(int idxThread) {

}

inline void* liw_maddr_frame(liw_hdl_type handle) {
	return (void*)handle;
}

template<class T>
inline void liw_mset_frame(liw_hdl_type handle, const T& val) {
	*((T*)handle) = val;
}
template<class T>
inline void liw_mset_frame(liw_hdl_type handle, T&& val) {
	*((T*)handle) = val;
}

template<class T>
inline T liw_mget_frame(liw_hdl_type handle) {
	return std::move(*((T*)handle));
}

inline liw_hdl_type liw_malloc_frame(liw_memsize_type size) {
	return (liw_hdl_type)FrameMemBuffer::s_frameBufferLAllocators[LIW::LIWThreadGetID()].Allocate(size);
}

inline void liw_free_frame(liw_hdl_type handle) {
	
}

//
// Double frame buffer
//

const size_t SIZE_MEM_DFRAME_BUFFER = size_t{ 1 } << 26; // 64MB
const size_t SIZE_MEM_DFRAME_BUFFER_BLOCK = size_t{ 1 } << 16; // 64KB
typedef LIW::Util::LIWLGStackAllocator<SIZE_MEM_DFRAME_BUFFER, SIZE_MEM_DFRAME_BUFFER_BLOCK> DFrameBufferAllocator;
struct DFrameBuffer {
	static DFrameBufferAllocator::GlobalStackAllocator s_dframeBufferGAllocator[2];
	static std::vector<std::array<DFrameBufferAllocator::LocalStackAllocator, 2>> s_dframeBufferLAllocators;
	static int s_dframeIdx;
};

inline void liw_minit_dframe(int countThreads) {
	DFrameBuffer::s_dframeBufferGAllocator[0].Init();
	DFrameBuffer::s_dframeBufferGAllocator[1].Init();
	DFrameBuffer::s_dframeBufferLAllocators.reserve(countThreads);
	DFrameBuffer::s_dframeBufferLAllocators.resize(countThreads);
	DFrameBuffer::s_dframeIdx = 0;
}

inline void liw_mupdate_dframe() {
	DFrameBuffer::s_dframeBufferGAllocator[DFrameBuffer::s_dframeIdx].Clear(); //TODO: This is not right!
	DFrameBuffer::s_dframeIdx = !DFrameBuffer::s_dframeIdx;
}

inline void liw_mclnup_dframe() {
	DFrameBuffer::s_dframeBufferGAllocator[0].Cleanup();
	DFrameBuffer::s_dframeBufferGAllocator[1].Cleanup();
}

inline void liw_minit_dframe_thd(int idxThread) {
	DFrameBuffer::s_dframeBufferLAllocators[idxThread][0].Init(DFrameBuffer::s_dframeBufferGAllocator[0]);
	DFrameBuffer::s_dframeBufferLAllocators[idxThread][1].Init(DFrameBuffer::s_dframeBufferGAllocator[1]);
}

inline void liw_mupdate_dframe_thd(int idxThread) {

}

inline void liw_mclnup_dframe_thd(int idxThread) {

}

inline void* liw_maddr_dframe(liw_hdl_type handle) {
	return (void*)handle;
}

template<class T>
inline void liw_mset_dframe(liw_hdl_type handle, const T& val) {
	*((T*)handle) = val;
}
template<class T>
inline void liw_mset_dframe(liw_hdl_type handle, T&& val) {
	*((T*)handle) = val;
}

template<class T>
inline T liw_mget_dframe(liw_hdl_type handle) {
	return std::move(*((T*)handle));
}

inline liw_hdl_type liw_malloc_dframe(liw_memsize_type size) {
	return (liw_hdl_type)DFrameBuffer::s_dframeBufferLAllocators[LIW::LIWThreadGetID()][DFrameBuffer::s_dframeIdx].Allocate(size);
}

inline void liw_free_dframe(liw_hdl_type handle) {

}

//
// Interface
//

inline void* liw__maddr_foo(liw__mem_alloc_tag_sys, liw_hdl_type handle) { return liw_maddr_sys(handle); }
inline void* liw__maddr_foo(liw__mem_alloc_tag_def, liw_hdl_type handle) { return liw_maddr_def(handle); }
inline void* liw__maddr_foo(liw__mem_alloc_tag_static, liw_hdl_type handle) { return liw_maddr_static(handle); }
inline void* liw__maddr_foo(liw__mem_alloc_tag_frame, liw_hdl_type handle) { return liw_maddr_frame(handle); }
inline void* liw__maddr_foo(liw__mem_alloc_tag_dframe, liw_hdl_type handle) { return liw_maddr_dframe(handle); }

template<LIWMemAllocation MemAlloc>
inline void* liw_maddr(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	return liw__maddr_foo(tag(), handle);
}

template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_sys, liw_hdl_type handle, T&& val) { liw_mset_sys<T>(handle, std::forward<T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_def, liw_hdl_type handle, T&& val) { liw_mset_def<T>(handle, std::forward<T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_static, liw_hdl_type handle, T&& val) { liw_mset_static<T>(handle, std::forward<T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_frame, liw_hdl_type handle, T&& val) { liw_mset_frame<T>(handle, std::forward<T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_dframe, liw_hdl_type handle, T&& val) { liw_mset_dframe<T>(handle, std::forward<T>(val)); }

template<LIWMemAllocation MemAlloc, class T>
inline void liw_mset(liw_hdl_type handle, T&& val) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	liw__mset_foo<T>(tag(), handle, std::forward<T>(val));
}

template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_sys, liw_hdl_type handle, const T& val) { liw_mset_sys<T>(handle, std::forward<const T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_def, liw_hdl_type handle, const T& val) { liw_mset_def<T>(handle, std::forward<const T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_static, liw_hdl_type handle, const T& val) { liw_mset_static<T>(handle, std::forward<const T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_frame, liw_hdl_type handle, const T& val) { liw_mset_frame<T>(handle, std::forward<const T>(val)); }
template<class T>
inline void liw__mset_foo(liw__mem_alloc_tag_dframe, liw_hdl_type handle, const T& val) { liw_mset_dframe<T>(handle, std::forward<const T>(val)); }

template<LIWMemAllocation MemAlloc, class T>
inline void liw_mset(liw_hdl_type handle, const T& val) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	liw__mset_foo<T>(tag(), handle, std::forward<T>(val));
}

template<class T>
inline T liw__mget_foo(liw__mem_alloc_tag_sys, liw_hdl_type handle) { return liw_mget_sys<T>(handle); }
template<class T>
inline T liw__mget_foo(liw__mem_alloc_tag_def, liw_hdl_type handle) { return liw_mget_def<T>(handle); }
template<class T>
inline T liw__mget_foo(liw__mem_alloc_tag_static, liw_hdl_type handle) { return liw_mget_static<T>(handle); }
template<class T>
inline T liw__mget_foo(liw__mem_alloc_tag_frame, liw_hdl_type handle) { return liw_mget_frame<T>(handle); }
template<class T>
inline T liw__mget_foo(liw__mem_alloc_tag_dframe, liw_hdl_type handle) { return liw_mget_dframe<T>(handle); }

template<LIWMemAllocation MemAlloc, class T>
inline T liw_mget(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	return liw__mget_foo<T>(tag(), handle);
}

inline liw_hdl_type liw__malloc_foo(liw__mem_alloc_tag_sys, liw_memsize_type size) { return liw_malloc_sys(size); }
inline liw_hdl_type liw__malloc_foo(liw__mem_alloc_tag_def, liw_memsize_type size) { return liw_malloc_def(size); }
inline liw_hdl_type liw__malloc_foo(liw__mem_alloc_tag_static, liw_memsize_type size) { return liw_malloc_static(size); }
inline liw_hdl_type liw__malloc_foo(liw__mem_alloc_tag_frame, liw_memsize_type size) { return liw_malloc_frame(size); }
inline liw_hdl_type liw__malloc_foo(liw__mem_alloc_tag_dframe, liw_memsize_type size) { return liw_malloc_dframe(size); }

template<LIWMemAllocation MemAlloc>
inline liw_hdl_type liw_malloc(liw_memsize_type size) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	return liw__malloc_foo(tag(), size);
}

inline void liw__free_foo(liw__mem_alloc_tag_sys, liw_hdl_type handle) { liw_free_sys(handle); }
inline void liw__free_foo(liw__mem_alloc_tag_def, liw_hdl_type handle) { liw_free_def(handle); }
inline void liw__free_foo(liw__mem_alloc_tag_static, liw_hdl_type handle) { liw_free_static(handle); }
inline void liw__free_foo(liw__mem_alloc_tag_frame, liw_hdl_type handle) { liw_free_frame(handle); }
inline void liw__free_foo(liw__mem_alloc_tag_dframe, liw_hdl_type handle) { liw_free_dframe(handle); }

template<LIWMemAllocation MemAlloc>
inline void liw_free(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	liw__free_foo(tag(), handle);
}


//
// LIWPointer
//

//template<LIWMemAllocation AllocType>
//class LIWBasePointer {
//public:
//	LIWBasePointer() : m_handle(liw_c_nullhdl) {}
//	LIWBasePointer(liw_hdl_type handle) : m_handle(handle) {}
//	LIWBasePointer(const LIWBasePointer& other) = default;
//	LIWBasePointer(LIWBasePointer&& other) = default;
//	LIWBasePointer& operator=(const LIWBasePointer& other) = default;
//	LIWBasePointer& operator=(LIWBasePointer&& other) = default;
//
//	
//
//	//
//	// Access
//	//
//	
//
//
//public:
//	static const LIWBasePointer null;
//};

template<class T, LIWMemAllocation AllocType>
class LIWPointer final {
private:
	typedef LIWPointer<T, AllocType> this_type;
public:
	LIWPointer() : m_handle(liw_c_nullhdl) {}
	LIWPointer(liw_hdl_type handle) : m_handle(handle) {}
	LIWPointer(const this_type& other) = default;
	LIWPointer(this_type&& other) = default;
	LIWPointer& operator=(const this_type& other) = default;
	LIWPointer& operator=(this_type&& other) = default;

	template<class TOther>
	LIWPointer(const LIWPointer<TOther, AllocType>& other) {
		static_assert(std::is_base_of_v<T, TOther>, "Cannot assign nonderived class pointer to base class pointer. ");
		m_handle = other.get_handle();
	}
	template<class TOther>
	LIWPointer(this_type && other) {
		static_assert(std::is_base_of_v<T, TOther>, "Cannot assign nonderived class pointer to base class pointer. ");
		m_handle = other.get_handle();
	}
	template<class TOther>
	LIWPointer& operator=(const this_type & other) {
		static_assert(std::is_base_of_v<T, TOther>, "Cannot assign nonderived class pointer to base class pointer. ");
		m_handle = other.get_handle();
		return *this;
	}
	template<class TOther>
	LIWPointer& operator=(this_type && other) {
		static_assert(std::is_base_of_v<T, TOther>, "Cannot assign nonderived class pointer to base class pointer. ");
		m_handle = other.get_handle();
		return *this;
	}
	//LIWPointer(const LIWPointer& other) = default;
	//LIWPointer(LIWPointer && other) = default;
	//LIWPointer& operator=(const LIWPointer & other) = default;
	//LIWPointer& operator=(LIWPointer && other) = default;

	// No Step (cannot step handle)

	//
	// Compare
	//
	inline bool operator==(const this_type& other) const {
		return m_handle == other.m_handle;
	}
	inline bool operator!=(const this_type& other) const {
		return !(*this == other);
	}
	inline bool is_null() const {
		return m_handle == liw_c_nullhdl;
	}

	//
	// Access
	//
	inline T& operator*() {
		return *((T*)liw_maddr<AllocType>(m_handle));
	}
	inline const T& operator*() const {
		return *((const T*)liw_maddr<AllocType>(m_handle));
	}
	inline T* operator->() {
		return (T*)liw_maddr<AllocType>(m_handle);
	}
	inline const T* operator->() const {
		return (const T*)liw_maddr<AllocType>(m_handle);
	}
	inline T& operator[](size_t idx) {
		T* ptr = (T*)liw_maddr<AllocType>(m_handle);
		return ptr[idx];
	}
	inline const T& operator[](size_t idx) const {
		const T* ptr = (const T*)liw_maddr<AllocType>(m_handle);
		return ptr[idx];
	}
	inline T* get_ptr() const {
		return (T*)liw_maddr<AllocType>(m_handle);
	}
	inline liw_hdl_type get_handle() const {
		return m_handle;
	}
	static inline constexpr bool get_null() {
		return LIWPointer(liw_c_nullhdl);
	}
	inline void set_null() {
		m_handle = liw_c_nullhdl;
	}

private:
	liw_hdl_type m_handle;
#ifdef _DEBUG

#endif // _DEBUG

};

//template<class T, LIWMemAllocation AllocType>
//class LIWCPointer {
//private:
//	typedef LIWCPointer<T, AllocType> this_type;
//public:
//	LIWCPointer() :base_type() {}
//	LIWCPointer(liw_hdl_type handle) : base_type(handle) {}
//	LIWCPointer(const LIWCPointer& other) = default;
//	LIWCPointer(LIWCPointer&& other) = default;
//	LIWCPointer& operator=(const LIWCPointer& other) = default;
//	LIWCPointer& operator=(LIWCPointer&& other) = default;
//
//	LIWCPointer(const base_type& other) { *this = other; };
//	LIWCPointer& operator=(const base_type& other) { base_type::m_handle = other.get_handle(); }
//
//	// No Step (cannot step handle)
//
//	//
//	// Access
//	//
//	inline const T& operator*() {
//		return *((const T*)liw_maddr<AllocType>(base_type::m_handle));
//	}
//	inline const T& operator*() const {
//		return *((const T*)liw_maddr<AllocType>(base_type::m_handle));
//	}
//	inline const T* operator->() {
//		return (const T*)liw_maddr<AllocType>(base_type::m_handle);
//	}
//	inline const T* operator->() const {
//		return (const T*)liw_maddr<AllocType>(base_type::m_handle);
//	}
//	inline const T& operator[](size_t idx) {
//		const T* ptr = (const T*)liw_maddr<AllocType>(base_type::m_handle);
//		return ptr[idx];
//	}
//	inline const T& operator[](size_t idx) const {
//		const T* ptr = (const T*)liw_maddr<AllocType>(base_type::m_handle);
//		return ptr[idx];
//	}
//	inline T* get_ptr() const {
//		return (T*)liw_maddr<AllocType>(base_type::m_handle);
//	}
//};

//
// New&Delete (Variable&Array)
//

//
// System memory allocation (new delete)
// 

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_System> liw_new_sys(Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_sys(sizeof(T));
	const liw_hdl_type handle = (liw_hdl_type)(new(ptrRaw)T(std::forward<Args>(args)...));
	return LIWPointer<T, LIWMem_System>(handle);
}

template<class T>
inline void liw_delete_sys(LIWPointer<T, LIWMem_System> ptr) {
	const liw_hdl_type handle = ptr.get_handle();
	((T*)handle)->~T();
	liw_free_sys(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_System> liw_anew_sys(size_t size, Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_sys(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptrRaw[i]) T(std::forward<Args>(args)...);
	}
	const liw_hdl_type handle = (liw_hdl_type)(ptrRaw);
	return LIWPointer<T, LIWMem_System>(handle);
}

template<class T>
inline void liw_adelete_sys(LIWPointer<T, LIWMem_System> ptr, size_t size) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptrRaw[i].~T();
	}
	liw_free_sys(handle);
}

//
// Default memory allocation (new delete)
// 
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Default> liw_new_def(Args&&... args) {
	liw_hdl_type handle = liw_malloc_def(sizeof(T));
	T* ptrRaw = (T*)liw_maddr_def(handle);
	new(ptrRaw)T(std::forward<Args>(args)...);
	return LIWPointer<T, LIWMem_Default>(handle);
}

template<class T>
inline void liw_delete_def(LIWPointer<T, LIWMem_Default> ptr) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)liw_maddr_def(handle);
	ptrRaw->~T();
	liw_free_def(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Default> liw_anew_def(size_t size, Args&&... args) {
	liw_hdl_type handle = liw_malloc_def(sizeof(T) * size);
	T* ptrRaw = (T*)liw_maddr_def(handle);
	for (size_t i = 0; i < size; i++) {
		new(&ptrRaw[i]) T(std::forward<Args>(args)...);
	}
	return LIWPointer<T, LIWMem_Default>(handle);
}

template<class T>
inline void liw_adelete_def(LIWPointer<T, LIWMem_Default> ptr, size_t size) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)liw_maddr_def(handle);
	for (size_t i = 0; i < size; i++) {
		ptrRaw[i].~T();
	}
	liw_free_def(handle);
}

//
// Static buffer (new delete)
//
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Static> liw_new_static(Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_static(sizeof(T));
	const liw_hdl_type handle = (liw_hdl_type)(new(ptrRaw)T(std::forward<Args>(args)...));
	return LIWPointer<T, LIWMem_Static>(handle);
}

template<class T>
inline void liw_delete_static(LIWPointer<T, LIWMem_Static> ptr) {
	const liw_hdl_type handle = ptr.get_handle();
	((T*)handle)->~T();
	liw_free_static(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Static> liw_anew_static(size_t size, Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_static(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptrRaw[i]) T(std::forward<Args>(args)...);
	}
	const liw_hdl_type handle = (liw_hdl_type)(ptrRaw);
	return LIWPointer<T, LIWMem_Static>(handle);
}

template<class T>
inline void liw_adelete_static(LIWPointer<T, LIWMem_Static> ptr, size_t size) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptrRaw[i].~T();
	}
	liw_free_static(handle);
}

//
// Per frame buffer (new delete)
//
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Frame> liw_new_frame(Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_frame(sizeof(T));
	const liw_hdl_type handle = (liw_hdl_type)(new(ptrRaw)T(std::forward<Args>(args)...));
	return LIWPointer<T, LIWMem_Frame>(handle);
}

template<class T>
inline void liw_delete_frame(LIWPointer<T, LIWMem_Frame> ptr) {
	const liw_hdl_type handle = ptr.get_handle();
	((T*)handle)->~T();
	liw_free_frame(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Frame> liw_anew_frame(size_t size, Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_frame(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptrRaw[i]) T(std::forward<Args>(args)...);
	}
	const liw_hdl_type handle = (liw_hdl_type)(ptrRaw);
	return LIWPointer<T, LIWMem_Frame>(handle);
}

template<class T>
inline void liw_adelete_frame(LIWPointer<T, LIWMem_Frame> ptr, size_t size) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptrRaw[i].~T();
	}
	liw_free_frame(handle);
}

//
// Double frame buffer (new delete)
//
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_DFrame> liw_new_dframe(Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_dframe(sizeof(T));
	const liw_hdl_type handle = (liw_hdl_type)(new(ptrRaw)T(std::forward<Args>(args)...));
	return LIWPointer<T, LIWMem_DFrame>(handle);
}

template<class T>
inline void liw_delete_dframe(LIWPointer<T, LIWMem_DFrame> ptr) {
	const liw_hdl_type handle = ptr.get_handle();
	((T*)handle)->~T();
	liw_free_dframe(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_DFrame> liw_anew_dframe(size_t size, Args&&... args) {
	T* ptrRaw = (T*)liw_malloc_dframe(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptrRaw[i]) T(std::forward<Args>(args)...);
	}
	const liw_hdl_type handle = (liw_hdl_type)(ptrRaw);
	return LIWPointer<T, LIWMem_DFrame>(handle);
}

template<class T>
inline void liw_adelete_dframe(LIWPointer<T, LIWMem_DFrame> ptr, size_t size) {
	const liw_hdl_type handle = ptr.get_handle();
	T* ptrRaw = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptrRaw[i].~T();
	}
	liw_free_dframe(handle);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_System> liw__new_foo(liw__mem_alloc_tag_sys, Args&&... args) { return liw_new_sys<T>(std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Default> liw__new_foo(liw__mem_alloc_tag_def, Args&&... args) { return liw_new_def<T>(std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Static> liw__new_foo(liw__mem_alloc_tag_static, Args&&... args) { return liw_new_static<T>(std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Frame> liw__new_foo(liw__mem_alloc_tag_frame, Args&&... args) { return liw_new_frame<T>(std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_DFrame> liw__new_foo(liw__mem_alloc_tag_dframe, Args&&... args) { return liw_new_dframe<T>(std::forward<Args>(args)...); }

template<LIWMemAllocation MemAlloc, class T, class ... Args>
inline LIWPointer<T, MemAlloc> liw_new(Args&&... args) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	return liw__new_foo<T>(tag(), std::forward<Args>(args)...);
}

template<class T, class ... Args>
inline void liw__delete_foo(liw__mem_alloc_tag_sys, LIWPointer<T, LIWMem_System> ptr) { liw_delete_sys<T>(ptr); }
template<class T, class ... Args>
inline void liw__delete_foo(liw__mem_alloc_tag_def, LIWPointer<T, LIWMem_Default> ptr) { liw_delete_def<T>(ptr); }
template<class T, class ... Args>
inline void liw__delete_foo(liw__mem_alloc_tag_static, LIWPointer<T, LIWMem_Static> ptr) { liw_delete_static<T>(ptr); }
template<class T, class ... Args>
inline void liw__delete_foo(liw__mem_alloc_tag_frame, LIWPointer<T, LIWMem_Frame> ptr) { liw_delete_frame<T>(ptr); }
template<class T, class ... Args>
inline void liw__delete_foo(liw__mem_alloc_tag_dframe, LIWPointer<T, LIWMem_DFrame> ptr) { liw_delete_dframe<T>(ptr); }

template<LIWMemAllocation MemAlloc, class T>
inline void liw_delete(LIWPointer<T, MemAlloc> ptr) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	liw__delete_foo<T>(tag(), ptr);
}

template<class T, class ... Args>
inline LIWPointer<T, LIWMem_System> liw__anew_foo(liw__mem_alloc_tag_sys, size_t size, Args&&... args) { return liw_anew_sys<T>(size, std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Default> liw__anew_foo(liw__mem_alloc_tag_def, size_t size, Args&&... args) { return liw_anew_def<T>(size, std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Static> liw__anew_foo(liw__mem_alloc_tag_static, size_t size, Args&&... args) { return liw_anew_static<T>(size, std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_Frame> liw__anew_foo(liw__mem_alloc_tag_frame, size_t size, Args&&... args) { return liw_anew_frame<T>(size, std::forward<Args>(args)...); }
template<class T, class ... Args>
inline LIWPointer<T, LIWMem_DFrame> liw__anew_foo(liw__mem_alloc_tag_dframe, size_t size, Args&&... args) { return liw_anew_dframe<T>(size, std::forward<Args>(args)...); }

template<LIWMemAllocation MemAlloc, class T, class ... Args>
inline LIWPointer<T, MemAlloc> liw_anew(size_t size, Args&&... args) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	return liw__anew_foo<T>(tag(), size, std::forward<Args>(args)...);
}

template<class T, class ... Args>
inline void liw__adelete_foo(liw__mem_alloc_tag_sys, LIWPointer<T, LIWMem_System> ptr, size_t size) { liw_adelete_sys<T>(ptr, size); }
template<class T, class ... Args>
inline void liw__adelete_foo(liw__mem_alloc_tag_def, LIWPointer<T, LIWMem_Default> ptr, size_t size) { liw_adelete_def<T>(ptr, size); }
template<class T, class ... Args>
inline void liw__adelete_foo(liw__mem_alloc_tag_static, LIWPointer<T, LIWMem_Static> ptr, size_t size) { liw_adelete_static<T>(ptr, size); }
template<class T, class ... Args>
inline void liw__adelete_foo(liw__mem_alloc_tag_frame, LIWPointer<T, LIWMem_Frame> ptr, size_t size) { liw_adelete_frame<T>(ptr, size); }
template<class T, class ... Args>
inline void liw__adelete_foo(liw__mem_alloc_tag_dframe, LIWPointer<T, LIWMem_DFrame> ptr, size_t size) { liw_adelete_dframe<T>(ptr, size); }

template<LIWMemAllocation MemAlloc, class T>
inline void liw_adelete(LIWPointer<T, MemAlloc> ptr, size_t size) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	typedef typename liw__mem_alloc_tag<MemAlloc>::type tag;
	liw__adelete_foo<T>(tag(), ptr, size);
}


//
// GC threads init
//
struct GCThreads
{
	static std::vector<std::thread> s_gcThreads;
	static std::condition_variable s_gcThdCondVarExecute;
	static std::deque<std::mutex> s_gcThdMutex;
	static std::vector<bool> s_gcIsRunning;
};

// GC thread
void liw_mgc_thd(int idxThread);

// Notify GC to be executed
inline void liw_mgc_notify_execute() {
	GCThreads::s_gcThdCondVarExecute.notify_all(); //TODO: maybe need to do more than this. (eg. when some gc thd is not waiting for cond)
}

// Initialize GC
void liw_mgc_init(int countThreads);

// Wait for GC to complete and stop
void liw_mgc_wait_and_stop();