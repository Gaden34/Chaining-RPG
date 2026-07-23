#include "animation.h"

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
