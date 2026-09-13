#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>


struct AnimationFrame {
	sf::IntRect rect;
	float duration;
};

struct SpriteSheetGridSpec {
	int totalFrames = 0;
	int columns = 4;
	int frameWidth = 0;
	int frameHeight = 0;
	float frameDuration = 0.1f;
	int startX = 0;
	int startY = 0;
};

struct AnimationClip {
	std::vector<AnimationFrame> frames;
	bool loop = true;
	// If true, the clip should be drawn instantly at its target (no travel/drop animation).
	bool instant = false;
	// Optional display duration used for instant clips (seconds). If <= 0, the caller may choose a default.
	float displayDuration = 0.0f;
	// Optional origin offset (pixels) to use when drawing the sprite. If both are negative, caller should use default.
	float originX = -1.0f;
	float originY = -1.0f;
};

struct AnimationAsset {
	std::string texturePath;
	std::unordered_map<std::string, AnimationClip> clips;
};

std::vector<AnimationFrame> buildGridFrames (const SpriteSheetGridSpec& spec);
AnimationClip buildClipFromGrid(const SpriteSheetGridSpec& spec, bool loop);

class Animation
{
private:
	const AnimationClip* clip = nullptr;
	std::size_t currentFrame = 0;
	float elapsedTime = 0.0f;
	bool finished = false;


public:
	Animation() = default;
	explicit Animation(const AnimationClip& clip);
	explicit Animation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop = true);
	void update(float dt);
	void setAnimation(const AnimationClip& clip);
	void setAnimation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop = true);
	void reset() { currentFrame = 0; elapsedTime = 0.0f; finished = clip == nullptr || clip->frames.empty(); }
	bool isFinished() const { return finished; }
	bool isLooping() const { return clip != nullptr && clip->loop; }
	sf::IntRect getCurrentFrame() const;
	void setFrame(std::size_t frameIndex);
};

class AnimationLoader {
private:


public:
	static bool loadAssetFromFile(const std::string& filePath, const std::string& assetName, AnimationAsset& outAsset);
};