#include "TriangleBoard.h"
#include <string>
#include <algorithm>
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

		auto destination = rotate(
			logi,
			LogicalCoordinates(corners[distance(weights.begin(), max_element(weights.begin(), weights.end()))]),
			clockwise ? 1 : -1);
		if ((*this)[destination] == NULL) {
			(*this)[destination] = (*this)[logi];
			(*this)[logi] = NULL;
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

bool TriangleBoard::rotateTriangles(list<LogicalCoordinates>& triangles, LogicalCoordinates pivot, bool clockwise) {
	for (int i = 0; i < 5; ++i) {
		for (auto it = triangles.begin(); it != triangles.end(); ++it) {
			auto triangle = rotate(*it, pivot, clockwise ? 1 : -1);
			if ((*this)[triangle] == NULL)
				*it = triangle;
			else {
				for (auto it2 = triangles.begin(); it2 != it; ++it2) {
					*it2 = rotate(*it2, pivot, clockwise ? -1 : 1);
				}
				return false;
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
