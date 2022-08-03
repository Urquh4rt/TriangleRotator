#pragma once

#include <fstream>
#include <SDL.h>
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

	bool getSubgraph( 
		const LogicalCoordinates& root, 
		const LogicalCoordinates& subroot, 
		list<LogicalCoordinates>& result,
		const LogicalCoordinates& pivot = LogicalCoordinates{ -1, -1 });

	void simpleRotationAroundSelectedCorner(RealCoordinates mouseLocation, bool clockwise);

	void markCornersTest(RealCoordinates mouseLocation);

	void rotateOnClick(RealCoordinates mouseLocation);

	bool rotateTriangles(list<LogicalCoordinates>& triangles, LogicalCoordinates pivot, bool clockwise);

	void writeToFile();

	void readFromFile();
};