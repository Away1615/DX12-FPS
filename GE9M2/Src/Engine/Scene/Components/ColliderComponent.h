#pragma once
#include <string>
#include "Component.h"
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"

// A bottom-centered AABB
// transform.position represents the bottom center of the box
class ColliderComponent : public RenderComponent {

private:
    Vec3 _size;

public:

    ColliderComponent(const Vec3& size) : _size(size) {}

    const Vec3& size() const;

    Vec3 worldCenter() const;

    Vec3 worldMin() const;

    Vec3 worldMax() const;

    bool intersect(const ColliderComponent* other) const;

    bool raycast(const Vec3& ro, const Vec3& rd, float& tOut) const;

    static std::string Name() { return "ColliderComponent"; }

    void onRender(RenderContext& renderContext) override;

    RenderLayer layer() const override { return RenderLayer::DEBUG; }
};
