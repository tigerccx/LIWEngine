#pragma once
#include <cstdio>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"
#include "Containers/LIWObjectPool.h"
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

	/*{
		auto a = liw_new<LIWMem_Default, int>(10);

		LIW::LIWDObjectPool<TestStruct> objectPool(2, TestStruct{ 1, 1.2f, "Idk" });
		auto ptr = objectPool.fetch_object();
		auto ptrRaw = ptr.get_ptr();
		auto ptr1 = objectPool.fetch_object();
		objectPool.set_capacity(10, TestStruct{ 1, 1.2f, "Idk" });

		ptr->a = 10;
		ptr->b = 10.02f;

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		auto ptr2 = objectPool.fetch_object();
		TestStruct& obj = *ptr;

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		objectPool.return_object(ptr);
		TestStruct& obj2 = *ptr2;

		objectPool.return_object(ptr1);
		objectPool.return_object(ptr2);

	}*/

	{
		LIW::LIWObjectPool_Heap<TestStruct, 10> objectPool(TestStruct{ 1, 1.2f, "Idk" });
		auto ptr = objectPool.fetch_object();
		auto ptrRaw = ptr.get_ptr();
		auto ptr1 = objectPool.fetch_object();

		ptr->a = 10;
		ptr->b = 10.02f;

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		auto ptr2 = objectPool.fetch_object();
		TestStruct& obj1 = *ptr1;
		obj1.a = 22;
		obj1.b = 24.6f;
		strcpy_s(obj1.c, 32, "Wait what???????");

		liw_mupdate_def_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_static_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_frame_thd(LIW_THREAD_IDX_MAIN);
		liw_mupdate_dframe_thd(LIW_THREAD_IDX_MAIN);

		objectPool.return_object(ptr);
		TestStruct& obj2 = *ptr2;

		objectPool.return_object(ptr1);
		objectPool.return_object(ptr2);

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