#pragma once
#include "LIWComponentCollection.h"
#include "TestComponentCollection.h"

namespace LIW {
	/*
	* Define LIW::LIW__ComponentCollectionType
	* Must be derived from LIW::LIWComponentCollection
	*/
	typedef TestComponentCollection LIW__ComponentCollectionType;


	static_assert(std::is_base_of_v<LIWComponentCollection, LIW__ComponentCollectionType>, 
			"ComponentCollection must inherit from LIW::LIWComponentCollection. "); 
}