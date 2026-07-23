#include "animation.h"

void Animation::update(float dt) {
	if (frames.empty()) return;

	elapsedTime += dt;

	if (elapsedTime >= frames[currentFrame].duration) {
		elapsedTime -= frames[currentFrame].duration;
		currentFrame++;

		if (currentFrame >= frames.size()) {
			currentFrame = 0;
		}
	}
}

void Animation::setAnimation(const std::vector<AnimationFrame>& newFrames) {
	frames = newFrames;
	currentFrame = 0;
	elapsedTime = 0.0f;
}

sf::IntRect Animation::getCurrentFrame() const {
	if (frames.empty()) return sf::IntRect();
	return frames[currentFrame].rect;
}
