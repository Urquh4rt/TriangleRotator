#include <iostream>
#include <vector>
#include <list>
#include "Coords.h"

using namespace std;

template<typename T>
void print(const T& real, string suffix = "") {
	//printf("real%s: %f %f,", suffix, real.x, real.y);
	cout << "real" << suffix << ": " << real.x << " " << real.y << ", ";
}

#define PRINTCOORDS(var) cout << #var << " = " << (var).x << " " << (var).y << ", ";
#define PRINT(var) cout << #var << " = " << var << ", ";

template<typename T>
void print(const vector<T>& reals) {
	for (int i = 0; i < reals.size(); ++i) {
		print(reals[i], "" + i);
	}
}

template<typename T>
void print(const list<T>& reals) {
	print(vector<T>(reals.begin(), reals.end()));
}