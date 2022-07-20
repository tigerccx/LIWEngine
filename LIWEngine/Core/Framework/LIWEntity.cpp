#include "LIWEntity.h"

namespace LIW {
	const LIWEntity::handle_type LIWEntity::sc_hdlnull{ UINT32_MAX };
	LIWEntity::LIWEntity():
		m_handle(sc_hdlnull)
	{
	}

}
