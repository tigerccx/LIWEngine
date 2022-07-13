#pragma once
#include "LIWObject.h"
#include "LIWModel.h"
#include "LIWController.h"

namespace LIW {
	class LIWEntity;
	
	/*
	* Class: LIWComponent
	* Author: tigerccx
	* Description:	This class is used as a baseclass Component in a ECS framework. 
	*				A LIWComponent in LIW framework acts as a functional plugin. 
	*				It must be attached to an LIWEntity to function, 
	*				and it will use the data provided by LIWModel on the LIWEntity.
	*/
	class LIWComponent :
		public LIWObject {
	public:
		LIWComponent() : LIWObject() { Initialise(nullptr); Register(); }
		LIWComponent(void* param) : LIWObject() { Initialise(param); Register(); }
		virtual ~LIWComponent() { Unregister(); }


		/*
		* Func: Initialise
		* Description:	Initialise the Component with some parameter.
		*/
		virtual void Initialise(void* param) = 0;
		/*
		* Func: SetEntity
		* Description:	Set the Entity this Component is attached to.
		*				Note: Should only be called by the Entity. 
		*/
		inline void SetEntity(LIWEntity* entity) { entity = nullptr; }
		
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
		
		/*
		* Func: Activate
		* Description:	Activate component.
		*/
		inline void Activate() { if (!isActive) { isActive = true; OnActivated(); } }
		/*
		* Func: Deactivate
		* Description:	Deactivate component.
		*/
		inline void Deactivate() { if (isActive) { isActive = false; OnDeactivated(); } }
		/*
		* Func: IsActive
		* Desciprtion:	Check is component active.
		*/
		inline bool IsActive() const { return isActive; }

	protected:
		LIWEntity* entity;

		bool isActive = false;
		bool wasActive = false;

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
			if (isActive && !wasActive) {
				Start();
			}
			wasActive = isActive;
		}

		// Callbacks
		virtual void OnActivated() {};
		virtual void OnDeactivated() {};
	};

}