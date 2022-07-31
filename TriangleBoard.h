#pragma once

#include "SDL.h"
#include "Coords.h"

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
		const LogicalCoordinates& pivot = LogicalCoordinates{ -1, -1 }) {
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
};