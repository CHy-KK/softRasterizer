#include "Model.h"

// transformObjToWorld
//Matrix Model::transformMatrix()
//{
//	Matrix scale = Matrix::identity(3) * scaleSize;
//
//	float alpha = degrees_to_radians(eulaAngle.x);
//	float beta = degrees_to_radians(eulaAngle.y);
//	float gamma = degrees_to_radians(eulaAngle.z);
//
//	Matrix rx({
//		{1, 0, 0},
//		{0, cos(alpha), -sin(alpha)},
//		{0, sin(alpha), cos(alpha)},
//	});
//
//	Matrix ry({
//		{cos(beta), 0, sin(beta)},
//		{0, 1, 0},
//		{-sin(beta), 0, cos(beta)}
//	});
//
//	Matrix rz({
//		{cos(gamma), -sin(gamma), 0},
//		{sin(gamma), cos(gamma), 0},
//		{0, 0, 1}
//	});
//
//	scale = rz * ry * rx * scale;
//
//	return Matrix ({
//		{ scale[0][0], scale[0][1], scale[0][2], worldPos.x },
//		{ scale[1][0], scale[1][1], scale[1][2], worldPos.y },
//		{ scale[2][0], scale[2][1], scale[2][2], worldPos.z },
//		{0, 0, 0, 1}
//	});
//}
