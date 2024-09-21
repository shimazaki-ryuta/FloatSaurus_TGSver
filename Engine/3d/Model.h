#pragma once
#pragma region standardInclude
#include <string>
#pragma endregion
#include "DirectXCommon.h"
#include"MyMath.h"
#include "BlueMoon.h"
#include "worldTransform.h"
#include "ViewProjection.h"
#include "Texturemanager.h"
#include"DirectionalLight.h"
class Model
{
public:
	void Initialize(const std::string& directoryPath, const std::string& filename);
	void Draw(const WorldTransform& transform, const ViewProjection& viewProjection);
	void Finalize();

	static Model* CreateModelFromObj(const std::string& directoryPath, const std::string& filename);
	ModelData modelData_;
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	void SetColor(Vector4 col) { color = col; }
	void setIsLighting(bool flag) { material_->enableLighting = flag; }

	Material* material_;
private:
	Texturemanager* textureManager_;
	DirectXCommon* dxCommon_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	VertexData* vertexData_;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Transformmatrix* wvpData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	BlueMoon* engine_;
	uint32_t texture_;
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	DirectionalLight* directionalLight_;



private:
	void CreateVartexData();
	void SetColor();
	void TransformMatrix();


};

