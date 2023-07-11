#pragma once
#include <string>

#include "LIWTypes.h"

namespace LIW{
	class LIWObject {
	public:
		LIWObject();

		constexpr liw_objectID GetID() const { return m_objectID; }
		//inline virtual const LIWTypeID GetObjectTypeID() const { return GetTypeID<LIWObject>(); }
		//inline virtual const std::string GetObjectTypeName() const { return GetTypeName<LIWObject>(); }
		//inline virtual LIWBaseType* GetObjectType() { return GetType<LIWObject>(); }
	private:
		liw_objectID m_objectID;
	};
}