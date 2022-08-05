#pragma once

#include <vector>
#include <list>
#include <algorithm>
#include <concepts>
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

// returns the 3 corners of a Triangle in clockwise order with the top/bottom corner at index 1
vector<RealCoordinates> getTriangleCorners(const LogicalCoordinates& logi);

template<class T>
T getCenter(vector<T> coords) {
	static_assert(derived_from<T, FloatCoordinates<T, float>> == true);
	T center{ 0.f, 0.f };
	for (auto coord : coords) {
		center = center + coord / float(coords.size());
	}
	return center;
}

RealCoordinates rotate(const RealCoordinates& real, const RealCoordinates& pivot, float angle);

LogicalCoordinates rotate(const LogicalCoordinates& logi, const RealCoordinates& pivot, int angle);

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