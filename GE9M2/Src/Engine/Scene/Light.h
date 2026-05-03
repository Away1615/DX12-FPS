#pragma once
#include "../Foundation/Base/Maths.h"

struct SkyLight {
    Vec3  color;
    float intensity;
};

struct PointLight {
    Vec3  position;
    Vec3  color;
    float intensity;
    float range;
};
