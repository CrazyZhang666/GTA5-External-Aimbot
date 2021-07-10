#pragma once

#include "natives.h"
#include "objectwithaddr.h"

#include <optional>
#include <d3dx9math.h>

class Utils
{
public:
	static std::optional<D3DXVECTOR2> WorldToScreen(D3DXVECTOR3 worldPos, const viewport_t& viewport);
	static float Distance(D3DXVECTOR2 a, D3DXVECTOR2 b);
	static float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b);
	static D3DXVECTOR2 ClampVector2Length(D3DXVECTOR2 vct, float maxLength);

	static D3DXVECTOR3 GetBonePosition(const Ped& ped, int boneId);

	static D3DXVECTOR2 screenCenter;
};
