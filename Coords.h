#pragma once

#include <cmath>
#include <vector>
#include <cassert>
using namespace std;




bool isUpsideDown(int x, int y) {
	return (x + 1000000) % 2 == 0;
}

int f2i(float x) {
	if (x >= 0)
		return static_cast<int>(x + 0.5f);
	else
		return static_cast<int>(x - 0.5f);
}

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

bool isUpsideDown(LogicalCoords logi) {
	return isUpsideDown(logi.x, logi.y);
}

RealCoords RC(const CarthesianCoords& cart) {
	RealCoords real;
	real.x = cart.x + 0.5f * cart.y;
	real.y = sqrt(3.f) * cart.y / 2.f;
	return real;
}

CarthesianCoords CR(const RealCoords& real) {
	CarthesianCoords cart;
	cart.x = real.x - real.y / sqrt(3.f);
	cart.y = 2.f * real.y / sqrt(3.f);
	return cart;
}

CarthesianCoords CL(const LogicalCoords& logi) {
	if (isUpsideDown(logi)) {
		return CarthesianCoords{ float(logi.x / 2 + 1 - logi.y), float(logi.y + 1) };
	}
	else {
		return CarthesianCoords{ float((logi.x - 1) / 2 + 1 - logi.y), float(logi.y) };
	}
}

// maps a point within a triangle to the root corner of the triangle (bottom left or top right, depending on the orientation of the triangle)
LogicalCoords LC(const CarthesianCoords& cart) {
	LogicalCoords logi;
	CarthesianCoords root{ floor(cart.x), floor(cart.y) };
	if (cart.x + cart.y > root.x + root.y + 1.f) { // upside down
		root.x = ceil(cart.x);
		root.y = ceil(cart.y);
		logi.y = f2i(root.y - 1);
		logi.x = 2 * (f2i(root.x) - 1 + logi.y);
	}
	else { // right side up
		logi.y = f2i(root.y);
		logi.x = 2 * (f2i(root.x) - 1 + logi.y) + 1;
	}
	return logi;
}

RealCoords RL(const LogicalCoords& logi) {
	return RC(CL(logi));
}

vector<RealCoords> getTriangleCorners(const LogicalCoords& logi) {
	auto cart = CL(logi);
	if (isUpsideDown(logi)) {
		return
		{
			RC(CarthesianCoords{cart.x, cart.y}),
			RC(CarthesianCoords{cart.x - 1, cart.y}),
			RC(CarthesianCoords{cart.x, cart.y - 1}),
		};
	}
	else {
		return
		{
			RC(CarthesianCoords{cart.x, cart.y}),
			RC(CarthesianCoords{cart.x + 1, cart.y}),
			RC(CarthesianCoords{cart.x, cart.y + 1}),
		};
	}
}

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

	//LogicalCoords ToLogi(const RealCoords& real, bool isUpsideDown) const
	//{
	//	LogicalCoords logi;
	//	float Y = real.y * board[0].size();
	//	Y /= stretch
	//	Y -= isUpsideDown ? 1.f / 6.f : -1.f / 6.f;
	//	logi.x = f2i(real.x * board.size());
	//	logi.y = f2i(Y);
	//	return logi;
	//}

	//RealCoords ToReal(const LogicalCoords& logi) const
	//{
	//	RealCoords real;
	//	real.x = logi.x;
	//	real.x /= width();
	//	real.y = logi.y + (isUpsideDown(logi) ? 1.f / 6.f : -1.f / 6.f);
	//	real.y *= stretch;
	//	real.y /= height();
	//	return real;
	//}
};