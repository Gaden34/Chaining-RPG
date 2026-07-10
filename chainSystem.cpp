#include "chainSystem.h"


void ChainSystem::registerHit() {
	if (windowOpen) {
		chainCount++;
	}
	else chainCount = 1;
}