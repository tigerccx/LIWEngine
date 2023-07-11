#pragma once

#include "LIWConfig.h"
#include "LIWMacros.h"
#include "LIWTypes.h"

//
// Object and Memory
//
const liw_objectID liw_c_invalidID = liw_objectID{ 0 } - 1;

const liw_hdl_type liw_c_hdlmask_def = (size_t{ 1 } << LIW_MEMORY_DEFAULT_HANDLE_HANDLE_DIGITS) - 1;
const liw_hdl_type liw_c_offsetmask_def = ~liw_c_hdlmask_def;
const liw_hdl_type liw_c_nullhdl = size_t{ 0 } - 1;

const liw_objhdl_type liw_c_nullobjhdl = UINT32_MAX;