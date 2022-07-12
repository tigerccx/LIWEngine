#pragma once
#include "LIWEntityAttachment.h"

namespace LIW {
	class LIWComponentManager;

	/*
	* Class: LIWComponent
	* Author: tigerccx
	* Description:	This class is used as a baseclass Component in a ECS framework. 
	*				(And a part of Controller in MVC-like framework)
	*				A LIWComponent in LIW framework acts as a functional plugin. 
	*				It must be attached to an LIWEntity to function, 
	*				and it will use the data provided by LIWModel on the LIWEntity.
	*/
	class LIWComponent :
		public LIWEntityAttachment{
	public:
		LIWComponent(int priority=0) : 
			LIWEntityAttachment(), m_updatePriority(priority){ Initialise(nullptr); Register(); }
		LIWComponent(void* param, int priority=0) : 
			LIWEntityAttachment(), m_updatePriority(priority){ Initialise(param); Register(); }
		virtual ~LIWComponent() { Unregister(); }

		friend class LIWComponentManager;

		/*
		* Func: Initialise
		* Description:	Initialise the Component with some parameter.
		*/
		virtual void Initialise(void* param) = 0;
		
		/*
		* Func: Start
		* Description:	Called the first frame of functioning.
		*/
		virtual void Start() { }
		/*
		* Func: Update
		* Desciption:	Update.
		*				Called when active.
		*/
		virtual void Update(float dt) { }
		/*
		* Func: FixedUpdate
		* Desciption:	Update at a fixed rate.
		*				Called when active.
		*/
		virtual void FixedUpdate(float dt) { }
		/*
		* Func: PreRenderUpdate
		* Description:	Update before render.
		*				Called when active.
		*/
		virtual void PreRenderUpdate() { }
		/*
		* Func: PostRenderUpdate
		* Description:	Update after render.
		*				Called when active.
		*/
		virtual void PostRenderUpdate() { }
		

		///*
		//* Func: SetUpdatePriority
		//* Description:	Set update priority. 
		//*				Smaller number means Update is called earlier.
		//*/
		//inline void SetUpdatePriority(int priority) { m_updatePriority = priority; }
		/*
		* Func: Activate
		* Description:	Activate component.
		*/
		inline void Activate() { if (!m_isActive) { m_isActive = true; OnActivated(); } }
		/*
		* Func: Deactivate
		* Description:	Deactivate component.
		*/
		inline void Deactivate() { if (m_isActive) { m_isActive = false; OnDeactivated(); } }
		/*
		* Func: IsActive
		* Desciprtion:	Check is component active.
		*/
		inline bool IsActive() const { return m_isActive; }

	protected:
		int m_updatePriority = 0;
		bool m_isActive = false;
		bool m_wasActive = false;

		/*
		* Func: Register
		* Description:	Register the instance to LIWComponentManager.
		*/
		virtual void Register();
		/*
		* Func: Unregister
		* Description:	Unregister the instance from LIWComponentManager.
		*/
		virtual void Unregister();

		/*
		* Func: PreUpdate
		* Description:	Handle pre-update controller events.
		*				Called no matter active or not.
		*/
		void PreUpdate() {
			if (m_isActive && !m_wasActive) {
				Start();
			}
			m_wasActive = m_isActive;
		}

		// Callbacks
		virtual void OnActivated() {};
		virtual void OnDeactivated() {};
	};

}