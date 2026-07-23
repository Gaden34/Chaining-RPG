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


public:
	void update(float dt);
	void setAnimation(const std::vector<AnimationFrame>& newFrames);
	sf::IntRect getCurrentFrame() const;
};

