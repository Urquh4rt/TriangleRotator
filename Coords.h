#pragma once

#include <vector>
#include <list>
#include <algorithm>
using namespace std;

bool isUpsideDown(int x, int y);

int f2i(float x);

template<class T, typename number>
struct FloatCoordinates {
public:
	number x;
	number y;

	T operator+ (const T& real) const {
		return T{ x + real.x, y + real.y };
	}

	T operator- (const T& real) const {
		return T{ x - real.x, y - real.y };
	}

	T operator* (float c) const {
		return T{ c * x, c * y };
	}

	T operator/ (float c) const {
		return T{ x / c, y / c };
	}

	bool operator== (const T& p) { return p.x == this->x && p.y == this->y; }

	number dot(const T& t) const {
		return x * t.x + y * t.y;
	}
};

struct LogicalCoordinates;
struct RealCoordinates;
struct BarycentricCoordinates;

struct LogicalCoordinates : FloatCoordinates<LogicalCoordinates, int> {
	operator BarycentricCoordinates() const;
	operator RealCoordinates() const;
};

struct RealCoordinates : public FloatCoordinates<RealCoordinates, float> {
	operator BarycentricCoordinates() const;
	operator LogicalCoordinates() const;
};

struct BarycentricCoordinates : public FloatCoordinates<BarycentricCoordinates, float> {
	operator RealCoordinates() const;
	// maps a point within a triangle to the root corner of the triangle (bottom left or top right, depending on the orientation of the triangle)
	operator LogicalCoordinates() const;
};

bool isUpsideDown(LogicalCoordinates logi);

vector<RealCoordinates> getTriangleCorners(const LogicalCoordinates& logi);

RealCoordinates getCenter(vector<RealCoordinates> reals);

RealCoordinates rotate(const RealCoordinates& real, const RealCoordinates& pivot, float angle);

LogicalCoordinates rotate(const LogicalCoordinates& logi, const LogicalCoordinates& pivot, int angle);

float Dot(const RealCoordinates& a, const RealCoordinates& b);

vector<float> getWeights(const RealCoordinates& p, const RealCoordinates& a, const RealCoordinates& b, const RealCoordinates& c);

vector<LogicalCoordinates> neighborCandidates(const LogicalCoordinates& root, int width, int height);

//// first = left, second = right
////	  r
//// c ____
////   \  /
////  l \/
//pair<LogicalCoordinates, LogicalCoordinates> neighborsOfCorner(LogicalCoordinates triangle, BarycentricCoordinates corner) {
//	auto allCorners = getTriangleCorners(triangle);
//}