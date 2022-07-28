#pragma once

#include <cmath>
#include <vector>
#include <cassert>
using namespace std;

bool isUpsideDown(int x, int y);

int f2i(float x);

struct RealCoords {
	float x;
	float y;

	RealCoords operator+ (const RealCoords& real) const {
		return RealCoords{ x + real.x, y + real.y };
	}

	RealCoords operator* (float c) const {
		return RealCoords{ c * x, c * y };
	}

	RealCoords operator/ (float c) const {
		return RealCoords{ x / c, y / c };
	}
};

struct CarthesianCoords {
	float x;
	float y;
};

struct LogicalCoords {
	int x;
	int y;

	bool operator== (const LogicalCoords& p) { return p.x == this->x && p.y == this->y; }
};

bool isUpsideDown(LogicalCoords logi);

RealCoords RC(const CarthesianCoords& cart);

CarthesianCoords CR(const RealCoords& real);

CarthesianCoords CL(const LogicalCoords& logi);

// maps a point within a triangle to the root corner of the triangle (bottom left or top right, depending on the orientation of the triangle)
LogicalCoords LC(const CarthesianCoords& cart);

RealCoords RL(const LogicalCoords& logi);

vector<RealCoords> getTriangleCorners(const LogicalCoords& logi);

struct TriangleBoard {
	vector<vector<int>> board;

	TriangleBoard(int width, int height) : board(vector<vector<int>>(width, vector<int>(height))) {}

	int width() const
	{
		return board.size();
	}
	int height() const
	{
		return board.size() > 0 ? board[0].size() : 0;
	}
};