#pragma once
#include "DirectXCommon.h"
#include"MyMath.h"
#include "BlueMoon.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "Worldtransform.h"
#include "ViewProjection.h"
#include "SrvDescriptorheap.h"
#include<random>
#include <list>
class Particle
{
public:
	void Initialize(uint32_t Drawparticle);
	void Draw(const ViewProjection& viewProjection, const Vector4& material, uint32_t index);
	void Update();
	void AddColorUpdate(Vector4 color);
	void Finalize();
	void AddParticle(const Emitter& emitter, const int& count);
	void Cler();
	std::list<ParticleData>* GetParticleDate() { return &particles_; };
private:
	void SettingVertex();
	void SetColor();
	void TransformMatrix();
	void CreateSRV(uint32_t num);
private:
	Texturemanager* textureManager_;
	SrvDescriptorHeap* srvHeap_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	DirectXCommon* direct_;
	VertexData* vertexData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	Transformmatrix* wvpData_;
	DirectionalLight* directionalLight_;
	static	const uint32_t kNumMaxInstance_ = 100000;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	ParticleForGPU* instancingData;
	std::list<ParticleData>particles_;
	uint32_t index_;
	int knumInstance_;
	uint32_t DrawInstanceNum_;
	int instanceCount;
private:
	ParticleData MakeNewParticle(const Emitter& emitter, std::mt19937& randomEngine);
};

