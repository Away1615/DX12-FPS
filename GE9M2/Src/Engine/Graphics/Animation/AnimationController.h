#pragma once
#include "AnimationData.h"

class AnimationController {

private:
	AnimationData* _animationData;
	Matrix boneGlobalPoseMatrices[256];
	std::string currentAnimationName;
	float t;
	Matrix coordTransform;

public:

	Matrix skinningMatrices[256];

	void init(AnimationData* animationData, int fromYZX = 0) {
		_animationData = animationData; 
	}

	void resetAnimationTime() {
		t = 0;
	}

	bool animationFinished() {
		if (!_animationData) return true;
		AnimationSequence seq = _animationData->getAnimationSequence(currentAnimationName);
		return t >= seq.duration();
	}

	void play(const std::string& name) {
		if (currentAnimationName != name) {
			currentAnimationName = name;
			t = 0.0f;
		}
	}

	void update(float dt) {
		t += dt;

		if (animationFinished() == true) {
			return;
		}

		if (currentAnimationName.empty()) {
			return;
		}

		// Get frame index and interpolation weight
		int frameIndex = 0;
		float interpolationWeight = 0;
		_animationData->getFrameIndexAndInterpolationWeight(currentAnimationName, t, frameIndex, interpolationWeight);

		// FK to get bone global pose matrices
		for (int i = 0; i < _animationData->bonesSize(); i++) {
			boneGlobalPoseMatrices[i] =
				_animationData->getInterpolatedBoneMatrices(
					currentAnimationName,
					boneGlobalPoseMatrices,
					frameIndex,
					interpolationWeight,
					i
				);
		}

		// Skinning matrices
		_animationData->getSkinningMatrices(boneGlobalPoseMatrices, skinningMatrices, coordTransform);
	}
};