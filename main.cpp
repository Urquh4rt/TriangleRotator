#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include "debug.cpp"
#include "Coords.h"
#include "TriangleBoard.h"

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int SCREEN_PADDING_X = 20;
const int SCREEN_PADDING_Y = 20;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

TriangleBoard gBoard(21, 10);

//Starts up SDL and creates window
bool init() {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("Triangle Rotator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				return false;
			}
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_Image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					return false;
				}
				else if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					return false;
				}
			}
		}
		return true;
	}
}

SDL_Texture* getModeIcon(string text) {	//this opens a font style and sets a size
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
	SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	return Message;
}
vector<SDL_Texture*> iconTextures;

//Frees media and shuts down SDL
void close() {

	for (auto texture : iconTextures) {
		SDL_DestroyTexture(texture);
	}

	SDL_DestroyRenderer(gRenderer);

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

SDL_FPoint FPointFromReal(const RealCoordinates& real) {
	return SDL_FPoint{ (real.x - .5f) * SCREEN_WIDTH / 12 + SCREEN_PADDING_X, SCREEN_HEIGHT - (real.y) * SCREEN_WIDTH / 12 - SCREEN_PADDING_Y };
}

RealCoordinates RealFromFPoint(const SDL_FPoint& fp) {
	return RealCoordinates{ (fp.x - SCREEN_PADDING_X) * 12 / SCREEN_WIDTH + .5f, (SCREEN_HEIGHT - fp.y - SCREEN_PADDING_Y) * 12 / SCREEN_WIDTH };
}

RealCoordinates getRealMousePosition() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return RealFromFPoint(SDL_FPoint{ float(x),float(y) });
}

void renderTriangle(
		SDL_Renderer* renderer, LogicalCoordinates logi, const SDL_Color& color, 
		RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationSteps = 0.f, float padding = 0.1f
	) {
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

void renderStaticBoard(
	SDL_Renderer* renderer, SDL_Color emptyTriangleColor = { 255, 255, 255, 255 },
	RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationSteps = 0.f) {
	for (int xi = 0; xi < gBoard.width(); ++xi) {
		for (int yi = 0; yi < gBoard.height(); ++yi) {
			if (gBoard.board[xi][yi] == NULL)
				renderTriangle(renderer, LogicalCoordinates{ xi, yi }, emptyTriangleColor, offset, pivot, rotationSteps);
			else
				renderTriangle(renderer, LogicalCoordinates{ xi, yi }, gBoard.board[xi][yi]->color, offset, pivot, rotationSteps);
		}
	}
}

void renderStaticBoardPartially(
	SDL_Renderer* renderer, const list<LogicalCoordinates>& trianglesToRender,
	RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationRadians = 0.f) {
	for (auto logi : trianglesToRender) {
		renderTriangle(renderer, logi, gBoard[logi]->color, offset, pivot, rotationRadians);
	}
}

enum GameMode {
	PLAY,
	EDIT,
};
GameMode currentMode = PLAY;

int main(int argc, char* args[])
{
	//Initialize SDL
	if (init())
	{

		iconTextures = {
		   getModeIcon("P"),
		   getModeIcon("E"),
		};

		gBoard.readFromFile("default.data");

		//TriangleBoard<int> board(2, 2);
		//board[LogicalCoordinates{ 1, 1 }];

		bool quit = false;
		SDL_Event e;
		//SDL_RenderCopy(gRenderer, gKeyPressTextures[KEY_PRESS_SURFACE_DEFAULT], NULL, NULL);

		renderStaticBoard(gRenderer);

		list<pair<pair<LogicalCoordinates, LogicalCoordinates>, Triangle*>> trianglesBeingRotated;
		RealCoordinates pivot;
		float currentRotationSteps = 0.f;
		float targetRotationSteps = 0.f;
		bool clockwise = false;
		bool animateRotation = 0;

		auto lastFrame = chrono::system_clock::now();
		while (!quit) {
			SDL_RenderClear(gRenderer);
			if (!animateRotation) {
				while (SDL_PollEvent(&e) != 0) {
					switch (e.type) {
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_KEYDOWN:
						switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_e:
							currentMode = EDIT;
							break;
						case SDLK_p:
							currentMode = PLAY;
							break;
						case SDLK_s:
							gBoard.writeToFile();
							break;
						case SDLK_o:
							gBoard.readFromFile();
							break;
						}
						break;
					case SDL_MOUSEBUTTONDOWN:
						if (currentMode == PLAY) {
							switch (e.button.button) {
							case SDL_BUTTON_LEFT:
							case SDL_BUTTON_RIGHT:
							{
								//gBoard.simpleRotationAroundSelectedCorner(getRealMousePosition(), e.button.button == SDL_BUTTON_LEFT);
								trianglesBeingRotated.clear();
								if (gBoard.complexRotationAroundSelectedCorner(
									getRealMousePosition(), trianglesBeingRotated, pivot, targetRotationSteps, clockwise)) {
									currentRotationSteps = 0.f;
									animateRotation = true;
									lastFrame = chrono::system_clock::now();
								}

							}
							break;
							case SDL_BUTTON_MIDDLE:
							{
								gBoard.markCornersTest(getRealMousePosition());
							}
							break;
							}
						}
						if (currentMode == EDIT) {
							switch (e.button.button) {
							case SDL_BUTTON_LEFT:
								if (gBoard[getRealMousePosition()] == NULL)
									gBoard[getRealMousePosition()] = new Triangle{ SDL_Color{ 255, 0, 0, 255 } };
								else {
									delete gBoard[getRealMousePosition()];
									gBoard[getRealMousePosition()] = NULL;
								}
								break;
							}
						}
						break;
					}
				}
			}
			renderStaticBoard(gRenderer);

			{
				SDL_Rect Message_rect; //create a rect
				Message_rect.x = 0;  //controls the rect's x coordinate 
				Message_rect.y = 0; // controls the rect's y coordinte
				Message_rect.w = 25; // controls the width of the rect
				Message_rect.h = 50; // controls the height of the rect
				SDL_RenderCopy(gRenderer, iconTextures[currentMode], NULL, &Message_rect);
			}
			if (animateRotation) {
				auto currentFrame = chrono::system_clock::now();
				chrono::duration<float> elapsed_seconds = currentFrame - lastFrame;
				lastFrame = chrono::system_clock::now();
				currentRotationSteps = min(
					targetRotationSteps, 
					3.f*elapsed_seconds.count() * targetRotationSteps  + (1.f - elapsed_seconds.count()) * currentRotationSteps);

				for (auto triangle : trianglesBeingRotated) {
					renderTriangle(gRenderer, triangle.first.first, triangle.second->color, { 0.f, 0.f }, pivot, 
						clockwise ? currentRotationSteps : -currentRotationSteps);
				}
				if (abs(currentRotationSteps - targetRotationSteps) < 0.01f) {
					cout << "done" << endl;
					animateRotation = false;
					for (auto triangle : trianglesBeingRotated) {
						gBoard[triangle.first.second] = triangle.second;
					}
				}
			}
			SDL_RenderPresent(gRenderer);
		}
	}
	else {
		printf("Media could not be loaded!");
		return 1;
	}
	close();
	return 0;
}