#pragma once

#include "Color.h"

struct Vertex
{                   // offset  size
	float x, y, z;  //   0      12
	Color color;    //  12       4
	float u, v;     //  16       8
};
