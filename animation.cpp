#include "animation.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


using json = nlohmann::json;

std::vector<AnimationFrame> buildGridFrames(const SpriteSheetGridSpec& spec) {
	std::vector<AnimationFrame> frames;
	if (spec.totalFrames <= 0 || spec.frameWidth <= 0 || spec.frameHeight <= 0) {
		return frames; // Return empty if invalid spec
	}

	frames.reserve(static_cast<std::size_t>(spec.totalFrames));

	for (int i = 0; i < spec.totalFrames; ++i) {
		const int col = i % spec.columns;
		const int row = i / spec.columns;

		const int x = spec.startX + col * spec.frameWidth;
		const int y = spec.startY + row * spec.frameHeight;
		frames.push_back({ sf::IntRect(x, y, spec.frameWidth, spec.frameHeight), spec.frameDuration });
	}
	return frames;
}

AnimationClip buildClipFromGrid(const SpriteSheetGridSpec& spec, bool loop) {
	AnimationClip clip;
	clip.frames = buildGridFrames(spec);
	clip.loop = loop;
	return clip;
}

Animation::Animation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop) {
	setAnimation(newFrames, shouldLoop);
}

void Animation::update(float dt) {
	if (frames.empty() || finished) return;

	elapsedTime += dt;

	while (elapsedTime >= frames[currentFrame].duration) {
		elapsedTime -= frames[currentFrame].duration;
		
		if (currentFrame + 1 < frames.size()) {
			currentFrame++;
		} else if (loop) {
				currentFrame = 0;
			} else {
				finished = true;
				break;
			}
		}
	}

void Animation::setAnimation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop) {
	frames = newFrames;
	loop = shouldLoop;
	currentFrame = 0;
	elapsedTime = 0.0f;
	finished = frames.empty();
}

sf::IntRect Animation::getCurrentFrame() const {
	if (frames.empty()) return sf::IntRect();
	return frames[currentFrame].rect;
}

namespace {
	bool parseGridSpec(const json& node, SpriteSheetGridSpec& outSpec) {
		 if (!node.is_object()) {
        return false;
    }

    outSpec.totalFrames = node.value("totalFrames", 0);
    outSpec.columns = node.value("columns", 4);
    outSpec.frameWidth = node.value("frameWidth", 0);
    outSpec.frameHeight = node.value("frameHeight", 0);
    outSpec.frameDuration = node.value("frameDuration", 0.1f);
    outSpec.startX = node.value("startX", 0);
    outSpec.startY = node.value("startY", 0);

    return outSpec.totalFrames > 0
        && outSpec.columns > 0
        && outSpec.frameWidth > 0
        && outSpec.frameHeight > 0
        && outSpec.frameDuration > 0.0f;
}
}

bool AnimationLoader::loadAssetFromFile(const std::string& filePath, const std::string& assetName, AnimationAsset& outAsset) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open animation file: " << filePath << "\n";
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse animation JSON: " << e.what() << "\n";
        return false;
    }

    if (!root.contains(assetName)) {
        std::cerr << "Missing asset in animation JSON: " << assetName << "\n";
        return false;
    }

    const json& assetNode = root[assetName];
    outAsset = AnimationAsset{};
    outAsset.texturePath = assetNode.value("texture", "");

    if (outAsset.texturePath.empty()) {
        std::cerr << "Animation asset has empty texture path: " << assetName << "\n";
        return false;
    }

    const json& animationsNode = assetNode["animations"];
    if (!animationsNode.is_object()) {
        std::cerr << "animations must be an object for asset: " << assetName << "\n";
        return false;
    }

    for (auto it = animationsNode.begin(); it != animationsNode.end(); ++it) {
        const std::string clipName = it.key();
        const json& clipNode = it.value();

        SpriteSheetGridSpec spec;
        if (!parseGridSpec(clipNode, spec)) {
            std::cerr << "Invalid clip spec for " << assetName << ":" << clipName << "\n";
            continue;
        }

        const bool loop = clipNode.value("loop", true);
        outAsset.clips[clipName] = buildClipFromGrid(spec, loop);
    }

    return !outAsset.clips.empty();
}
