#include "Shader.h"

Matrix transformObjToWorld = Matrix();
Matrix transformObjToWorldNormal = Matrix();
Matrix transformWorldToView = Matrix();
Matrix transformViewToClip = Matrix();
Vec3f lightDir = Vec3f();
Vec3f cameraPos = Vec3f();