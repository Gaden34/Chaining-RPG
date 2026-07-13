#pragma once


class ChainSystem
{
private:
	int chainCount = 0;
	bool windowOpen = false;
	float chainTimer = 0.f;

public:
	void startChainTimer(float dt);
	void openWindow();
	void closeWindow() { windowOpen = false; }
	void registerHit();
	void reset() { chainCount = 0; }
	int getChainCount() const { return chainCount; }
	bool isWindowOpen() const { return windowOpen; }
};

