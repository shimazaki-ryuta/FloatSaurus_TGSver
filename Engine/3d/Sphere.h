#pragma once
#include "DirectXCommon.h"
#include"MyMath.h"
#include "BlueMoon.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include "Texturemanager.h"
#include"DirectionalLight.h"
class Sphere
{
public:
	void Initialize();


	//void Draw(const Vector4& material, const Transform& transform, uint32_t texIndex, const Transform& cameraTransform, const DirectionalLight& light);
	void Draw(const Vector4& material, const WorldTransform& transform, uint32_t texIndex, const ViewProjection& viewProjection);
	void Finalize();
	void setIsLighting(bool flag) { materialData_->enableLighting = flag; }
private:
	Texturemanager* textureManager_;
	DirectionalLight* directionalLight_;
	DirectXCommon* dxCommon_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Transformmatrix* wvpData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_;
	BlueMoon* engine_;
	const float pi = 3.14159265358979f;
	uint32_t kSubDivision;
	uint32_t vertexCount;




private:
	void CreateVartexData();
	void SetColor();
	void TransformMatrix();

};

