#pragma once

#include <string>
#include <SDL.h>
#include <fstream>
#include "Coords.h"
#include "FileSelector.h"

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

	void writeToFile() {
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

	void readFromFile() {
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
};