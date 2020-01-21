#pragma once

#include "Color.h"

struct FanVertex
{                   // offset  size
    float x, y, z;  //   0      12
    Color color;    //  12       4
};
