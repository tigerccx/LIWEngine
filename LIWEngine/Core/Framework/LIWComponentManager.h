#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <functional>
#include <mutex>

#include "LIWEntity.h"
#include "LIWComponent.h"
#include "Containers/LIWDArray.h"

/*
* class: LIWComponentManager<T>
* author: tigerccx
* description:	LIWComponentManager<T> is used to hold all type T components. 
*				It can offer systems API to iterate through all of the same type of components. 
*/

/*
* Rules:
* Within one memory update
* 1. Iteration through active list is safe and can be conducted by multiple threads. 
* 2. Any operation during iteration through active list is safe and can be conducted by multiple threads. 
* 3. All changes to active list is only marked during iteration and processed later. 
* 4. Changes will only guarantee to happen before the next frame.
*/
namespace LIW {
	template<class T>
	class LIWComponentManager
	{
		static_assert(std::is_base_of_v<LIWComponent, T>, "Only components can use LIWComponentManager");
	public:
		typedef std::mutex mtx_type;
		typedef std::lock_guard<mtx_type> lkgd_type;
	private:
		static const uint32_t sc_maskActive; //Mask of active bit
		static const uint32_t sc_maskOffsetComponent; //Mask of component offset bits
		struct HandleData {
			union HandleOffset {
				uint32_t m_offsetComponent;
				liw_objhdl_type m_offsetNextFreeHandle;
			} m_offset;
			inline void SetNextFreeHandle(liw_objhdl_type handle) { m_offset.m_offsetNextFreeHandle = handle; }
			inline liw_objhdl_type GetNextFreeHandle() const { return m_offset.m_offsetNextFreeHandle; }
			inline void SetActive() {
				m_offset.m_offsetComponent = m_offset.m_offsetComponent | sc_maskActive;
			}
			inline void SetInactive() {
				m_offset.m_offsetComponent = m_offset.m_offsetComponent & ~sc_maskActive;
			}
			inline bool GetActive() const {
				return m_offset.m_offsetComponent & sc_maskActive;
			}
			inline void SetOffsetComponent(uint32_t offset) {
				m_offset.m_offsetComponent =
					m_offset.m_offsetComponent & ~sc_maskOffsetComponent +
					offset & sc_maskOffsetComponent;
			}
			inline uint32_t GetOffsetComponent() const {
				return m_offset.m_offsetComponent & sc_maskOffsetComponent;
			}
		};

	public:
		LIWComponentManager(size_t initCapability, size_t expandCount) :
			m_components{ initCapability }, m_componentsInactive{ initCapability }, m_handles{ initCapability }
		{
			m_components.init_expand_size(expandCount);
			m_handles.init_expand_size(expandCount);
			m_componentsInactive.init_expand_size(expandCount);

			for (uint32_t i = 0; i < initCapability; i++) {
				m_handles.push_back(HandleData{ i + 1 });
			}
			m_handles[initCapability - 1].SetNextFreeHandle(liw_c_nullobjhdl);
			m_nextFreeHandle = 0;
		}
		~LIWComponentManager() {
			CleanUp();
		}

		//
		// Modification
		//
		// NOTE: rule is that creating/deleting/activating/deactivating components should not affect current iteration. 

		/// <summary>
		/// Create component. 
		/// NOTE: Created component will be appended to the end of m_componentsInactive, which should not affect iteration. 
		/// </summary>
		/// <returns> handle of created component </returns>
		liw_objhdl_type CreateComponent() {
			const uint32_t curCountComponents = (uint32_t)m_componentsInactive.get_size();
			m_componentsInactive.push_back(T{});
			liw_objhdl_type handle = m_nextFreeHandle;
			if (handle == liw_c_nullobjhdl) { // If there is no free handle
				m_handles.expand(); // Expand to get more
				const uint32_t newCap = (uint32_t)m_handles.get_capacity();
				const uint32_t curSize = (uint32_t)m_handles.get_size();
				for (uint32_t i = curSize; i < newCap; i++) { // Link created handles
					m_handles.push_back(HandleData{ i + 1 });
				}
				m_handles[newCap - 1].SetNextFreeHandle(liw_c_nullobjhdl);
				handle = curSize; // Set handle to the first created handle
			}
			// Set next free handle
			m_nextFreeHandle = m_handles[handle].GetNextFreeHandle();
			m_handles[handle].SetNextFreeHandle(curCountComponents);
			// Init state of created component
			m_componentsInactive[curCountComponents].SetHandle(handle);
			m_componentsInactive[curCountComponents].UnmarkRemove();
			m_componentsInactive[curCountComponents].MarkActive(); // So that ApplyChange will pick this up and add to active list. 
			return handle;
		}
		// Thread-safe version
		inline liw_objhdl_type CreateComponent_thdsf() {
			lkgd_type lk(m_mtx);
			return CreateComponent();
		}
		/// <summary>
		/// Create count of components. 
		/// NOTE: It is safe if the current thread is the only one creating this type of components. 
		/// </summary>
		/// <param name="handles"> handles to components created </param>
		/// <param name="count"> count of components to create </param>
		void CreateComponents(LIWDArray<liw_objhdl_type>& handles, uint32_t count) {
			for (uint32_t i = 0; i < count; i++) {
				auto handle = CreateComponent();
				handles.push_back(handle);
			}
		}
		// Thread-safe version
		inline void CreateComponents_thdsf(LIWDArray<liw_objhdl_type>& handles, uint32_t count) {
			lkgd_type lk(m_mtx);
			CreateComponents(handles, count);
		}

		/// <summary>
		/// Mark component to be removed. 
		/// NOTE: Component marked will be ignored by ApplyIteration.
		/// </summary>
		/// <param name="handle"> handle to the component </param>
		inline void MarkRemove(liw_objhdl_type handle) {
			T& component = GetComponent(handle);
			component.MarkRemove();
		}
		/// <summary>
		/// Mark component to not be removed. 
		/// </summary>
		/// <param name="handle"> handle to the component </param>
		inline void UnmarkRemove(liw_objhdl_type handle) {
			T& component = GetComponent(handle);
			component.UnmarkRemove();
		}
		/// <summary>
		/// Mark component to be deactivated. 
		/// </summary>
		/// <param name="handle"> handle to the component </param>
		inline void MarkInactive(liw_objhdl_type handle) {
			T& component = GetComponent(handle);
			component.MarkInactive();
		}
		/// <summary>
		/// Mark component to be activated. 
		/// </summary>
		/// <param name="handle"> handle to the component </param>
		inline void MarkActive(liw_objhdl_type handle) {
			T& component = GetComponent(handle);
			component.MarkActive();
		}

		inline void MarkEntity(LIWEntity entity, liw_objhdl_type componentHandle) {
			m_entityComponentChanges.push(std::pair<LIWEntity, liw_objhdl_type>(entity, componentHandle));
		}
		inline void MarkRemoveEntity(liw_objhdl_type componentHandle) {
			MarkEntity(LIWEntityInvalid, componentHandle);
		}
		inline liw_objhdl_type GetComponentFromEntity(LIWEntity entity) {
			auto itr = m_entityComponent.find(entity);
			if (itr != m_entityComponent.end())
				return itr->second;
			else
				return liw_c_nullobjhdl;
		}

		/// <summary>
		/// Apply marked changes, 
		/// including entity assigning, removal, deactivation, activation
		/// NOTE: Not threadsafe!!! Only need to be done in one thread once per frame
		/// </summary>
		void ApplyChange() {
			//
			// Entity assign
			//
			while (!m_entityComponentChanges.empty()) {
				auto kv = m_entityComponentChanges.front();
				const LIWEntity entity = kv.first;
				const liw_objhdl_type componentHandle = kv.second;
				// Set in lookup table
				m_entityComponent[kv.first] = kv.second; 
				// Set in component
				auto& component = GetComponent(componentHandle);
				component.SetEntity(entity);
				m_entityComponentChanges.pop();
			}

			//
			// Remove
			//
			// Remove from active list
			for (auto itr = m_components.get_beg(); itr != m_components.get_end(); ) {
				if (itr->GetRemove()) {
					T& component = *itr;
					// Free handle
					const uint32_t handle = component.GetHandle();
					m_handles[handle].SetNextFreeHandle(m_nextFreeHandle);
					m_nextFreeHandle = handle;
					// Remove component
					itr = m_components.erase(itr);
				}
				else {
					itr++;
				}
			}
			// Remove from inactive list
			for (auto itr = m_componentsInactive.get_beg(); itr != m_componentsInactive.get_end(); ) {
				if (itr->GetRemove()) {
					T& component = *itr;
					// Free handle
					const uint32_t handle = component.GetHandle();
					m_handles[handle].SetNextFreeHandle(m_nextFreeHandle);
					m_nextFreeHandle = handle;
					// Remove component
					itr = m_componentsInactive.erase(itr);
				}
				else {
					itr++;
				}
			}

			uint32_t curActiveSize = (uint32_t)m_components.get_size();
			uint32_t curDeactiveSize = (uint32_t)m_componentsInactive.get_size();

			//
			// Deactivate
			//
			for (auto itr = m_components.get_beg(); itr != m_components.get_end(); ) {
				if (itr->GetInactive()) {
					T component = *itr;
					// Mark handle
					const uint32_t handle = component.GetHandle();
					m_handles[handle].SetInactive();
					m_handles[handle].SetOffsetComponent((uint32_t)m_componentsInactive.get_size());
					// Remove component
					itr = m_components.erase(itr);
					m_componentsInactive.push_back(std::move(component));
				}
				else
				{
					itr++;
				}
			}

			//
			// Activate
			//
			uint32_t count = 0; // Avoid processing the components freshly added to m_componentsInactive
			for (auto itr = m_componentsInactive.get_beg(); count < curDeactiveSize; count++) {
				if (itr->GetActive()) {
					T component = *itr;
					// Mark handle
					const uint32_t handle = component.GetHandle();
					m_handles[handle].SetActive();
					m_handles[handle].SetOffsetComponent((uint32_t)m_components.get_size());
					// Remove component
					itr = m_componentsInactive.erase(itr);
					m_components.push_back(std::move(component));
				}
				else
				{
					itr++;
				}
			}
		}

		/// <summary>
		/// Clean up everything
		/// NOTE: Not threadsafe!!! Only need to be done once per game
		/// </summary>
		void CleanUp() {
			m_components.clear();
			m_componentsInactive.clear();
			m_handles.clear();
			m_entityComponent.clear();
			while (!m_entityComponentChanges.empty()) m_entityComponentChanges.pop();
		}

		//
		// Capacity
		//
		size_t GetActiveComponentCount() const { return m_components.get_size(); }
		size_t GetInactiveComponentCount() const { return m_componentsInactive.get_size(); }

		//
		// Access
		//
		auto GetIterBegActive() { return m_components.get_beg(); }
		auto GetCIterBegActive() const { return m_components.get_cbeg(); }
		auto GetIterEndActive() { return m_components.get_end(); }
		auto GetCIterEndActive() const { return m_components.get_cend(); }
		//auto GetIterBegInactive() { return m_componentsInactive.begin(); }
		//auto GetCIterBegInactive() const { return m_componentsInactive.begin(); }
		//auto GetIterEndInactive() { return m_componentsInactive.end(); }
		//auto GetCIterEndInactive() const { return m_componentsInactive.end(); }
		inline T& GetComponent(liw_objhdl_type handle) {
			HandleData& data = m_handles[handle];
			uint32_t offset = data.GetOffsetComponent();
			if (data.GetActive()) {
				return m_components[offset];
			}
			else {
				return m_componentsInactive[offset];
			}
		}

		LIWDArray<T, LIWMem_Default, LIWDArrayExpand_Constant> m_components;
		LIWDArray<T, LIWMem_Default, LIWDArrayExpand_Constant> m_componentsInactive;
		LIWDArray<HandleData, LIWMem_Default, LIWDArrayExpand_Constant> m_handles;
		// Only allow one entity to have one in one type of components. 
		std::unordered_map<LIWEntity, liw_objhdl_type> m_entityComponent; //TODO: Build my own container and use that.
		std::queue<std::pair<LIWEntity, liw_objhdl_type>> m_entityComponentChanges;
		liw_objhdl_type m_nextFreeHandle{ liw_c_nullobjhdl };
	private:
		mtx_type m_mtx;
	};

	template<class T>
	const uint32_t LIWComponentManager<T>::sc_maskActive = 1 << 31;
	template<class T>
	const uint32_t LIWComponentManager<T>::sc_maskOffsetComponent = (uint32_t(1) << 31) - 1;
}
