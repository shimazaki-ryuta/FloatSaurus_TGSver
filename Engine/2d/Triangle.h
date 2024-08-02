#pragma once
#include"DirectXCommon.h"
#include"MyMath.h"
#include "Worldtransform.h"
#include "ViewProjection.h"
#include "Texturemanager.h"
#include"DirectionalLight.h"
class BlueMoon;

class Triangle
{
public:
	void Initialize();
	//void Draw(const Transform& transform, const Transform& cameraTransform, const Vector4& material);
	//void Draw(const WorldTransform& transform, const Transform& cameratransform, const Vector4& material);
	void Draw(const WorldTransform& transform, const ViewProjection& viewProjection, const Vector4& material);
	void Finalize();

private:
	void SettingVertex();
	void SetColor();
	void TransformMatrix();

private:
	Texturemanager* textureManager_;
	BlueMoon* Engine;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	DirectXCommon* direct_;
	VertexData* vertexData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Transformmatrix* wvpData_;
	DirectionalLight* directionalLight_;

};

