#include "LIWMemory.h"

DefaultBufferAllocator::GlobalGPAllocator DefaultMemBuffer::s_defaultBufferGAllocator;
std::vector<DefaultBufferAllocator::LocalGPAllocator> DefaultMemBuffer::s_defaultBufferLAllocators;

StaticBufferAllocator::GlobalStackAllocator StaticMemBuffer::s_staticBufferGAllocator;
std::vector<StaticBufferAllocator::LocalStackAllocator> StaticMemBuffer::s_staticBufferLAllocators;

FrameBufferAllocator::GlobalStackAllocator FrameMemBuffer::s_frameBufferGAllocator;
std::vector<FrameBufferAllocator::LocalStackAllocator> FrameMemBuffer::s_frameBufferLAllocators;

int DFrameBuffer::s_dframeIdx{ 0 };
DFrameBufferAllocator::GlobalStackAllocator DFrameBuffer::s_dframeBufferGAllocator[2];
std::vector<std::array<DFrameBufferAllocator::LocalStackAllocator, 2>> DFrameBuffer::s_dframeBufferLAllocators;

std::vector<std::thread> GCThreads::s_gcThreads;
std::condition_variable GCThreads::s_gcThdCondVarExecute;
std::deque<std::mutex> GCThreads::s_gcThdMutex;
std::vector<bool> GCThreads::s_gcIsRunning;

#include "Threading/LIWFiberExecutor.h"

void liw_mgc_thd(int idxThread)
{
	using namespace LIW;
	std::unique_lock<std::mutex> lk(GCThreads::s_gcThdMutex[idxThread]);
	while (GCThreads::s_gcIsRunning[idxThread]) {
		GCThreads::s_gcThdCondVarExecute.wait(lk);
		if (!GCThreads::s_gcIsRunning[idxThread])
			break;
		liw_mupdate_def_thd(idxThread);
		liw_mupdate_static_thd(idxThread);
		liw_mupdate_frame_thd(idxThread);
		liw_mupdate_dframe_thd(idxThread);
#ifdef DEBUG_PRINT_MEMORY_INFO
		printf("GC%d \t Def local_hdl: %llu global_hdl %llu \n", idxThread, 
																 DefaultMemBuffer::s_defaultBufferLAllocators[idxThread].m_handleCount, 
																 DefaultBufferAllocator::LocalGPAllocator::s_handleCount.load());
#endif
		LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, 1);
	}
}

void liw_mgc_init(int countThreads) {
	GCThreads::s_gcThreads.reserve(countThreads);
	GCThreads::s_gcThdMutex.resize(countThreads);
	GCThreads::s_gcIsRunning.reserve(countThreads);
	for (int i = 0; i < countThreads; i++) {
		GCThreads::s_gcIsRunning.emplace_back(true);
		GCThreads::s_gcThreads.emplace_back(liw_mgc_thd, i);
	}
}

void liw_mgc_wait_and_stop() {
	for (int i = 0; i < GCThreads::s_gcIsRunning.size(); i++) {
		GCThreads::s_gcIsRunning[i] = false;
	}
	GCThreads::s_gcThdCondVarExecute.notify_all();
	for (int i = 0; i < GCThreads::s_gcThreads.size(); i++) {
		GCThreads::s_gcThreads[i].join();
	}
}