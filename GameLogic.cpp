#include "GameLogic.h"

void simpleRotationAroundSelectedCorner(TriangleBoard& board, RealCoordinates mouseLocation, bool clockwise)
{
	auto bary = CR(mouseLocation);
	auto logi = LR(mouseLocation);
	if (board[logi] != 0) {
		auto corners = getTriangleCorners(logi);
		auto weights = getWeights(mouseLocation, corners[0], corners[1], corners[2]);

		auto destination = rotate(
			logi,
			LR(corners[distance(weights.begin(), max_element(weights.begin(), weights.end()))]),
			clockwise ? 1 : -1);
		if (board[destination] == NULL) {
			board[destination] = board[logi];
			board[logi] = NULL;
		}
	}
}

void markCornersTest(TriangleBoard& board, RealCoordinates mouseLocation) {
	auto bary = CR(mouseLocation);
	auto logi = LR(mouseLocation);
	list<LogicalCoordinates> subgraph;
	bool success = board.getSubgraph(
		logi,
		LogicalCoordinates{ logi.x + 1, logi.y },
		subgraph,
		LogicalCoordinates{ logi.x - 1, logi.y });
	if (success) {
		for (auto logi : subgraph) {
			board[logi]->color = SDL_Color{ 0, 255, 0, 255 };
		}
	}
	else {
		printf("Cycle.\n");
	}
}
