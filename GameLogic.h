#pragma once

#include <SDL.h>
#include <algorithm>
#include "Coords.h"
#include "TriangleBoard.h"

void simpleRotationAroundSelectedCorner(TriangleBoard& board, RealCoordinates mouseLocation, bool clockwise);

void markCornersTest(TriangleBoard& board, RealCoordinates mouseLocation);