#pragma once
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Vec.h"
#include <vector>
#include<cassert>

static int DEFAULT_ALLOC = 4;

using std::vector;
// Matrix 3x3
class Matrix {
private:
	mutable vector<std::vector<float>> m;
	int rows, cols;
public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC):rows(r), cols(c) {
		m.resize(rows, vector<float>(cols, 0));
	}

	Matrix(const vector<vector<float>>& _m): m(_m) {
		rows = _m.size();
		cols = rows > 0 ? _m[0].size() : 0;
	}

	Matrix(const Matrix& _mat) : rows(_mat.nrows()), cols(_mat.ncols()) {
		//std::cout << "copy construct" << std::endl;
		m.resize(rows, vector<float>(cols, 0));
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				m[i][j] = _mat[i][j];
	}

	//Matrix(const Matrix&& _mat) : rows(_mat.nrows()), cols(_mat.ncols()) {
	//	std::cout << "move construct" << std::endl;
	//	for (int i = 0; i < rows; i++)
	//		for (int j = 0; j < cols; j++)
	//			m[i][j] = _mat[i][j];
	//}

	int nrows() const { return rows; }
	int ncols() const { return cols; }
	static Matrix identity(int dim);

	vector<float>& operator[] (const int i) const;
	Matrix operator*(const Matrix& a);
	Matrix operator*(const float n);
	Vec4f operator*(const Vec4f& v);
	Vec3f operator*(const Vec3f& v);
	Matrix transpose();
	Matrix inverse();

};

inline std::ostream& operator<<(std::ostream& s, const Matrix& m) {

	for (int i = 0; i < m.nrows(); i++) {
		for (int j = 0; j < m.ncols(); j++) {
			s << m[i][j];
			if (j < m.ncols() - 1) s << "\t";
		}
		s << "\n";
	}
	return s;
}
inline Matrix identityMatrix(int dimensions);

#endif