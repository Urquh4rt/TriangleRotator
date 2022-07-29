/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Coords.h"

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
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
			}
		}
		return true;
	}
}

//Frees media and shuts down SDL
void close() {

	SDL_DestroyRenderer(gRenderer);

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_FPoint FPointFromReal(const RealCoordinates& real) {
	return SDL_FPoint{ (real.x - .5f) * SCREEN_WIDTH / 12 + SCREEN_PADDING_X, SCREEN_HEIGHT - (real.y) * SCREEN_WIDTH / 12 - SCREEN_PADDING_Y };
}

RealCoordinates RealFromFPoint(const SDL_FPoint& fp) {
	return RealCoordinates{ (fp.x - SCREEN_PADDING_X) * 12 / SCREEN_WIDTH + .5f, (SCREEN_HEIGHT - fp.y - SCREEN_PADDING_Y) * 12 / SCREEN_WIDTH };
}

void renderTriangle(SDL_Renderer* renderer, LogicalCoordinates logi, const SDL_Color& color, float padding = 0.1f) {
	auto corners = getTriangleCorners(logi);
	vector<SDL_Vertex> vertices;
	RealCoordinates center = (corners[0] + corners[1] + corners[2]) / 3.f;
	vertices.reserve(3);	
	for (int i = 0; i < 3; ++i) {
		vertices.push_back({ FPointFromReal(center * padding + corners[i] * (1.f - padding)), color, SDL_FPoint{ 0 } });
	}
	if (SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0) == -1) {
		printf("Could not render: %s\n", IMG_GetError());
	}
}

void renderStaticBoard(SDL_Renderer* renderer) {
	for (int xi = 0; xi < gBoard.width(); ++xi) {
		for (int yi = 0; yi < gBoard.height(); ++yi) {
			if (gBoard.board[xi][yi] == 0)
				renderTriangle(renderer, LogicalCoordinates{ xi, yi }, SDL_Color{ 255, 255, 255, 255 }); 
			else
				renderTriangle(renderer, LogicalCoordinates{ xi, yi }, SDL_Color{ 255, 0, 0, 255 });
		}
	}
}

void print(const RealCoordinates& real, string suffix = "") {
	printf("real%s: %f %f,", suffix, real.x, real.y);
}

#define PRINTCOORDS(var) printf("%s = %f %f, ", #var, var.x, var.y);
#define PRINT(var) cout << #var << " = " << var << ", ";

void print(const vector<RealCoordinates>& reals) {
	for (int i = 0; i < reals.size(); ++i) {
		print(reals[i], "" + i);
	}
}

int main(int argc, char* args[])
{
	//Initialize SDL
	if (init())
	{

		gBoard[LogicalCoordinates{ 3,3 }] = 1;

		bool quit = false;
		SDL_Event e;
		//SDL_RenderCopy(gRenderer, gKeyPressTextures[KEY_PRESS_SURFACE_DEFAULT], NULL, NULL);

		renderStaticBoard(gRenderer);

		while (!quit) {
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
					case SDLK_UP:
						break;
					case SDLK_DOWN:
						break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					{
						int x, y;
						SDL_GetMouseState(&x, &y);
						auto real = RealFromFPoint(SDL_FPoint{ float(x),float(y) });
						auto bary = CR(real);
						auto logi = LR(real);
						if (gBoard[logi] != 0) {
							auto corners = getTriangleCorners(logi);
							auto weights = getWeights(real, corners[0], corners[1], corners[2]);

							auto destination = rotate(
								logi, 
								LR(corners[distance(weights.begin(), max_element(weights.begin(), weights.end()))]), 
								e.button.button == SDL_BUTTON_LEFT ? 1 : -1);
							if (gBoard[destination] == 0) {
								gBoard[logi] = 0;
								gBoard[destination] = 1;
							}
						}
					}
					break;
				}				
			}
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				RealCoordinates real(RealFromFPoint(SDL_FPoint{ float(x),float(y) }));
				BarycentricCoordinates bary = CR(real);
				LogicalCoordinates logi = LC(bary);
				//printf("Mouse: Screen: %i %i, Real: %f %f, bary: %f %f, Logi: %i %i\n", x, y, real.x, real.y, bary.x, bary.y, logi.x, logi.y);
			}

			SDL_RenderClear(gRenderer);
			renderStaticBoard(gRenderer);
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