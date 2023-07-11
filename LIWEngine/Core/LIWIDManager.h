#pragma once

#include <atomic>
#include <cassert>
#include "LIWTypes.h"
#include "LIWConstants.h"
#include "LIWSingleton.h"

namespace LIW
{
	//TODO: Make this proper
	class LIWIDManager
	{
	public:
		liw_objectID GetNewID();

	private:
		std::atomic<liw_objectID> m_id{0};
	};
}
