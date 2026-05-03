#pragma once
#include <vector>
#include <string>
#include <cassert>
#include "../../Foundation/Base/Maths.h"
#include "../../Foundation/Base/Transform.h"
#include "../../../../Third_Party/GEMLoader.h"

struct Bone {
	std::string name;
	Matrix offset;
	int parentIndex;
};

struct Skeleton {
	std::vector<Bone> bones;
	Matrix globalInverse;

	// Find bone index by name, return -1 if not found
	int findBone(std::string name) {
		for (int i = 0; i < bones.size(); i++) {
			if (bones[i].name == name) {
				return i;
			}
		}
		return -1;
	}

	int getBoneParentByIndex(int boneIndex) {
		if (boneIndex < 0 || boneIndex >= bones.size()) {
			return -1;
		}
		return bones[boneIndex].parentIndex;
	}
};

struct AnimationFrame {

	//  Local transforms for each bone at this frame
	std::vector<Transform> boneLocalTransforms;
};

class AnimationSequence {

public:
	std::vector<AnimationFrame> frames;
	float ticksPerSecond;

	float duration() const {
		return frames.size() / ticksPerSecond;
	}

	int getNextFrameIndex(int currentIndex) const {
		return std::min<int>(currentIndex + 1, (int)frames.size() - 1);
	}

	// Get current frame index and interpolation weight at time t
	void getFrameIndexAndInterpolationWeight(
		float t,
		int& outFrameIndex,
		float& outInterpolationWeight
	) const {

		// Which frame should be playing at the current duration
		float idealFrameIndex = t * ticksPerSecond;
		outFrameIndex = (int)floorf(idealFrameIndex);

		// Interpolation factor between current frame and next frame
		outInterpolationWeight = idealFrameIndex - (float)outFrameIndex;

		// Get clamped frame index
		outFrameIndex = std::min<int>(outFrameIndex, frames.size() - 1);
	}

	// Get interpolated bone matrix in world space at time t
	Matrix getInterpolatedBoneMatrix(
		Matrix* boneGlobalPoseMatrices,
		int baseFrameIndex,
		float interpolationFact,
		Skeleton* skeleton,
		int boneIndex
	) const {
		Transform localTransform = getInterpolatedLocalTransform(baseFrameIndex, interpolationFact, boneIndex);
		Matrix T = Matrix::Translation(localTransform.position);
		Matrix R = localTransform.rotation.toMatrix();
		Matrix S = Matrix::Scale(localTransform.scale);

		Matrix localPoseMatrix = S * R * T ;

		int parentIndex = skeleton->getBoneParentByIndex(boneIndex);

		if (parentIndex > -1) {
			Matrix globalPoseMatrix = localPoseMatrix * boneGlobalPoseMatrices[parentIndex];
			return globalPoseMatrix;
		}

		return localPoseMatrix;
	}

private:
	Transform getInterpolatedLocalTransform(int baseFrameIndex, float interpolationFact, int boneIndex) const {
		int nextFrameIndex = getNextFrameIndex(baseFrameIndex);
		if (baseFrameIndex == -1) {
			return Transform();
		}
		const auto& frame1 = frames[baseFrameIndex];
		const auto& frame2 = frames[nextFrameIndex];

		if (boneIndex >= frame1.boneLocalTransforms.size() ||
			boneIndex >= frame2.boneLocalTransforms.size())
		{
			return Transform();
		}


		const Transform& t1 = frames[baseFrameIndex].boneLocalTransforms[boneIndex];
		const Transform& t2 = frames[nextFrameIndex].boneLocalTransforms[boneIndex];

		// Get new transform by interpolating t1 and t2
		return Transform::Interpolate(t1, t2, interpolationFact);
	}
};

class AnimationData {
public:
	std::map<std::string, AnimationSequence> animations;
	Skeleton skeleton;

	int bonesSize() {
		return skeleton.bones.size();
	}

	int getBoneParentByIndex(int boneIndex) {
		return skeleton.getBoneParentByIndex(boneIndex);
	}

	AnimationSequence& getAnimationSequence(std::string name) {
		return animations[name];
	}

	// Get animation current frame index and interpolation weight at time t
	void getFrameIndexAndInterpolationWeight(
		std::string name,
		float t,
		int& outFrameIndex,
		float& outInterpolationWeight
	) {
		animations[name].getFrameIndexAndInterpolationWeight(t, outFrameIndex, outInterpolationWeight);
	}

	Matrix getInterpolatedBoneMatrices(
		std::string name,
		Matrix* boneGlobalPoseMatrices,
		int baseFrameIndex,
		float interpolationFact,
		int boneIndex
	) {
		return animations[name].getInterpolatedBoneMatrix(
			boneGlobalPoseMatrices,
			baseFrameIndex,
			interpolationFact,
			&skeleton,
			boneIndex);
	}

	void getSkinningMatrices(Matrix* boneGlobalPoseMatrices, Matrix* outMatrices, Matrix coordTransform) {
		for (int i = 0; i < bonesSize(); ++i) {
			outMatrices[i] = skeleton.bones[i].offset * boneGlobalPoseMatrices[i] * skeleton.globalInverse * coordTransform;
		}
	}

	bool hasAnimation(std::string name) {
		if (animations.find(name) == animations.end()) {
			return false;
		}
		return true;
	}
};