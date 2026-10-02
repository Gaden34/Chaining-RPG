#include "animation.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::vector<AnimationFrame> buildGridFrames(const SpriteSheetGridSpec& spec) {
	std::vector<AnimationFrame> frames;
    if (spec.totalFrames <= 0 || spec.columns <= 0 || spec.frameWidth <= 0 || spec.frameHeight <= 0 || spec.frameDuration <= 0.0f) {
        return frames;
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

Animation::Animation(const AnimationClip& clip) {
	setAnimation(clip);
}

Animation::Animation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop) {
	setAnimation(newFrames, shouldLoop);
}

void Animation::setAnimation(const AnimationClip& clip) {
    setAnimation(clip.frames, clip.loop);
}

void Animation::update(float dt) {
    if (clip == nullptr || clip->frames.empty() || finished) {
        return;
    }

    // Record previous frame so we can detect newly-entered frames
    previousFrame = currentFrame;
    elapsedTime += dt;

    while (elapsedTime >= clip->frames[currentFrame].duration) {
        elapsedTime -= clip->frames[currentFrame].duration;

        if (currentFrame + 1 < clip->frames.size()) {
            ++currentFrame;
        } else if (clip->loop) {
            currentFrame = 0;
        } else {
            finished = true;
            break;
        }
    }

    // Detect any event frames that were entered during this update and record them for consumers.
    if (!clip->eventFrames.empty()) {
        // If we wrapped or advanced multiple frames, check all intermediate frames.
        std::size_t start = previousFrame;
        std::size_t end = currentFrame;
        if (start == end) {
            // If same frame but elapsed progressed past duration and clip looped, we may have wrapped.
            // In that case, consider the current frame as entered.
            // Fall through to check current frame below.
        }

        for (int ef : clip->eventFrames) {
            if (ef < 0 || static_cast<std::size_t>(ef) >= clip->frames.size()) continue;

            // Simple check: if we advanced forward without wrapping and ef is between previousFrame exclusive and currentFrame inclusive
            if (previousFrame <= currentFrame) {
                if (static_cast<std::size_t>(ef) > previousFrame && static_cast<std::size_t>(ef) <= currentFrame) {
                    enteredEvents.push_back(ef);
                }
            } else {
                // Wrapped around: event is entered if ef > previousFrame or ef <= currentFrame
                if (static_cast<std::size_t>(ef) > previousFrame || static_cast<std::size_t>(ef) <= currentFrame) {
                    enteredEvents.push_back(ef);
                }
            }
        }
    }
}

void Animation::setAnimation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop) {
	clip = new AnimationClip{newFrames, shouldLoop};
	currentFrame = 0;
	elapsedTime = 0.0f;
	finished = clip->frames.empty();
}

std::vector<int> Animation::consumeEnteredEvents() {
    std::vector<int> out;
    out.swap(enteredEvents);
    return out;
}

sf::IntRect Animation::getCurrentFrame() const {
	if (clip == nullptr || clip->frames.empty()) return sf::IntRect();
	return clip->frames[currentFrame].rect;
}

void Animation::setFrame(size_t frameIndex) {
	if (clip == nullptr || clip->frames.empty() || frameIndex >= clip->frames.size()) return;
	currentFrame = frameIndex;
	elapsedTime = 0.0f;
	finished = false;
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
        std::cerr << "Failed to open animation file at: " << filePath << '\n';
        return false;
    }

    json root;
    try {
        file >> root;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse animation JSON: " << e.what() << '\n';
        return false;
    }

    if (!root.contains(assetName)) {
        std::cerr << "Missing animation asset: " << assetName << '\n';
        return false;
    }

    const json& assetNode = root[assetName];
    outAsset = AnimationAsset{};
    outAsset.texturePath = assetNode.value("texture", "");

    if (outAsset.texturePath.empty()) {
        std::cerr << "Animation asset has no texture path: " << assetName << '\n';
        return false;
    }

    const json& animationsNode = assetNode["animations"];
    if (!animationsNode.is_object()) {
        std::cerr << "animations must be an object for asset: " << assetName << '\n';
        return false;
    }

    for (auto it = animationsNode.begin(); it != animationsNode.end(); ++it) {
        const std::string clipName = it.key();
        const json& clipNode = it.value();

        SpriteSheetGridSpec spec;
        if (!parseGridSpec(clipNode, spec)) {
            std::cerr << "Invalid clip spec for " << assetName << ": " << clipName << '\n';
            continue;
        }

        const bool loop = clipNode.value("loop", true);
        AnimationClip clip = buildClipFromGrid(spec, loop);

        // parse optional clip-level properties
        clip.instant = clipNode.value("instant", false);
        clip.displayDuration = clipNode.value("displayDuration", 0.0f);

        const std::string anchorStr = clipNode.value("anchor", "target");
        if (anchorStr == "screen") clip.anchor = EffectAnchor::Screen;
        else if (anchorStr == "caster") clip.anchor = EffectAnchor::Caster;
        else clip.anchor = EffectAnchor::Target;

        // origin can be specified as a string like "bottom" or numeric originX/originY
        if (clipNode.contains("origin") && clipNode["origin"].is_string()) {
            std::string originStr = clipNode.value("origin", "");
            if (originStr == "bottom") {
                // bottom-center
                clip.originX = spec.frameWidth / 2.0f;
                clip.originY = spec.frameHeight;
            }
            else if (originStr == "center") {
                clip.originX = spec.frameWidth / 2.0f;
                clip.originY = spec.frameHeight / 2.0f;
            }
        }

        if (clipNode.contains("originX") && clipNode.contains("originY")) {
            clip.originX = clipNode.value("originX", clip.originX);
            clip.originY = clipNode.value("originY", clip.originY);
        }

        // optional events array: list of frame indices that emit an event when entered
        if (clipNode.contains("events") && clipNode["events"].is_array()) {
            for (const auto& ev : clipNode["events"]) {
                if (ev.is_number_integer()) clip.eventFrames.push_back(ev.get<int>());
            }
        }

        outAsset.clips[clipName] = std::move(clip);
    }

    return !outAsset.clips.empty();
}
