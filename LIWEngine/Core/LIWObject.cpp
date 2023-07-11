#include "LIWObject.h"
#include "LIWGlobal.h"

namespace LIW{
	LIWObject::LIWObject():
		m_objectID(LIWGlobal::s_idManager.GetNewID())
	{
	}
}