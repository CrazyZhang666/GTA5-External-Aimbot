#include "utils.h"

#include <cmath>

#pragma comment(lib, "d3dx9.lib")

D3DXVECTOR2 Utils::screenCenter = { 0.5f * GetSystemMetrics(SM_CXSCREEN), 0.5f * GetSystemMetrics(SM_CYSCREEN) };

std::optional<D3DXVECTOR2> Utils::WorldToScreen(D3DXVECTOR3 worldPos, const viewport_t& viewport)
{
	auto viewmatrix = viewport.viewmatrix;
	D3DXMatrixTranspose(&viewmatrix, &viewmatrix);
	auto vec_x = D3DXVECTOR4(viewmatrix._21, viewmatrix._22, viewmatrix._23, viewmatrix._24),
		vec_y = D3DXVECTOR4(viewmatrix._31, viewmatrix._32, viewmatrix._33, viewmatrix._34),
		vec_z = D3DXVECTOR4(viewmatrix._41, viewmatrix._42, viewmatrix._43, viewmatrix._44);

	auto screen_pos = D3DXVECTOR3((vec_x.x * worldPos.x) + (vec_x.y * worldPos.y) + (vec_x.z * worldPos.z) + vec_x.w,
		(vec_y.x * worldPos.x) + (vec_y.y * worldPos.y) + (vec_y.z * worldPos.z) + vec_y.w,
		(vec_z.x * worldPos.x) + (vec_z.y * worldPos.y) + (vec_z.z * worldPos.z) + vec_z.w);

	if (screen_pos.z <= 0.001f)
		return {};

	screen_pos.z = 1.0f / screen_pos.z;
	screen_pos.x *= screen_pos.z;
	screen_pos.y *= screen_pos.z;

	screen_pos.x = screenCenter.x * (1.0f + screen_pos.x);
	screen_pos.y = screenCenter.y * (1.0f - screen_pos.y);
	return D3DXVECTOR2(screen_pos.x, screen_pos.y);
}

float Utils::Distance(D3DXVECTOR2 a, D3DXVECTOR2 b)
{
	D3DXVECTOR2 relative = a - b;
	return D3DXVec2Length(&relative);
}

float Utils::Distance(D3DXVECTOR3 a, D3DXVECTOR3 b)
{
	D3DXVECTOR3 relative = a - b;
	return D3DXVec3Length(&relative);
}

D3DXVECTOR2 Utils::ClampVector2Length(D3DXVECTOR2 vct, float maxLength)
{
	float currentLength = D3DXVec2Length(&vct);
	if (currentLength > maxLength)
	{
		vct *= maxLength / currentLength;
	}
	return vct;
}

D3DXVECTOR3 Utils::GetBonePosition(const Ped& ped, int boneId)
{
	auto matrix = *reinterpret_cast<D3DXMATRIX*>(&(Ped&)ped.obj.matrix);
	auto bone = *reinterpret_cast<D3DXVECTOR3*>(&(Ped&)ped.obj.bones[boneId]);
	D3DXVECTOR4 transform;
	D3DXVec3Transform(&transform, &bone, &matrix);
	return D3DXVECTOR3(transform.x, transform.y, transform.z);
}
