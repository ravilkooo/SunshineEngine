#pragma once

#include <SimpleMath.h>

namespace DX = DirectX;
namespace DXSM = DX::SimpleMath;

inline void DecomposeTransform(
	DXSM::Matrix& fullTransform,
	DXSM::Vector3& scale,
	DXSM::Vector3& rotation,
	DXSM::Vector3& translation)
{
	translation = DXSM::Vector3(fullTransform._41, fullTransform._42, fullTransform._43);
	scale = DXSM::Vector3::One;
	scale.x = DXSM::Vector3(fullTransform._11, fullTransform._12, fullTransform._13).Length();
	scale.y = DXSM::Vector3(fullTransform._21, fullTransform._22, fullTransform._23).Length();
	scale.z = DXSM::Vector3(fullTransform._31, fullTransform._32, fullTransform._33).Length();
	
	auto rotateMatrix = DXSM::Matrix(fullTransform);
	rotateMatrix._41 = 0.0f; rotateMatrix._42 = 0.0f; rotateMatrix._43 = 0.0f;
	rotateMatrix._11 /= scale.x; rotateMatrix._12 /= scale.x; rotateMatrix._13 /= scale.x;
	rotateMatrix._21 /= scale.y; rotateMatrix._22 /= scale.y; rotateMatrix._23 /= scale.y;
	rotateMatrix._31 /= scale.z; rotateMatrix._32 /= scale.z; rotateMatrix._33 /= scale.z;

	auto q = DXSM::Quaternion::CreateFromRotationMatrix(rotateMatrix);
	auto eu = q.ToEuler();
	rotation = DXSM::Vector3(eu.y, eu.x, eu.z);
}