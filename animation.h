#pragma once
#include <SFML/Graphics.hpp>
#include <vector>


struct AnimationFrame {
	sf::IntRect rect;
	float duration;
};


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

