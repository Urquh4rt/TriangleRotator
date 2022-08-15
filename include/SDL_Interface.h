#pragma once
#include <SDL_pixels.h>
#include "Coords.h"

enum GameMode {
	PLAY,
	EDIT,
};

class TriangleRenderer {
public:
	virtual ~TriangleRenderer() {

	}

	virtual void renderTriangle(
		LogicalCoordinates logi, const SDL_Color& color,
		RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationSteps = 0.f, float padding = 0.1f
	) = 0;

	virtual void renderGameModeIcon(GameMode mode) = 0;
	virtual RealCoordinates getRealMousePosition() = 0;
	virtual void nextFrame() = 0;
};

TriangleRenderer* getRenderer(int width = 800, int height = 600, int padding_x = 20, int padding_y = 20);

bool init();
bool quit();