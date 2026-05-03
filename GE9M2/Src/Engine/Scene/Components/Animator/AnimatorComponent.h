#pragma once
#include "../Component.h"
#include "../../../Graphics/Animation/AnimationController.h"
#include <string>
#include <vector>

class AnimationData;

class AnimatorComponent : public Component {
private:
    AnimationController _controller;
    std::string _currentSequence;

public:
    AnimatorComponent(AnimationData* animation);

    void onUpdate(float dt) override;

    void play(const std::string& clip, bool force = false);
    bool isPlaying(const std::string& clip) const;
    bool finished();

    Matrix* bonesMatrixs();

    static std::string Name() { return "AnimatorComponent"; }
};
