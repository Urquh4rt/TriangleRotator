#define _USE_MATH_DEFINES
#include <cmath>
#include <cassert>
#include "Coords.h"


bool isUpsideDown(int x, int y) {
	return (x + y + 1000000) % 2 == 0;
}

int f2i(float x) {
	if (x >= 0)
		return static_cast<int>(x + 0.5f);
	else
		return static_cast<int>(x - 0.5f);
}

bool isUpsideDown(LogicalCoordinates logi) {
	return isUpsideDown(logi.x, logi.y);
}

vector<RealCoordinates> getTriangleCorners(const LogicalCoordinates& logi) {
	BarycentricCoordinates bary = logi;
	if (isUpsideDown(logi)) {
		return
		{
			BarycentricCoordinates{ bary.x, bary.y },
			BarycentricCoordinates{ bary.x - 1, bary.y },
			BarycentricCoordinates{ bary.x, bary.y - 1 },
		};
	}
	else {
		return
		{
			BarycentricCoordinates{ bary.x, bary.y },
			BarycentricCoordinates{ bary.x + 1, bary.y },
			BarycentricCoordinates{ bary.x, bary.y + 1 },
		};
	}
}

RealCoordinates getCenter(vector<RealCoordinates> reals) {
	RealCoordinates center{ 0.f, 0.f };
	for (auto real : reals) {
		center = center + real / float(reals.size());
	}
	return center;
}

RealCoordinates rotate(const RealCoordinates& real, const RealCoordinates& pivot, float radians) {
	auto relative = real - pivot;
	float coss = cos(radians), sinn = sin(radians);
	return RealCoordinates{ coss * relative.x + sinn * relative.y, -sinn * relative.x + coss * relative.y } + pivot;
}

LogicalCoordinates rotate(const LogicalCoordinates& logi, const LogicalCoordinates& pivot, int angle) {
	return rotate(getCenter(getTriangleCorners(logi)), RealCoordinates(pivot), angle * M_PI / 3.f);
}

[[deprecated]]
float Dot(const RealCoordinates& a, const RealCoordinates& b) {
	return a.x * b.x + a.y * b.y;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
vector<float> getWeights(const RealCoordinates& p, const RealCoordinates& a, const RealCoordinates& b, const RealCoordinates& c)
{
	RealCoordinates v0 = b - a, v1 = c - a, v2 = p - a;
	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);
	float d20 = v2.dot(v0);
	float d21 = v2.dot(v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	return { u, v, w };
}

vector<LogicalCoordinates> neighborCandidates(const LogicalCoordinates& root, int width, int height) {
	vector<LogicalCoordinates> result;
	result.reserve(3);
	if (root.x > 0)
		result.push_back(LogicalCoordinates{ root.x - 1, root.y });
	if (root.x < width - 1)
		result.push_back(LogicalCoordinates{ root.x + 1, root.y });
	if (isUpsideDown(root)) {
		if (root.y < height - 1)
			result.push_back(LogicalCoordinates{ root.x, root.y + 1 });
	}
	else {
		if (root.y > 0)
			result.push_back(LogicalCoordinates{ root.x, root.y - 1 });
	}
	return result;
}

BarycentricCoordinates::operator RealCoordinates() const { 
	return RealCoordinates{ x + 0.5f * y, sqrt(3.f) * y / 2.f };
}

BarycentricCoordinates::operator LogicalCoordinates() const {
	LogicalCoordinates logi;
	BarycentricCoordinates root{ floor(x), floor(y) };
	if (x + y > root.x + root.y + 1.f) { // upside down
		root.x = ceil(x);
		root.y = ceil(y);
		logi.y = f2i(root.y - 1);
		logi.x = 2 * (f2i(root.x) - 1 + logi.y) - logi.y;
	}
	else { // right side up
		logi.y = f2i(root.y);
		logi.x = 2 * (f2i(root.x) - 1 + logi.y) + 1 - logi.y;
	}
	return logi;
}

LogicalCoordinates::operator BarycentricCoordinates() const {
	if (isUpsideDown(*this)) {
		return BarycentricCoordinates{ float((x + y) / 2 + 1 - y), float(y + 1) };
	}
	else {
		return BarycentricCoordinates{ float((x + y - 1) / 2 + 1 - y), float(y) };
	}
}

inline LogicalCoordinates::operator RealCoordinates() const {
	return BarycentricCoordinates(*this);
}

RealCoordinates::operator BarycentricCoordinates() const { 
	return BarycentricCoordinates{ x - y / sqrt(3.f), 2.f * y / sqrt(3.f) };
}

inline RealCoordinates::operator LogicalCoordinates() const {
	return BarycentricCoordinates(*this);
}
