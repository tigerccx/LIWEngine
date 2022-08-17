#pragma once

#include "LIWConfig.h"
#include "LIWTypes.h"

//
// Memory
//
const liw_hdl_type liw_c_hdlmask_def = (size_t{ 1 } << LIW_MEMORY_HANDLE_DIGITS) - 1;
const liw_hdl_type liw_c_offsetmask_def = ~liw_c_hdlmask_def;
const liw_hdl_type liw_c_nullhdl = size_t{ 0 } - 1;

const liw_objhdl_type liw_c_nullobjhdl = UINT32_MAX;