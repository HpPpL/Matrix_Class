#include <iostream>
#include "matrix.h"

using namespace linalg;
int main() {
	Matrix<int> matr_1 = { {10, 20, 30}, {40, 50, 60} };
	Matrix<char> matr_2 = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
	matr_1 *= matr_2;
	std::cout << matr_1;
}