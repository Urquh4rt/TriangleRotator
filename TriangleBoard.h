#pragma once

#include <fstream>
#include <SDL.h>
#include <functional>
#include "Coords.h"

using namespace std;
struct Triangle {
	SDL_Color color;
};
struct TriangleBoard {
	vector<vector<Triangle*>> board;

	TriangleBoard(int width, int height) : board(vector<vector<Triangle*>>(width, vector<Triangle*>(height))) {}

	int width() const
	{
		return board.size();
	}
	int height() const
	{
		return board.size() > 0 ? board[0].size() : 0;
	}

	Triangle*& operator[](const LogicalCoordinates& logi) {
		return board[logi.x][logi.y];
	}

	vector<Triangle*>& operator[](int x) {
		return board[x];
	}

	bool isInBounds(const LogicalCoordinates& logi) {
		return logi.x >= 0 && logi.x < width() && logi.y >= 0 && logi.y < height();
	}

	bool isEmpty(const LogicalCoordinates& logi) {
		return isInBounds(logi) && (*this)[logi] == NULL;
	}

	bool getSubgraph( 
		const LogicalCoordinates& root, 
		const LogicalCoordinates& subroot, 
		list<LogicalCoordinates>& result,
		const LogicalCoordinates& pivot = LogicalCoordinates{ -7, -7 });

	void simpleRotationAroundSelectedCorner(RealCoordinates mouseLocation, bool clockwise);

	bool complexRotationAroundSelectedCorner(RealCoordinates mouseLocation);

	void markCornersTest(RealCoordinates mouseLocation);

	void rotateOnClick(RealCoordinates mouseLocation);

	bool rotateTriangles(list<LogicalCoordinates>& triangles, const RealCoordinates& pivot, bool clockwise);

	void writeToFile();

	void readFromFile();
};