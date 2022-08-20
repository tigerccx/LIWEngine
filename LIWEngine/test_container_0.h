#pragma once
#include <cstdio>

#include "Memory/LIWMemory.h"
#include "Threading/LIWThread.h"
#include "Utility/LIWFile.h"

#include "Containers/LIWForwardList.h"


using namespace LIW;

void test() {

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
		LIWFowardList<int> list;
		LIWFowardList<int> list1;
		for (int i = 0; i < 10; ++i) {
			list.push_front(i);
			printf("%llu ", list.get_size());
		}
		printf("\n");
		for (auto ptr = list.get_head(); !ptr.is_null(); ptr = ptr->m_next) {
			printf("%d ", ptr->m_val);
		}
		printf("\n");

		for (auto itr = list.get_beg(); itr != list.get_end(); itr++) {
			printf("%d ", *itr);
		}
		printf("\n");
		printf("\n");

		list1.insert_after(list1.get_prebeg(), list.get_beg(), list.get_end());
		for (auto ptr = list1.get_head(); !ptr.is_null(); ptr = ptr->m_next) {
			printf("%d ", ptr->m_val);
		}
		printf("\n");
		for (int i = 0; i < 4; ++i) {
			list1.erase_after(list1.get_beg());
		}
		for (auto itr = list1.get_beg(); itr != list1.get_end(); itr++) {
			printf("%d ", *itr);
		}
		printf("\n");
		list1.erase_after(list1.get_beg(), list1.get_end());
		for (auto itr = list1.get_beg(); itr != list1.get_end(); itr++) {
			printf("%d ", *itr);
		}
		printf("\n");
		printf("\n");

		auto itr0 = list.get_beg();
		itr0++;
		itr0++;
		itr0 = list.insert_after(itr0, 15);
		itr0 = list.insert_after(itr0, 16);
		itr0 = list.insert_after(itr0, 17);
		for (auto itr = list.get_beg(); itr != list.get_end(); itr++) {
			printf("%d ", *itr);
		}
		printf("\n");




		for (int i = 0; i < 10; ++i) {
			list.pop_front();
			printf("%llu ", list.get_size());
		}
		printf("\n");

		list.clear();
		printf("%llu\n", list.get_size());
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