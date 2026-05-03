#include "CowControllerComponent.h"
#include "../../GameObject.h"
#include "../Animator/AnimatorComponent.h"

CowControllerComponent::CowControllerComponent(const CowAnimConfig& config)
	: _animConfig(config) {}

void CowControllerComponent::setIntent(Intent intent) {
    // Only accept new intent when idle
    if (_state != State::Idle)
        return;

    if (_intent == Intent::None)
        _intent = intent;
}

void CowControllerComponent::onUpdate(float dt) {
    auto* animator = _owner->getComponent<AnimatorComponent>();
    if (!animator) return;

    if (!_idleInitialized) {
        animator->play(_animConfig.idle, true);
        _state = State::Idle;
        _idleInitialized = true;
        return;
    }

    if (_state == State::Death) { return; }

    switch (_state) {
    case State::Idle:
        if (_intent == Intent::Hitted) {
            animator->play(_animConfig.hit, true);
            _state = State::Hitting;
        }
        else if (_intent == Intent::Death) {
            animator->play(_animConfig.death, true);
            _state = State::Death;
        }
        _intent = Intent::None;
        break;

    case State::Hitting:
        if (animator->finished()) {
            animator->play(_animConfig.idle);
            _state = State::Idle;
        }
        break;

    case State::Death:
        if (animator->finished()) {
            break;
        }
        break;
    }

}