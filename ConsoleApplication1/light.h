#pragma once


#include <stdint.h>
#include "vect.h"

class CLight {
    CVec direction;
public:
    CLight() { direction.x = 0; direction.y = 0; direction.z = 0; direction.w = 1; }

    void init_light(CVec direction);
    CVec get_light_direction(void);
    uint32_t apply_light_intensity(uint32_t original_color, float percentage_factor);

};