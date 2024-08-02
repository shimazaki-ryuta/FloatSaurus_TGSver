#include "DirectionalLight.h"
DirectionalLight* DirectionalLight::GetInstance()
{
	static DirectionalLight instance;
	return &instance;
}
void DirectionalLight::Initialize()
{
	directXCommon_ = DirectXCommon::GetInstance();

	Resource = directXCommon_->CreateBufferResource(directXCommon_->GetDevice().Get(), sizeof(DirectionalLightData));
	Resource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
}

void DirectionalLight::Update()
{
	directionalLightData->direction = Normalise(directionalLightData->direction);
}
