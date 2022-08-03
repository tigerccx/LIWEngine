#ifndef LIW_LIGHTING_DEFERRED_VISIBILITY_H
#define LIW_LIGHTING_DEFERRED_VISIBILITY_H

#include "LIWLighting_Deferred.glsl"

const uint liw_c_maskIdxTriangle = (0x1<<18)-1; //Last 18 bits
const uint liw_c_maskIdxObject = ~liw_c_maskIdxTriangle; //First 14 bits

#endif //LIW_LIGHTING_DEFERRED_VISIBILITY_H