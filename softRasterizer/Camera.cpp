#include "Camera.h"

Matrix Camera::transformMatrix()
{
	Matrix rotation = rotationMatrix();
	std::cout << rotation << std::endl;

	cameraForward = normalize(rotation * cameraForward);
	cameraRight = normalize(rotation * cameraRight);
	cameraUp = normalize(rotation * cameraUp);
	std::cout << cameraForward << std::endl;
	std::cout << cameraRight << std::endl;
	std::cout << cameraUp << std::endl;

	Matrix coord({
		{cameraRight.x, cameraRight.y, cameraRight.z, 0},
		{cameraUp.x, cameraUp.y, cameraUp.z, 0},
		{cameraForward.x, cameraForward.y, cameraForward.z, 0},
		{0, 0, 0, 1}
	});

	Matrix translation({
		{1, 0, 0, -worldPos.x},
		{0, 1, 0, -worldPos.y},
		{0, 0, 1, -worldPos.z},
		{0, 0, 0, 1}
	});

	return  coord * translation;
}

Matrix Camera::perspectiveMatrix()
{
	float fovAngle = degrees_to_radians(fov);
	float halfH = tan(fovAngle) * 2;
	float halfW = halfH * aspect;
	return Matrix({
		{1.f / halfW,			0,				   0,					0},
		{		   0, 1.f / halfH,				   0,					0},
		{		   0,			0, (n + f) / (n - f), 2 * n * f / (n - f)},
		{		   0,			0,				  -1,					0}
	});
}

Matrix Camera::othographicMatrix()
{
	/*float fovAngle = degrees_to_radians(fov);
	float halfH = tan(fovAngle) * near * 2;
	float halfW = halfH * aspect;
	return Matrix({
		{1 / halfW, 0, 0, 0},
		{0, 1 / halfH, 0, 0},
		{0, 0, 2 / (near - far), () / (near - far)},
		{0, 0, 0, 1}
	});*/
	return Matrix();
}

