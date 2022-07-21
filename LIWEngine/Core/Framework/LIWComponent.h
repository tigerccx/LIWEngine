#pragma once
#include <atomic>

#include "LIWObject.h"
#include "LIWTypes.h"
#include "LIWConstants.h"
#include "LIWEntity.h"


/*
* class: LIWComponent
* author: tigerccx
* description:	Base class of all Components (C in ECS).
*				It offers programable APIs to implement gameplay, just as MonoBehaviour in Unity.
*				Note: Use T5CreateComponent and T5ReturnComponent to create and destroy components.
*/
namespace LIW {
	class LIWComponent :
		public LIW::LIWObject
	{
		template<class T> friend class LIWComponentManager;
	public:
		static const uint32_t sc_maskIsDeactivating;
		static const uint32_t sc_maskIsRemoving;
		static const uint32_t sc_maskHandle;

	public:
		LIWComponent() { }
		LIWComponent(const LIWComponent& other) {
			m_state = other.m_state.load(); 
			m_entity = other.m_entity.load();
		}
		LIWComponent& operator=(const LIWComponent& other) { 
			m_state = other.m_state.load(); 
			m_entity = other.m_entity.load();
			return *this; 
		}
		virtual ~LIWComponent() { }

	public:
		//
		// Management
		//
		inline void MarkRemove() { m_state.fetch_or(sc_maskIsRemoving); }
		inline void UnmarkRemove() { m_state.fetch_and(~sc_maskIsRemoving); }
		inline bool GetRemove() const { return m_state.load() & sc_maskIsRemoving; }

		inline void MarkInactive() { m_state.fetch_or(sc_maskIsDeactivating); }
		inline void MarkActive() { m_state.fetch_and(~sc_maskIsDeactivating); }
		inline bool GetInactive() const { return m_state.load() & sc_maskIsDeactivating; }
		inline bool GetActive() const { return !GetInactive(); }

		inline LIWEntity GetEntity() const { return m_entity.load(); }

		inline uint32_t GetHandle() const { return m_state.load() & sc_maskHandle; }

	private:
		//NOTE: offset can only be using 30bits (but that should be well enough)
		//NOTE: NOT thread-safe! 
		inline void SetHandle(uint32_t offset) { m_state = (m_state & (~sc_maskHandle)) + (offset & sc_maskHandle); }
		inline void SetEntity(LIWEntity entity) { m_entity.store(entity); }

	private:
		std::atomic<uint32_t> m_state{ ~uint32_t(0) }; //32bit: 1bit isDeactivating; 1bit isDeleting; 30bit handle
		std::atomic<uint32_t> m_entity{ liw_c_nullobjhdl };
	};
}

