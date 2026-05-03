#include "AnimatorComponent.h"
#include "../../../Graphics/Animation/AnimationData.h"
#include "../../../Graphics/Animation/AnimationController.h"

AnimatorComponent::AnimatorComponent(AnimationData* animation) {
    _controller.init(animation);
}

void AnimatorComponent::onUpdate(float dt) {
    _controller.update(dt);
}

void AnimatorComponent::play(const std::string& sequence, bool force) {
    if (!force && _currentSequence == sequence)
        return;

    _currentSequence = sequence;
    _controller.play(sequence);
}

bool AnimatorComponent::isPlaying(const std::string& clip) const {
    return _currentSequence == clip;
}

bool AnimatorComponent::finished() {
    return _controller.animationFinished();
}

Matrix* AnimatorComponent::bonesMatrixs() {
    return _controller.skinningMatrices;
}