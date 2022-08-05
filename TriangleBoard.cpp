#include "TriangleBoard.h"
#include <string>
#include <algorithm>
#include <iostream>
#include "FileSelector.h"

bool TriangleBoard::getSubgraph(const LogicalCoordinates& root, const LogicalCoordinates& subroot, list<LogicalCoordinates>& result, const LogicalCoordinates& pivot) {
	vector<vector<bool>> visited(width(), vector<bool>(height()));
	visited[root.x][root.y] = true;
	list<LogicalCoordinates> queue;
	queue.push_back(subroot);
	while (!queue.empty()) {
		auto logi = queue.front();
		queue.erase(queue.begin());
		if ((*this)[logi] != NULL && !visited[logi.x][logi.y]) {
			if (logi == pivot)
				return false;
			visited[logi.x][logi.y] = true;
			result.push_back(logi);
			auto neighbors = neighborCandidates(logi, width(), height());
			for (auto neighbor : neighbors)
				queue.push_back(neighbor);
		}
	}
	return true;
}

void TriangleBoard::simpleRotationAroundSelectedCorner(RealCoordinates mouseLocation, bool clockwise)
{
	LogicalCoordinates logi = mouseLocation;
	if ((*this)[logi] != 0) {
		auto corners = getTriangleCorners(logi);
		auto weights = getWeights(mouseLocation, corners[0], corners[1], corners[2]);
		int indexOfClosestCorner = distance(weights.begin(), max_element(weights.begin(), weights.end()));
		auto destination = rotate(
			logi,
			corners[indexOfClosestCorner],
			clockwise ? 1 : -1);
		if ((*this)[destination] == NULL) {
			(*this)[destination] = (*this)[logi];
			(*this)[logi] = NULL;
		}
	}
}

bool TriangleBoard::complexRotationAroundSelectedCorner(RealCoordinates mouseLocation) {
	LogicalCoordinates logi = mouseLocation;
	if ((*this)[logi] != NULL) {
		auto corners = getTriangleCorners(logi);
		auto weights = getWeights(mouseLocation, corners[0], corners[1], corners[2]);
		int indexOfClosestCorner = distance(weights.begin(), max_element(weights.begin(), weights.end()));
		LogicalCoordinates nextNeighborClockwise = getCenter(vector<RealCoordinates>{
			corners[indexOfClosestCorner],
				corners[(indexOfClosestCorner + 1) % 3],
				corners[indexOfClosestCorner] + corners[(indexOfClosestCorner + 1) % 3] - corners[(indexOfClosestCorner + 2) % 3]
		});
		LogicalCoordinates prevNeighborClockwise = getCenter(vector<RealCoordinates>{
			corners[indexOfClosestCorner],
			corners[(indexOfClosestCorner + 2) % 3],
			corners[indexOfClosestCorner] + corners[(indexOfClosestCorner + 2) % 3] - corners[(indexOfClosestCorner + 1) % 3]
		});
		//(*this)[nextNeighborClockwise] = new Triangle{ SDL_Color{ 0, 0, 255, 255 } };
		//(*this)[prevNeighborClockwise] = new Triangle{ SDL_Color{ 0, 255, 255, 255 } };
		if ((*this)[nextNeighborClockwise] != NULL && (*this)[prevNeighborClockwise] != NULL)
			return false;
		if ((*this)[nextNeighborClockwise] == NULL && (*this)[prevNeighborClockwise] == NULL)
			return false;

		bool canRotateClockwise = isEmpty(prevNeighborClockwise);
		bool canRotateCounterClockwise = isEmpty(nextNeighborClockwise);
		if (canRotateClockwise && canRotateCounterClockwise) {
			// rotation direction is ambigous
			return false;
		}
		if (!canRotateClockwise && !canRotateCounterClockwise) {
			// rotation is blocked
			return false;
		}

		LogicalCoordinates oppositeNeighbor = getCenter(vector<RealCoordinates>{
			corners[(indexOfClosestCorner + 1) % 3],
				corners[(indexOfClosestCorner + 2) % 3],
				corners[(indexOfClosestCorner + 1) % 3] + corners[(indexOfClosestCorner + 2) % 3] - corners[indexOfClosestCorner]
		});
		list<LogicalCoordinates> originalTriangles;
		originalTriangles.push_back(logi);
		if (getSubgraph(logi, oppositeNeighbor, originalTriangles, canRotateClockwise ? nextNeighborClockwise : prevNeighborClockwise)) {
			//if (render) {
			//	for (auto triangle : originalTriangles) {
			//		(*this)[triangle] = new Triangle{ SDL_Color{ 0, 255, 255, 255 } };
			//	}
			//	render();
			//	int x;
			//	cin >> x;
			//}
			list<LogicalCoordinates> trianglesRotated = originalTriangles;
			if (rotateTriangles(trianglesRotated, corners[indexOfClosestCorner], canRotateClockwise)) {
				auto it1 = originalTriangles.begin(), it2 = trianglesRotated.begin();
				while (it1 != originalTriangles.end()) {
					(*this)[*it2] = (*this)[*it1];
					(*this)[*it1] = NULL;
					++it1;
					++it2;
				}
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}
}

void TriangleBoard::markCornersTest(RealCoordinates mouseLocation) {
	LogicalCoordinates logi = mouseLocation;
	list<LogicalCoordinates> subgraph;
	bool success = getSubgraph(
		logi,
		LogicalCoordinates{ logi.x + 1, logi.y },
		subgraph,
		LogicalCoordinates{ logi.x - 1, logi.y });
	if (success) {
		for (auto logi : subgraph) {
			//board[logi]->color = SDL_Color{ 0, 255, 0, 255 };
			(*this)[logi] = new Triangle{ SDL_Color{ 0, 255, 0, 255 } };
		}
	}
	else {
		printf("Cycle.\n");
	}
}

void TriangleBoard::rotateOnClick(RealCoordinates mouseLocation) {
	
}

bool TriangleBoard::rotateTriangles(list<LogicalCoordinates>& triangles, const RealCoordinates& pivot, bool clockwise) {
	for (int i = 0; i < 5; ++i) {
		for (auto it = triangles.begin(); it != triangles.end(); ++it) {
			auto triangle = rotate(*it, pivot, clockwise ? 1 : -1);
			if ((*this)[triangle] == NULL)
				*it = triangle;
			else {
				for (auto it2 = triangles.begin(); it2 != it; ++it2) {
					*it2 = rotate(*it2, pivot, clockwise ? -1 : 1);
				}
				return i != 0;
			}

		}
	}
	return true;
}

void TriangleBoard::writeToFile() {
	string path = getSaveFileName();
	ofstream file;
	file.open(path);
	file << width() << " " << height() << endl;
	for (int y = height() - 1; y >= 0; --y) {
		for (int x = 0; x < width(); ++x) {
			file << (board[x][y] == NULL ? 0 : 1) << " ";
		}
		file << endl;
	}
	file.close();

}

void TriangleBoard::readFromFile() {
	string path = getOpenFileName();
	ifstream file;
	file.open(path);
	int w, h;
	file >> w >> h;
	board = vector<vector<Triangle*>>(w, vector<Triangle*>(h));
	for (int y = height() - 1; y >= 0; --y) {
		for (int x = 0; x < width(); ++x) {
			int v;
			file >> v;
			if (v != 0)
				board[x][y] = new Triangle{ SDL_Color{ 255, 0, 0, 255 } };
		}
	}
}
