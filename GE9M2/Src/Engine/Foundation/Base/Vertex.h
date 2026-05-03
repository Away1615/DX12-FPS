#pragma once
#include "Maths.h"

struct STATIC_VERTEX {
    Vec3 pos;
    Vec3 normal;
    Vec3 tangent;
    float tu;
    float tv;
};

struct ANIMATED_VERTEX {
    Vec3 pos;
    Vec3 normal;
    Vec3 tangent;
    float tu;
    float tv;
    unsigned int bonesIDs[4];
    float boneWeights[4];
};

struct DEBUG_VERTEX {
    Vec3 pos;
};