#pragma once
#include <cstdio>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"
#include "Containers/LIWDObjectPool.h"

struct TestStruct {
	int a;
	float b;
	char c[32];
};

void test() {
	printf("%llu\n", sizeof(LIWPointer<int, LIWMem_Default>));
	printf("%llu\n", sizeof(LIWPointer<float, LIWMem_DFrame>));
	//printf("%llu\n", sizeof(LIWCPointer<float, LIWMem_Default>));

	// Init mem
	liw_minit_def(1);
	liw_minit_static(1);
	liw_minit_frame(1);
	liw_minit_dframe(1);

	LIW::LIWThreadRegisterID(LIW_THREAD_IDX_MAIN);

	// Init mem for thread
	liw_minit_def_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_static_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_frame_thd(LIW_THREAD_IDX_MAIN);
	liw_minit_dframe_thd(LIW_THREAD_IDX_MAIN);

	{
		auto a = liw_new<LIWMem_Default, int>(10);

		LIW::LIWDObjectPool<TestStruct> objectPool(2, TestStruct{ 1, 1.2f, "Idk" });
		liw_objhdl_type handle = objectPool.fetch_object();
		liw_objhdl_type handle1 = objectPool.fetch_object();
		objectPool.set_capacity(10, TestStruct{ 1, 1.2f, "Idk" });

		objectPool.get_object(handle).a = 10;
		objectPool.get_object(handle1).b = 10.02f;

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		liw_objhdl_type handle2 = objectPool.fetch_object();
		TestStruct& obj = objectPool.get_object(handle);

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		objectPool.return_object(handle);
		TestStruct& obj2 = objectPool.get_object(handle2);

		objectPool.return_object(handle1);
		objectPool.return_object(handle2);

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