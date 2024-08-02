#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "myMath.h"
#include"DirectXCommon.h" 
class DirectionalLight
{
public:
	static DirectionalLight* GetInstance();
	void Initialize();
	void Update();
	ID3D12Resource* GetResource() { return Resource.Get(); }
private:
	DirectXCommon* directXCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	DirectionalLightData* directionalLightData = nullptr;
};

