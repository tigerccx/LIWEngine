#pragma once
#include "LIWObject.h"

namespace LIW {
	class LIWEntity;

	/*
	* Class: LIWEntityAttachment
	* Author: tigerccx
	* Description:	A baseclass for classes which can be acttached to Entity.
	*/
	class LIWEntityAttachment:
		public LIWObject
	{
	public:
		LIWEntityAttachment() = default;
		virtual ~LIWEntityAttachment() = default;
		/*
		* Func: SetEntity
		* Description:	Set the Entity this Component is attached to.
		*				Note: Should only be called by the Entity.
		*/
		inline void SetEntity(LIWEntity* entity) { m_entity = entity; }
		/*
		* Func: GetEntity
		* Description:	Get the Entity this Component is attached to.
		*/
		inline LIWEntity* GetEntity() { return m_entity; }
	protected:
		LIWEntity* m_entity;
	};
}