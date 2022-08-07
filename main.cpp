#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include "SDL_Interface.h"
#include "debug.cpp"
#include "Coords.h"
#include "TriangleBoard.h"

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int SCREEN_PADDING_X = 20;
const int SCREEN_PADDING_Y = 20;

TriangleBoard gBoard(21, 10);

//Frees media and shuts down SDL

void renderStaticBoard(
	TriangleRenderer& renderer,
	SDL_Color emptyTriangleColor = { 255, 255, 255, 255 },
	RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationSteps = 0.f) {
	for (int xi = 0; xi < gBoard.width(); ++xi) {
		for (int yi = 0; yi < gBoard.height(); ++yi) {
			if (gBoard.board[xi][yi] == NULL)
				renderer.renderTriangle(LogicalCoordinates{ xi, yi }, emptyTriangleColor, offset, pivot, rotationSteps);
			else
				renderer.renderTriangle(LogicalCoordinates{ xi, yi }, gBoard.board[xi][yi]->color, offset, pivot, rotationSteps);
		}
	}
}

void renderStaticBoardPartially(
	TriangleRenderer& renderer,
	const list<LogicalCoordinates>& trianglesToRender,
	RealCoordinates offset = { 0.f ,0.f }, RealCoordinates pivot = { 0.f ,0.f }, float rotationRadians = 0.f) {
	for (auto logi : trianglesToRender) {
		renderer.renderTriangle(logi, gBoard[logi]->color, offset, pivot, rotationRadians);
	}
}

int main(int argc, char* args[])
{
	
	//Initialize SDL
	if (init())
	{
		TriangleRenderer& renderer = *(getRenderer());

		GameMode currentMode = PLAY;

		gBoard.readFromFile("default.data");

		//TriangleBoard<int> board(2, 2);
		//board[LogicalCoordinates{ 1, 1 }];

		bool quit = false;
		SDL_Event e;
		//SDL_RenderCopy(gRenderer, gKeyPressTextures[KEY_PRESS_SURFACE_DEFAULT], NULL, NULL);

		renderStaticBoard(renderer);

		list<pair<pair<LogicalCoordinates, LogicalCoordinates>, Triangle*>> trianglesBeingRotated;
		RealCoordinates pivot;
		float currentRotationSteps = 0.f;
		float targetRotationSteps = 0.f;
		bool clockwise = false;
		bool animateRotation = 0;

		auto lastFrame = chrono::system_clock::now();
		while (!quit) {
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
									renderer.getRealMousePosition(), trianglesBeingRotated, pivot, targetRotationSteps, clockwise)) {
									currentRotationSteps = 0.f;
									animateRotation = true;
									lastFrame = chrono::system_clock::now();
								}

							}
							break;
							case SDL_BUTTON_MIDDLE:
							{
								gBoard.markCornersTest(renderer.getRealMousePosition());
							}
							break;
							}
						}
						if (currentMode == EDIT) {
							switch (e.button.button) {
							case SDL_BUTTON_LEFT:
								if (gBoard[renderer.getRealMousePosition()] == NULL)
									gBoard[renderer.getRealMousePosition()] = new Triangle{ SDL_Color{ 255, 0, 0, 255 } };
								else {
									delete gBoard[renderer.getRealMousePosition()];
									gBoard[renderer.getRealMousePosition()] = NULL;
								}
								break;
							}
						}
						break;
					}
				}
			}
			renderStaticBoard(renderer);

			if (animateRotation) {
				auto currentFrame = chrono::system_clock::now();
				chrono::duration<float> elapsed_seconds = currentFrame - lastFrame;
				lastFrame = chrono::system_clock::now();
				currentRotationSteps = min(
					targetRotationSteps, 
					3.f*elapsed_seconds.count() * targetRotationSteps  + (1.f - elapsed_seconds.count()) * currentRotationSteps);

				for (auto triangle : trianglesBeingRotated) {
					renderer.renderTriangle(triangle.first.first, triangle.second->color, { 0.f, 0.f }, pivot, 
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
			renderer.renderGameModeIcon(currentMode);
			renderer.nextFrame();
		}
		delete &renderer;
	}
	quit();
	return 0;
}