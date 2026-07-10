#pragma once


class ChainSystem
{
private:
	int chainCount = 0;
	bool windowOpen = true;

public:
	void openWindow() { windowOpen = true; }
	void closeWindow() { windowOpen = false; }
	void registerHit();
	void reset() { chainCount = 0; }
	int getChainCount() const { return chainCount; }
	bool isWindowOpen() const { return windowOpen; }
};

