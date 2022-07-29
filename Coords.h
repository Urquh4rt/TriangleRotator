#pragma once

#include <vector>
using namespace std;

bool isUpsideDown(int x, int y);

int f2i(float x);

struct RealCoordinates {
	float x;
	float y;

	RealCoordinates operator+ (const RealCoordinates& real) const {
		return RealCoordinates{ x + real.x, y + real.y };
	}

	RealCoordinates operator- (const RealCoordinates& real) const {
		return RealCoordinates{ x - real.x, y - real.y };
	}

	RealCoordinates operator* (float c) const {
		return RealCoordinates{ c * x, c * y };
	}

	RealCoordinates operator/ (float c) const {
		return RealCoordinates{ x / c, y / c };
	}
};

struct BarycentricCoordinates {
	float x;
	float y;
};

struct LogicalCoordinates {
	int x;
	int y;

	bool operator== (const LogicalCoordinates& p) { return p.x == this->x && p.y == this->y; }
};

bool isUpsideDown(LogicalCoordinates logi);

RealCoordinates RC(const BarycentricCoordinates& bary);

BarycentricCoordinates CR(const RealCoordinates& real);

BarycentricCoordinates CL(const LogicalCoordinates& logi);

// maps a point within a triangle to the root corner of the triangle (bottom left or top right, depending on the orientation of the triangle)
LogicalCoordinates LC(const BarycentricCoordinates& bary);

RealCoordinates RL(const LogicalCoordinates& logi);

LogicalCoordinates LR(const RealCoordinates& real);

vector<RealCoordinates> getTriangleCorners(const LogicalCoordinates& logi);

RealCoordinates getCenter(vector<RealCoordinates> reals);

RealCoordinates rotate(const RealCoordinates& real, const RealCoordinates& pivot, float angle);

LogicalCoordinates rotate(const LogicalCoordinates& logi, const LogicalCoordinates& pivot, int angle);

float Dot(const RealCoordinates& a, const RealCoordinates& b);

vector<float> getWeights(const RealCoordinates& p, const RealCoordinates& a, const RealCoordinates& b, const RealCoordinates& c);

using Triangle = int;
struct TriangleBoard {
	vector<vector<Triangle>> board;

	TriangleBoard(int width, int height) : board(vector<vector<int>>(width, vector<int>(height))) {}

	int width() const
	{
		return board.size();
	}
	int height() const
	{
		return board.size() > 0 ? board[0].size() : 0;
	}

	Triangle& operator[](const LogicalCoordinates& logi);
};