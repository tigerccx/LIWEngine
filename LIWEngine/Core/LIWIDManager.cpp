#include "LIWIDManager.h"

namespace LIW
{
	liw_objectID LIWIDManager::GetNewID()
	{
		liw_objectID invalidID{ liw_c_invalidID };

		if (m_id.compare_exchange_strong(invalidID, invalidID))
		{
			assert(false); //ID has reached maximum value! 
		}
		return m_id.fetch_add(1);
	}
}