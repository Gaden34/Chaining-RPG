#include "chainSystem.h"


void ChainSystem::startChainTimer(float dt) {
	if (windowOpen) {
		chainTimer += dt;
		if (chainTimer >= 0.2f) {
			chainCount = 0;
			windowOpen = false;
			chainTimer = 0.f;
		}
	}
}

void ChainSystem::openWindow() {
	windowOpen = true;
	startChainTimer(0.f);
}

void ChainSystem::registerHit() {
	if (windowOpen) {
		chainCount++;
	}
	else chainCount = 1;
}