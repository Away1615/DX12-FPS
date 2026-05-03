#pragma once
#include "../Component.h"

struct WeaponAnimConfig {
public:
    std::string idle;
    std::string fire;
    std::string reload;
};

class WeaponControllerComponent : public Component {
public:
    enum class State { Idle, Firing, Reloading };

    enum class Intent { None, Fire, Reload };

    State _state = State::Idle;
    Intent _intent = Intent::None;

    bool _idleInitialized = false;

    WeaponAnimConfig _animConfig;

    WeaponControllerComponent(const WeaponAnimConfig& config);

    void setIntent(Intent intent);

    void onUpdate(float dt) override;

    static std::string Name() { return "WeaponControllerComponent"; }

private:
    void fireRaycast();

};
