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