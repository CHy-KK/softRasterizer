#include "Matrix.h"

Matrix Matrix::identity(int dim)
{
    Matrix mat(dim, dim);
    for (int i = 0; i < dim; i++)
        mat[i][i] = 1.f;
    return mat;
}

vector<float>& Matrix::operator[](const int i) const
{
	assert(i >= 0 && i < rows);
	return m[i];
}

Matrix Matrix::operator*(const Matrix& a)
{
    assert(cols == a.rows);
    Matrix result(rows, a.cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k = 0; k < cols; k++) {
                result.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
    }
    return result;
}

Matrix Matrix::operator*(const float n)
{
    for (int i = 0; i < rows; i++) 
        for (int j = 0; j < cols; j++) 
            m[i][j] *= n;
    return m;
}

Vec4f Matrix::operator*(const Vec4f& v)
{
    assert(rows == 4 && cols == 4);
    Vec4f res;
    for (int i = 0; i < 4; i++) {
        res[i] = m[i][0] * v.x + m[i][1] * v.y + m[i][2] * v.z + m[i][3] * v.w;
    }
    return res;
}

Vec3f Matrix::operator*(const Vec3f& v)
{
    assert(rows == 3 && cols == 3);
    Vec4f res;
    for (int i = 0; i < 3; i++) {
        res[i] = m[i][0] * v.x + m[i][1] * v.y + m[i][2] * v.z;
    }
    return res;
}

Matrix Matrix::transpose()
{
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix Matrix::inverse()
{
    assert(rows == cols);
    // augmenting the square matrix with the identity matrix of the same dimensions A => [AI]
    Matrix result(rows, cols * 2);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result[i][j] = m[i][j];
    for (int i = 0; i < rows; i++)
        result[i][i + cols] = 1;
    // first pass
    for (int i = 0; i < rows - 1; i++) {
        // normalize the first row
        for (int j = result.cols - 1; j >= 0; j--)
            result[i][j] /= result[i][i];
        for (int k = i + 1; k < rows; k++) {
            float coeff = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // normalize the last row
    for (int j = result.cols - 1; j >= rows - 1; j--)
        result[rows - 1][j] /= result[rows - 1][rows - 1];
    // second pass
    for (int i = rows - 1; i > 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            float coeff = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            truncate[i][j] = result[i][j + cols];
    return truncate;
}

//std::ostream& operator<<(std::ostream& s, const Matrix& m)
//{
//}

inline Matrix identityMatrix(int dimensions)
{
    Matrix mat(dimensions, dimensions);
    for (int i = 0; i < dimensions; i++)
        mat[i][i] = 1.f;
    return mat;
}
