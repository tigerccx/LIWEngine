#pragma once
#include "LIWObject.h"
#include "LIWComponentType.h"

/*
* class: LIWComponent
* author: tigerccx
* description:	Base class of all Components (C in ECS).
*				It offers programable APIs to implement gameplay, just as MonoBehaviour in Unity.
*				Note: Use T5CreateComponent and T5ReturnComponent to create and destroy components.
*/
class LIWComponent:
	public LIW::LIWObject
{
	template<class T> friend class LIWComponentManager;
public:
	LIWComponent() { }
	LIWComponent(const LIWComponent& other) { }
	LIWComponent& operator=(const LIWComponent& other) = default;
	virtual ~LIWComponent() { }

	//Dynamic type
	virtual LIWComponentType& GetObjType() const { return GetType<LIWComponent>(); }


protected:
	//
	// Management
	//
	inline void MarkToRemove() { m_isDeleting = true; }
	inline void MarkToDeactivate() { m_isDeactivating = true; }

protected:
	bool m_isDeactivating = false;
	bool m_isDeleting = false;
};


#define LIW_REGISTER_COMPONENT(ComponentType) public: virtual LIWComponentType& GetObjType() const override { return GetType<ComponentType>(); }

