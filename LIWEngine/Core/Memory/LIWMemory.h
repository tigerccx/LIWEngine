#pragma once
#include <memory>
#include <vector>
#include <array>
#include "LIWTypes.h"
#include "LIWLGStackAllocator.h"
#include "LIWLGGPAllocator.h"
#include "Fiber/LIWThread.h"

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

template<class T, class ... Args>
inline liw_hdl_type liw_new_sys(Args&&... args) {
	T* ptr = (T*)liw_malloc_sys(sizeof(T));
	return (liw_hdl_type)(new(ptr)T(std::forward<Args>(args)...));
}

template<class T>
inline void liw_delete_sys(liw_hdl_type handle) {
	((T*)handle)->~T();
	liw_free_sys(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_anew_sys(size_t size, Args&&... args) {
	T* ptr = (T*)liw_malloc_sys(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptr[i]) T(std::forward<Args>(args)...);
	}
	return (liw_hdl_type)(ptr);
}

template<class T>
inline void liw_adelete_sys(liw_hdl_type handle, size_t size) {
	T* ptr = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptr[i].~T();
	}
	liw_free_sys(handle);
}

//
// Default memory allocation
// 

const size_t SIZE_MEM_DEFAULT_BUFFER = size_t{ 1 } << 32; // 4GB
const size_t COUNT_MEM_DEFAULT_HANDLE = size_t{ 1 } << 20; // around 1M handles each thread
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
	DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].GC_P1();
}

inline void liw_mclnup_def_thd(int idxThread) {
	DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].Cleanup();
}

inline void* liw_maddr_def(liw_hdl_type handle) {
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
	return DefaultMemBuffer::s_defaultBufferLAllocators[LIW::LIWThreadGetID()].Allocate(size);
}

inline void liw_free_def(liw_hdl_type handle) {
	DefaultMemBuffer::s_defaultBufferLAllocators[LIW::LIWThreadGetID()].Free(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_new_def(Args&&... args) {
	liw_hdl_type handle = liw_malloc_def(sizeof(T));
	T* ptr = (T*)liw_maddr_def(handle);
	new(ptr)T(std::forward<Args>(args)...);
	return handle;
}

template<class T>
inline void liw_delete_def(liw_hdl_type handle) {
	T* ptr = (T*)liw_maddr_def(handle);
	ptr->~T();
	liw_free_def(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_anew_def(size_t size, Args&&... args) {
	liw_hdl_type handle = liw_malloc_def(sizeof(T) * size);
	T* ptr = (T*)liw_maddr_def(handle);
	for (size_t i = 0; i < size; i++) {
		new(&ptr[i]) T(std::forward<Args>(args)...);
	}
	return handle;
}

template<class T>
inline void liw_adelete_def(liw_hdl_type handle, size_t size) {
	T* ptr = (T*)liw_maddr_def(handle);
	for (size_t i = 0; i < size; i++) {
		ptr[i].~T();
	}
	liw_free_def(handle);
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

template<class T, class ... Args>
inline liw_hdl_type liw_new_static(Args&&... args) {
	T* ptr = (T*)liw_malloc_static(sizeof(T));
	return (liw_hdl_type)(new(ptr)T(std::forward<Args>(args)...));
}

template<class T>
inline void liw_delete_static(liw_hdl_type handle) {
	((T*)handle)->~T();
	liw_free_static(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_anew_static(size_t size, Args&&... args) {
	T* ptr = (T*)liw_malloc_static(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptr[i]) T(std::forward<Args>(args)...);
	}
	return (liw_hdl_type)(ptr);
}

template<class T>
inline void liw_adelete_static(liw_hdl_type handle, size_t size) {
	T* ptr = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptr[i].~T();
	}
	liw_free_static(handle);
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

template<class T, class ... Args>
inline liw_hdl_type liw_new_frame(Args&&... args) {
	T* ptr = (T*)liw_malloc_frame(sizeof(T));
	return (liw_hdl_type)(new(ptr)T(std::forward<Args>(args)...));
}

template<class T>
inline void liw_delete_frame(liw_hdl_type handle) {
	((T*)handle)->~T();
	liw_free_frame(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_anew_frame(size_t size, Args&&... args) {
	T* ptr = (T*)liw_malloc_frame(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptr[i]) T(std::forward<Args>(args)...);
	}
	return (liw_hdl_type)(ptr);
}

template<class T>
inline void liw_adelete_frame(liw_hdl_type handle, size_t size) {
	T* ptr = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptr[i].~T();
	}
	liw_free_frame(handle);
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

template<class T, class ... Args>
inline liw_hdl_type liw_new_dframe(Args&&... args) {
	T* ptr = (T*)liw_malloc_dframe(sizeof(T));
	return (liw_hdl_type)(new(ptr)T(std::forward<Args>(args)...));
}

template<class T>
inline void liw_delete_dframe(liw_hdl_type handle) {
	((T*)handle)->~T();
	liw_free_dframe(handle);
}

template<class T, class ... Args>
inline liw_hdl_type liw_anew_dframe(size_t size, Args&&... args) {
	T* ptr = (T*)liw_malloc_dframe(sizeof(T) * size);
	for (size_t i = 0; i < size; i++) {
		new(&ptr[i]) T(std::forward<Args>(args)...);
	}
	return (liw_hdl_type)(ptr);
}

template<class T>
inline void liw_adelete_dframe(liw_hdl_type handle, size_t size) {
	T* ptr = (T*)handle;
	for (size_t i = 0; i < size; i++) {
		ptr[i].~T();
	}
	liw_free_dframe(handle);
}

//
// LIW memory interface
//
enum LIWMemAllocation {
	LIWMem_System,
	LIWMem_Default,
	LIWMem_Static,
	LIWMem_Frame,
	LIWMem_DFrame,
	LIWMem_Max
};


template<LIWMemAllocation MemAlloc>
inline void* liw_maddr(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		return liw_maddr_sys(handle);
	case LIWMem_Default:
		return liw_maddr_def(handle);
	case LIWMem_Static:
		return liw_maddr_static(handle);
	case LIWMem_Frame:
		return liw_maddr_frame(handle);
	case LIWMem_DFrame:
		return liw_maddr_dframe(handle);
	}
	throw "Im just here to stop the warning...";
}

template<LIWMemAllocation MemAlloc, class T>
inline void liw_mset(liw_hdl_type handle, T&& val) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		liw_mset_sys<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Default:
		liw_mset_def<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Static:
		liw_mset_static<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Frame:
		liw_mset_frame<T>(handle, std::forward<T>(val)); break;
	case LIWMem_DFrame:
		liw_mset_dframe<T>(handle, std::forward<T>(val)); break;
	}
}
template<LIWMemAllocation MemAlloc, class T>
inline void liw_mset(liw_hdl_type handle, const T& val) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		liw_mset_sys<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Default:
		liw_mset_def<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Static:
		liw_mset_static<T>(handle, std::forward<T>(val)); break;
	case LIWMem_Frame:
		liw_mset_frame<T>(handle, std::forward<T>(val)); break;
	case LIWMem_DFrame:
		liw_mset_dframe<T>(handle, std::forward<T>(val)); break;
	}
}

template<LIWMemAllocation MemAlloc, class T>
inline T liw_mget(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		return std::move(liw_mget_sys<T>(handle)); 
	case LIWMem_Default:
		return std::move(liw_mget_def<T>(handle));
	case LIWMem_Static:
		return std::move(liw_mget_static<T>(handle));
	case LIWMem_Frame:
		return std::move(liw_mget_frame<T>(handle));
	case LIWMem_DFrame:
		return std::move(liw_mget_dframe<T>(handle));
	}
	throw "Im just here to stop the warning...";
}

template<LIWMemAllocation MemAlloc>
inline liw_hdl_type liw_malloc(liw_memsize_type size) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		return liw_malloc_sys(size);
	case LIWMem_Default:
		return liw_malloc_def(size);
	case LIWMem_Static:
		return liw_malloc_static(size);
	case LIWMem_Frame:
		return liw_malloc_frame(size);
	case LIWMem_DFrame:
		return liw_malloc_dframe(size);
	}
	throw "Im just here to stop the warning...";
}

template<LIWMemAllocation MemAlloc>
inline void liw_free(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		liw_free_sys(handle); break;
	case LIWMem_Default:
		liw_free_def(handle); break;
	case LIWMem_Static:
		liw_free_static(handle); break;
	case LIWMem_Frame:
		liw_free_frame(handle); break;
	case LIWMem_DFrame:
		liw_free_dframe(handle); break;
	}
}

template<LIWMemAllocation MemAlloc, class T, class ... Args>
inline liw_hdl_type liw_new(Args&&... args) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		return liw_new_sys<T>(std::forward<Args>(args)...);
	case LIWMem_Default:
		return liw_new_def<T>(std::forward<Args>(args)...);
	case LIWMem_Static:
		return liw_new_static<T>(std::forward<Args>(args)...);
	case LIWMem_Frame:
		return liw_new_frame<T>(std::forward<Args>(args)...);
	case LIWMem_DFrame:
		return liw_new_dframe<T>(std::forward<Args>(args)...);
	}
	throw "Im just here to stop the warning...";
}

template<LIWMemAllocation MemAlloc, class T>
inline void liw_delete(liw_hdl_type handle) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		liw_delete_sys<T>(handle); break;
	case LIWMem_Default:
		liw_delete_def<T>(handle); break;
	case LIWMem_Static:
		liw_delete_static<T>(handle); break;
	case LIWMem_Frame:
		liw_delete_frame<T>(handle); break;
	case LIWMem_DFrame:
		liw_delete_dframe<T>(handle); break;
	}
}

template<LIWMemAllocation MemAlloc, class T, class ... Args>
inline liw_hdl_type liw_anew(size_t size, Args&&... args) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		return liw_anew_sys<T>(size, std::forward<Args>(args)...);
	case LIWMem_Default:
		return liw_anew_def<T>(size, std::forward<Args>(args)...);
	case LIWMem_Static:
		return liw_anew_static<T>(size, std::forward<Args>(args)...);
	case LIWMem_Frame:
		return liw_anew_frame<T>(size, std::forward<Args>(args)...);
	case LIWMem_DFrame:
		return liw_anew_dframe<T>(size, std::forward<Args>(args)...);
	}
	throw "Im just here to stop the warning...";
}

template<LIWMemAllocation MemAlloc, class T>
inline void liw_adelete(liw_hdl_type handle, size_t size) {
	static_assert(MemAlloc < LIWMem_Max, "Must use a valid LIWMemAllocation enum. ");
	switch (MemAlloc)
	{
	case LIWMem_System:
		liw_adelete_sys<T>(handle, size); break;
	case LIWMem_Default:
		liw_adelete_def<T>(handle, size); break;
	case LIWMem_Static:
		liw_adelete_static<T>(handle, size); break;
	case LIWMem_Frame:
		liw_adelete_frame<T>(handle, size); break;
	case LIWMem_DFrame:
		liw_adelete_dframe<T>(handle, size); break;
	}
}