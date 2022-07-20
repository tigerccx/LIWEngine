#pragma once
#include <cstdio>
#include <string>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"

#include "Framework/LIWComponent.h"
#include "Framework/LIWComponentManager.h"

using namespace std;

class TestComponent00 :
	public LIWComponent
{
public:
	int ma;
	float mb;
	string mc;
};


void test() {
	// Init mem
	liw_minit_def(1);
	liw_minit_static(1);
	liw_minit_frame(1);
	liw_minit_dframe(1);

	LIW::LIWThreadRegisterID(LIW_THREAD_IDX_MAIN);
	int threadid = LIW::LIWThreadGetID();

	// Init mem for thread
	liw_minit_def_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_static_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_frame_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_dframe_thd(LIW_THREAD_IDX_MAIN);

	
	{
		LIWComponentManager<TestComponent00> manager(2, 2);

		//Frame 0

		liw_objhdl_type handle0 = manager.CreateComponent();
		auto& component0 = manager.GetComponent(handle0);
		component0.ma = 10;
		component0.mb = 10.1f;
		component0.mc = "10/1";

		liw_objhdl_type handle1 = manager.CreateComponent();
		auto& component1 = manager.GetComponent(handle1);
		component1.ma = 20;
		component1.mb = 20.1f;
		component1.mc = "20/1";

		liw_objhdl_type handle2 = manager.CreateComponent(); //Should cause expansion on invalid list
		auto& component2 = manager.GetComponent(handle2);
		component2.ma = 30;
		component2.mb = 30.1f;
		component2.mc = "30/1";
		
		auto& component1_1 = manager.GetComponent(handle1);

		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++) {
			printf("%d ", itr->GetRemove());
		}
		manager.ApplyChange();
		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);
		printf("\n");

		//Frame 1
		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++) {
			printf("%d ", itr->GetRemove());
		}
		printf("\n");

		auto& component1_2 = manager.GetComponent(handle1);

		liw_objhdl_type handle345[3];
		int i = 0;
		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++, i++) {
			printf("%d, %f, %s\n", itr->ma, itr->mb, itr->mc.c_str());
			handle345[i] = manager.CreateComponent();
			auto& component = manager.GetComponent(handle345[i]);
			component.ma = (i+1)*100;
			component.mb = component.ma + 0.2f;
			component.mc = "Wait a minute";
		}

		manager.ApplyChange();
		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);
		printf("\n");


		//Frame 2
		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++) {
			printf("%d, %f, %s\n", itr->ma, itr->mb, itr->mc.c_str());
		}
		printf("\n");

		liw_objhdl_type handle678[10];
		i = 0;
		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++, i++) {
			if (itr->ma < 30) {
				itr->MarkRemove();
			}
			else if (itr->mb < 300.0f) {
				itr->MarkInactive();
			}
			
			printf("%d, %f, %s\n", itr->ma, itr->mb, itr->mc.c_str());
			handle678[i] = manager.CreateComponent();
			auto& component = manager.GetComponent(handle678[i]);
			component.ma = (i + 1) * 100 + 3;
			component.mb = component.ma + 0.3f;
			component.mc = "Hold on";
		}

		manager.ApplyChange();
		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);
		printf("\n");

		//Frame 3
		for (auto itr = manager.GetIterBegActive(); itr != manager.GetIterEndActive(); itr++) {
			printf("%d, %f, %s\n", itr->ma, itr->mb, itr->mc.c_str());
		}
		printf("\n");

		manager.ApplyChange();
		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);
		printf("\n");
	}



	// Cleanup mem for thread
	liw_mclnup_dframe_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_frame_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_static_thd(LIW_THREAD_IDX_MAIN);
	liw_mclnup_def_thd(LIW_THREAD_IDX_MAIN);

	// Cleanup mem
	liw_mclnup_dframe();
	liw_mclnup_frame();
	liw_mclnup_static();
	liw_mclnup_def();
}