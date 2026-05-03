#pragma once
#include "../Component.h"

struct CowAnimConfig {
public:
    std::string idle;
    std::string hit;
    std::string death;
};

class CowControllerComponent : public Component {
private:
    int _hp = 100;

public:
    enum class State { Idle, Hitting, Death };

    enum class Intent { None, Hitted, Death };

    State _state = State::Idle;
    Intent _intent = Intent::None;

    bool _idleInitialized = false;

    CowAnimConfig _animConfig;

public:
    CowControllerComponent(const CowAnimConfig& config);

    void setIntent(Intent intent);

    void onUpdate(float dt) override;

    void applyDamage(int value) {
        int newHP = _hp - value;
        _hp = newHP > 0 ? newHP : 0;

        if (_hp <= 0) {
            setIntent(Intent::Death);
        }
        else {
            setIntent(Intent::Hitted);
        }
    }

    static std::string Name() { return "CowControllerComponent"; }
};
