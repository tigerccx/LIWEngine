#include "LIWComponent.h"

const liw_objhdl_type LIWComponent::sc_maskIsDeactivating = 1 << 31;
const liw_objhdl_type LIWComponent::sc_maskIsRemoving = 1 << 30;
const liw_objhdl_type LIWComponent::sc_maskHandle = (1 << 30) - 1;