#include "SDL_Interface.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdexcept>
using namespace std;

struct TriangleRendererImplementation : TriangleRenderer {
	SDL_Window* window;
	SDL_Renderer* renderer;

	const int screen_width;
	const int screen_height;

	const int screen_padding_x;
	const int screen_padding_y;

	vector<SDL_Texture*> iconTextures;

	TriangleRendererImplementation(int width, int height, int padding_x, int padding_y);
	~TriangleRendererImplementation();
	SDL_Texture* getModeIcon(string text);

	SDL_FPoint FPointFromReal(const RealCoordinates& real) {
		return SDL_FPoint{ (real.x - .5f) * screen_width / 12 + screen_padding_x, screen_height - (real.y) * screen_width / 12 - screen_padding_y };
	}

	RealCoordinates RealFromFPoint(const SDL_FPoint& fp) {
		return RealCoordinates{ (fp.x - screen_padding_x) * 12 / screen_width + .5f, (screen_height - fp.y - screen_padding_y) * 12 / screen_width };
	}

	void renderTriangle(LogicalCoordinates logi, const SDL_Color& color, RealCoordinates offset, RealCoordinates pivot, float rotationSteps, float padding) {
		auto corners = getTriangleCorners(logi);
		vector<SDL_Vertex> vertices;
		RealCoordinates center = (corners[0] + corners[1] + corners[2]) / 3.f;
		vertices.reserve(3);
		for (int i = 0; i < 3; ++i) {
			vertices.push_back({
				FPointFromReal((center * padding + corners[i] * (1.f - padding) - pivot)
				.rotate(rotationSteps) + pivot + offset), color, SDL_FPoint{ 0 } });
		}
		if (SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0) == -1) {
			printf("Could not render: %s\n", IMG_GetError());
		}
	}

	void renderGameModeIcon(GameMode mode) {
		{
			SDL_Rect Message_rect; //create a rect
			Message_rect.x = 0;  //controls the rect's x coordinate 
			Message_rect.y = 0; // controls the rect's y coordinte
			Message_rect.w = 25; // controls the width of the rect
			Message_rect.h = 50; // controls the height of the rect
			SDL_RenderCopy(renderer, iconTextures[mode], NULL, &Message_rect);
		}
	}

	RealCoordinates getRealMousePosition() {
		int x, y;
		SDL_GetMouseState(&x, &y);
		return RealFromFPoint(SDL_FPoint{ float(x),float(y) });
	}

	void nextFrame() {
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
	}
};


TriangleRenderer* getRenderer(int width, int height, int padding_x, int padding_y) {
	return new TriangleRendererImplementation(width, height, padding_x, padding_y);
}

bool init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		throw runtime_error("SDL could not initialize! SDL_Error: " + string(SDL_GetError()) + "\n");
	}
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_Image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		throw runtime_error("SDL_Image could not initialize! SDL_image Error: " + string(IMG_GetError()) + "\n");
	}
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		throw runtime_error("SDL_ttf could not initialize! SDL_ttf Error: " + string(TTF_GetError()) + "\n");
	}
	return true;
}

bool quit() {

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return true;
}

TriangleRendererImplementation::TriangleRendererImplementation(int width, int height, int padding_x, int padding_y) :
	screen_width(width), screen_height(height), screen_padding_x(padding_x), screen_padding_y(padding_y) {

	//Create window
	window = SDL_CreateWindow("Triangle Rotator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		throw runtime_error("Window could not be created! SDL_Error: " + string(SDL_GetError()) + "\n");
	}
	else
	{
		//Create renderer for window
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			throw runtime_error("Renderer could not be created! SDL Error: " + string(SDL_GetError()) + "\n");
		}
		else {
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		}
	}

	iconTextures = {
	   getModeIcon("P"),
	   getModeIcon("E"),
	};
}

TriangleRendererImplementation::~TriangleRendererImplementation() {

	for (auto texture : iconTextures) {
		SDL_DestroyTexture(texture);
	}

	SDL_DestroyRenderer(renderer);

	//Destroy window
	SDL_DestroyWindow(window);
}

inline SDL_Texture* TriangleRendererImplementation::getModeIcon(string text) {	//this opens a font style and sets a size
	TTF_Font* Sans = TTF_OpenFont("media/OpenSans-Regular.ttf", 24);

	// this is the color in rgb format,
	// maxing out all would give you the color white,
	// and it will be your text's color
	SDL_Color White = { 255, 255, 255 };

	// as TTF_RenderText_Solid could only be used on
	// SDL_Surface then you have to create the surface first
	SDL_Surface* surfaceMessage =
		TTF_RenderText_Solid(Sans, text.c_str(), White);

	// now you can convert it into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	return Message;
}