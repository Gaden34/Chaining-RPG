#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>


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
	std::vector<AnimationFrame> frames;
	int currentFrame = 0;
	float elapsedTime = 0.0f;
	bool loop = true;
	bool finished = false;


public:
	Animation() = default;
	explicit Animation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop = true);
	void update(float dt);
	void setAnimation(const std::vector<AnimationFrame>& newFrames, bool shouldLoop = true);
	void reset() { currentFrame = 0; elapsedTime = 0.0f; finished = frames.empty(); }
	bool isFinished() const { return finished; }
	bool isLooping() const { return loop; }
	sf::IntRect getCurrentFrame() const;
};

class AnimationLoader {
private:


public:
	static bool loadAssetFromFile(const std::string& filePath, const std::string& assetName, AnimationAsset& outAsset);
};