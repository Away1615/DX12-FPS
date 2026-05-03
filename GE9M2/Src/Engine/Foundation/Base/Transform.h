#pragma once
#include "Maths.h"

class Transform {

public:
    Vec3 position = Vec3(0, 0, 0);
    Quaternion rotation = Quaternion();
    Vec3 scale = Vec3(1, 1, 1);

    Transform() = default;

    Transform(const Vec3& pos, const Quaternion& rot, const Vec3& scl)
        : position(pos), rotation(rot), scale(scl) {
	}

    Matrix localMatrix() const {
        Matrix T = Matrix::Translation(position);
        Matrix R = rotation.toMatrix();
        Matrix S = Matrix::Scale(scale);
        return S * R * T;
    }

    Matrix worldMatrix() {
        return localMatrix();
    }

    // Coordinate system:
    // Right-handed
    // +X = Right
    // +Y = Up
    // +Z = Forward
    Vec3 forward() const { return rotation.rotate(Vec3(0, 0, 1)); }
    Vec3 up() const { return rotation.rotate(Vec3(0, 1, 0)); }
    Vec3 right() const { return rotation.rotate(Vec3(1, 0, 0)); }


public:
    
    static Transform Interpolate(const Transform& t1, const Transform& t2, float t) {
        Transform result;

		// interpolate position by linear interpolation (Lerp)
        result.position = t1.position * (1.0f - t) + t2.position * t;

        // interpolate scale by linear interpolation (Lerp)
        result.scale = t1.scale * (1.0f - t) + t2.scale * t;

		// interpolate rotation by spherical linear interpolation (Slerp)
        result.rotation = Quaternion::slerp(t1.rotation, t2.rotation, t);

        return result;
    }

};