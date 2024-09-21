#pragma once
#include <math.h>
#include<stdint.h>
#include<cassert>
#include <vector>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES
#include <algorithm>
enum particletype {
	height,
	random,
	absorption,
};
struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};
inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z,v1.w + v2.w };
}
struct Vector3 {
	float x;
	float y;
	float z;
};
struct Vector2 {
	float x;
	float y;
};
struct Matrix4x4 final {
	float m[4][4];
};
struct Matrix3x3 {
	float m[3][3];
};
struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};
struct Transformmatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
struct DirectionalLightData {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

struct MaterialData {
	std::string textureFilePath;
};
struct Emitter {
	Transform transform;
	particletype type;
	Vector4 color;
};

enum ParticleAttribute
{
	NORMAL,
	ABSORPTION,
};

struct ParticleData {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	Emitter emitter;
	float lifeTime;
	float currentTime;
	bool isAlive;
	ParticleAttribute attribute;
	
};
struct ParticleForGPU {
	Matrix4x4 World;
	Vector4 Color;
};
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};
struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 orientation[3];
	Vector3 size;
};
struct StructSphere {
	Vector3 center;
	float radius;
};
inline Vector3 operator-(const Vector3& v) {
	return { -v.x, -v.y, -v.z };
}
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

inline Vector3 operator*(const Vector3& v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}
inline Vector3 operator*(float s, const Vector3& v) {
	return { s * v.x, s * v.y, s * v.z };
}

inline Vector3 operator*(const Vector3& v, const Matrix4x4& m) {
	return {
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2] };
}
Matrix4x4 MakeRotateXMatrix(float theta);
Matrix4x4 MakeRotateYMatrix(float theta);

Matrix4x4 MakeRotateZMatrix(float theta);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
Matrix4x4 MakeBillBoardMatrix(const Vector3& scale, Matrix4x4 billboard, const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//1　行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
//２　行列の減法
Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2);
//３　行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
//４　逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//５　転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//６　単位行列
Matrix4x4 MakeIdentity4x4();
//透視投射行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
float cot(float theta);
// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//ビューポート行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

Vector3 Normalise(const Vector3& v);
Vector3 Add(const Vector3& a, const Vector3& b);


Vector3 Subtract(const Vector3& v1, const Vector3& v2);
Vector3 Multiply(float scalar, const Vector3& v);
float Dot(const Vector3& v1, const Vector3& v2);
float Length(const Vector3& v);
Vector3 vectorTransform(const Vector3& vector, const Matrix4x4& matrix);
Vector3 Slerp(float t, const Vector3& s, const Vector3& e);
Vector3 Lerp(float t, const Vector3& s, const Vector3& e);
Vector3 Distance(const Vector3& v1, const Vector3& v2);
Matrix4x4 MakeRotateMatrix(Vector3 theta);
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
/// obbの衝突判定
inline Matrix4x4& SetTranslate(Matrix4x4& m, const Vector3& v) {
	m.m[3][0] = v.x, m.m[3][1] = v.y, m.m[3][2] = v.z;
	return m;
}

inline Matrix4x4 MakeInverseMatrix(const Matrix4x4& rotate, const Vector3& translate) {
	Matrix4x4 RT = Transpose(rotate);
	return SetTranslate(RT, -translate * RT);
}
inline Matrix4x4 MakeRotateMatrixFromOrientations(const Vector3 orientations[3]) {
	return {
		orientations[0].x,orientations[0].y,orientations[0].z,0.0f,
		orientations[1].x,orientations[1].y,orientations[1].z,0.0f,
		orientations[2].x,orientations[2].y,orientations[2].z,0.0f,
		0.0f,0.0f,0.0f,1.0f };
}
inline bool IsCollision(const AABB& aabb, const StructSphere& sphere) {
	Vector3 clossestPoint{
		std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)
	};

	float distance = Length(Subtract(clossestPoint, sphere.center));

	return distance <= sphere.radius;
}
inline bool IsCollision(const OBB& obb, const StructSphere& sphere) {
	Matrix4x4 obbWorldInverse = MakeInverseMatrix(MakeRotateMatrixFromOrientations(obb.orientation), obb.center);
	Vector3 centerInOBBLocalSpace = sphere.center * obbWorldInverse;
	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	StructSphere sphereObbLocal{ centerInOBBLocalSpace, sphere.radius };

	return IsCollision(aabbOBBLocal, sphereObbLocal);
}
inline bool IsCollision(const OBB& obb1, const OBB& obb2) {
	Vector3 separatingAxis[15];
	separatingAxis[0] = obb1.orientation[0];
	separatingAxis[1] = obb1.orientation[1];
	separatingAxis[2] = obb1.orientation[2];
	separatingAxis[3] = obb2.orientation[0];
	separatingAxis[4] = obb2.orientation[1];
	separatingAxis[5] = obb2.orientation[2];
	int axisNum = 6;
	for (int index1 = 0; index1 < 3; index1++)
	{
		for (int index2 = 0; index2 < 3; index2++)
		{
			separatingAxis[axisNum] = Cross(obb1.orientation[index1], obb2.orientation[index2]);
			axisNum++;
		}
	}

	Vector3 obb1Vetyex[8];
	//bottom
	obb1Vetyex[0] = Vector3{ -obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[1] = Vector3{ +obb1.size.x, -obb1.size.y, -obb1.size.z };
	obb1Vetyex[2] = Vector3{ -obb1.size.x, -obb1.size.y, +obb1.size.z };
	obb1Vetyex[3] = Vector3{ +obb1.size.x, -obb1.size.y, +obb1.size.z };
	//top
	obb1Vetyex[4] = Vector3{ -obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[5] = Vector3{ +obb1.size.x, +obb1.size.y, -obb1.size.z };
	obb1Vetyex[6] = Vector3{ -obb1.size.x, +obb1.size.y, +obb1.size.z };
	obb1Vetyex[7] = Vector3{ +obb1.size.x, +obb1.size.y, +obb1.size.z };

	Matrix4x4 rotateMatrix1 = MakeRotateMatrixFromOrientations(obb1.orientation);

	Vector3 obb2Vetyex[8];
	//bottom
	obb2Vetyex[0] = Vector3{ -obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[1] = Vector3{ +obb2.size.x, -obb2.size.y, -obb2.size.z };
	obb2Vetyex[2] = Vector3{ -obb2.size.x, -obb2.size.y, +obb2.size.z };
	obb2Vetyex[3] = Vector3{ +obb2.size.x, -obb2.size.y, +obb2.size.z };
	//top
	obb2Vetyex[4] = Vector3{ -obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[5] = Vector3{ +obb2.size.x, +obb2.size.y, -obb2.size.z };
	obb2Vetyex[6] = Vector3{ -obb2.size.x, +obb2.size.y, +obb2.size.z };
	obb2Vetyex[7] = Vector3{ +obb2.size.x, +obb2.size.y, +obb2.size.z };

	Matrix4x4 rotateMatrix2 = MakeRotateMatrixFromOrientations(obb2.orientation);

	for (int index = 0; index < 8; index++)
	{
		obb1Vetyex[index] = obb1Vetyex[index] * rotateMatrix1;
		obb1Vetyex[index] = Add(obb1Vetyex[index], obb1.center);
		obb2Vetyex[index] = obb2Vetyex[index] * rotateMatrix2;
		obb2Vetyex[index] = Add(obb2Vetyex[index], obb2.center);
	}

	for (axisNum = 0; axisNum < 15; axisNum++)
	{
		float projectionPoint1[8];
		float projectionPoint2[8];
		float min1, max1;
		float min2, max2;
		for (int index = 0; index < 8; index++)
		{
			projectionPoint1[index] = Dot(obb1Vetyex[index], Normalise(separatingAxis[axisNum]));
			projectionPoint2[index] = Dot(obb2Vetyex[index], Normalise(separatingAxis[axisNum]));
			if (index == 0)
			{
				min1 = projectionPoint1[index];
				min2 = projectionPoint2[index];
				max1 = min1;
				max2 = min2;
			}
			else
			{
				min1 = (std::min)(min1, projectionPoint1[index]);
				min2 = (std::min)(min2, projectionPoint2[index]);
				max1 = (std::max)(max1, projectionPoint1[index]);
				max2 = (std::max)(max2, projectionPoint2[index]);
			}
		}
		float L1 = max1 - min1;
		float L2 = max2 - min2;
		float sumSpan = L1 + L2;
		float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);
		if (sumSpan < longSpan)
		{
			return false;
		}
	}
	return true;
}
inline Vector3 GetXAxis(const Matrix4x4& m) {
	return { m.m[0][0],m.m[0][1],m.m[0][2] };
}
inline Vector3 GetYAxis(const Matrix4x4& m) {
	return { m.m[1][0],m.m[1][1],m.m[1][2] };
}inline Vector3 GetZAxis(const Matrix4x4& m) {
	return { m.m[2][0],m.m[2][1],m.m[2][2] };
}
inline void GetOrientations(const Matrix4x4& m, Vector3 orientations[3]) {
	orientations[0] = GetXAxis(m);
	orientations[1] = GetYAxis(m);
	orientations[2] = GetZAxis(m);
}
inline Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate) {
	Vector3 s = { std::sin(rotate.x), std::sin(rotate.y), std::sin(rotate.z) };
	Vector3 c = { std::cos(rotate.x), std::cos(rotate.y), std::cos(rotate.z) };
	return {
		c.y * c.z,						c.y * s.z,						-s.y,		0.0f,
		s.x * s.y * c.z - c.x * s.z,	s.x * s.y * s.z + c.x * c.z,	s.x * c.y,	0.0f,
		c.x * s.y * c.z + s.x * s.z,	c.x * s.y * s.z - s.x * c.z,	c.x * c.y,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f };
}
inline Vector3 Reflect(const Vector3& input, const Vector3& normal) {
	Vector3 r;
	r = input - 2 * Dot(input, normal) * normal;
	r = r * 0.8f;
	return r;
}

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);

