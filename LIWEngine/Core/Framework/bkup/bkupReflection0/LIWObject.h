#pragma once
#include <string>

#include "LIWType.h"

namespace LIW{
	typedef long LIWID;

	class LIWObject {
	public:
		inline LIWID GetID() { return (LIWID)this; }
		virtual const LIWTypeID GetObjectTypeID() const { return GetTypeID<LIWObject>(); }
		virtual const std::string GetObjectTypeName() const { return GetTypeName<LIWObject>(); }
		virtual const LIWBaseType* GetObjectType() const { return GetType<LIWObject>(); }
	};
}