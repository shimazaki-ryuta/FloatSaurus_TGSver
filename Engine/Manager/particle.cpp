#include "particle.h"
#include <assert.h>
#include "BlueMoon.h"
#include <numbers>
void Particle::Initialize(uint32_t Drawparticle)
{
	direct_ = DirectXCommon::GetInstance();

	textureManager_ = Texturemanager::GetInstance();
	srvHeap_ = SrvDescriptorHeap::GetInstance();
	directionalLight_ = DirectionalLight::GetInstance();
	knumInstance_ = 300;
	SettingVertex();
	SetColor();
	TransformMatrix();
}
void Particle::TransformMatrix()
{


	instancingResource_ = direct_->CreateBufferResource(direct_->GetDevice().Get(), sizeof(ParticleForGPU) * kNumMaxInstance_);
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	CreateSRV(1);
}
void Particle::CreateSRV(uint32_t num)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;//2Dテクスチャ
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = kNumMaxInstance_;
	srvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	//srvHeap_->AddIndex();
	srvHeap_->SetGPUHandle(direct_->GetSrvHeap().Get(), srvHeap_->GetSizeSRV(), 1);//direct_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
	srvHeap_->SetCPUHandle(direct_->GetSrvHeap().Get(), srvHeap_->GetSizeSRV(), 1);//srvHandleCPU_ = textureManager_->GettextureSrvHandleCPU(direct_->GetSrvHeap().Get(), textureManager_->GetSizeSRV(), num);
	//srvHandleGPU_ = textureManager_->GettextureSrvHandleGPU(direct_->GetSrvHeap().Get(), textureManager_->GetSizeSRV(), num);
	index_ = 1;
	direct_->GetDevice().Get()->CreateShaderResourceView(instancingResource_.Get(), &srvDesc, srvHeap_->GetCPUHandle(1));
}
ParticleData Particle::MakeNewParticle(const Emitter& emitter, std::mt19937& randomEngine)
{
	ParticleData particle;
	std::uniform_real_distribution<float>distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float>distri(0.0f, 1.0f);
	std::uniform_real_distribution<float>distrivelocity(-1.0f, 1.0f);
	std::uniform_real_distribution<float>distColor(-1.0f, 1.0f);
	std::uniform_real_distribution<float>distTime(1.0f, 2.0f);
	particle.transform.scale = { 1.5f,1.5f,1.5f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	particle.attribute = NORMAL;
	if (emitter.type == absorption) {
		particle.transform.translate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
		particle.velocity = { distrivelocity(randomEngine) * 8.0f,distrivelocity(randomEngine) * 8.0f,distrivelocity(randomEngine) * 8.0f };
		particle.attribute = ABSORPTION;
	}
	else if (emitter.type == random) {
		particle.transform.translate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
		particle.velocity = { distrivelocity(randomEngine) * 8.0f,distrivelocity(randomEngine) * 8.0f,distrivelocity(randomEngine) * 8.0f };
	}
	else {
		particle.transform.translate = { 0.0f,distri(randomEngine)-1.0f,0.0f};
		particle.velocity ={0.0f,distri(randomEngine) * 4.0f,0.0f };

	}
	particle.color = emitter.color;
	particle.lifeTime = 0.5f;
	particle.currentTime = 0.0f;
	particle.isAlive = true;
	particle.emitter = emitter;
	return particle;
}
void Particle::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(direct_->GetDevice().Get(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

}

void Particle::Draw(const ViewProjection& viewProjection, const Vector4& material, uint32_t index)
{
	Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));
	Matrix4x4 bakToFront = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboard = Multiply(bakToFront, Inverse(viewProjection.matView));
	billboard.m[3][0] = 0.0f;
	billboard.m[3][1] = 0.0f;
	billboard.m[3][2] = 0.0f;
	instanceCount = 0;
	for (std::list<ParticleData>::iterator iterator = particles_.begin();
		iterator != particles_.end(); ++iterator) {


		instancingData[instanceCount].World = MakeBillBoardMatrix((*iterator).transform.scale, billboard, (*iterator).transform.translate); //MakeAffineMatrix(particles_[i].transform.scale, particles_[i].transform.rotate, particles_[i].transform.translate);
		instancingData[instanceCount].World = Multiply(instancingData[instanceCount].World, MakeAffineMatrix((*iterator).emitter.transform.scale, { 0.0f,0.0f,0.0f }, (*iterator).emitter.transform.translate));
		instanceCount++;
	}
	*materialData_ = { material,false };
	materialData_->uvTransform = uvtransformMtrix;

	direct_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定すると考えておけばいい
	direct_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//material
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//worldTransform
	direct_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvHeap_->GetGPUHandle(index_));

	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	//Light
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLight_->GetResource()->GetGPUVirtualAddress());

	//texture
	direct_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(index));


	//描画！(DrawCall/ドローコール)・3頂点で1つのインスタンス。インスタンスについては今後
	direct_->GetCommandList()->DrawInstanced(6,DrawInstanceNum_, 0, 0);

}
void Particle::Update()
{
	const float kDeltTime = 1.0f / 60.0f;
	DrawInstanceNum_ = 0;
	instanceCount = 0;
	for (std::list<ParticleData>::iterator iterator = particles_.begin();
		iterator != particles_.end(); ) {
		if (DrawInstanceNum_ < kNumMaxInstance_) {
			if ((*iterator).lifeTime <= (*iterator).currentTime) {
				(*iterator).isAlive = false;
				iterator = particles_.erase(iterator);
				continue;
			}
			instancingData[instanceCount].World = MakeIdentity4x4();
			(*iterator).transform.translate = Add((*iterator).transform.translate, (*iterator).velocity * kDeltTime);
			(*iterator).currentTime += kDeltTime;
			instancingData[instanceCount].Color = (*iterator).color;
			++DrawInstanceNum_;
			++instanceCount;
			++iterator;
		}
	}
}
void Particle::AddColorUpdate(Vector4 color)
{
	const float kDeltTime = 1.0f / 60.0f;
	DrawInstanceNum_ = 0;
	instanceCount = 0;
	for (std::list<ParticleData>::iterator iterator = particles_.begin();
		iterator != particles_.end(); ) {
		if (DrawInstanceNum_ < kNumMaxInstance_) {
			if ((*iterator).lifeTime <= (*iterator).currentTime) {
				(*iterator).isAlive = false;
				iterator = particles_.erase(iterator);
				continue;
			}
			instancingData[instanceCount].World = MakeIdentity4x4();
			(*iterator).transform.translate = Add((*iterator).transform.translate, (*iterator).velocity * kDeltTime);
			(*iterator).currentTime += kDeltTime;
			instancingData[instanceCount].Color = (*iterator).color + color;
			materialData_->color = (*iterator).color + color;
			++DrawInstanceNum_;
			++instanceCount;
			++iterator;
		}
	}
}


void Particle::Finalize()
{

}

void Particle::AddParticle(const Emitter& emitter, const int& count)
{
	std::random_device seedGenerator_;
	std::mt19937 randomEngine(seedGenerator_());
	instanceCount = 0;
	if (particles_.size() == 0) {
		particles_.push_back(MakeNewParticle(emitter, randomEngine));
		instanceCount++;
	}
	for (int i = 0; i < count; i++) {
		instanceCount = 0;

		for (std::list<ParticleData>::iterator iterator = particles_.begin();
			iterator != particles_.end(); ++iterator) {
			instanceCount++;
			if (instanceCount >= kNumMaxInstance_) {
				break;
			}

			if ((*iterator).isAlive == false) {
				particles_.insert(iterator, MakeNewParticle(emitter, randomEngine));
				instanceCount++;
				continue;
			}

		}
		if (instanceCount >= kNumMaxInstance_) {
			break;
		}
		particles_.push_back(MakeNewParticle(emitter, randomEngine));
		instanceCount++;
		continue;

	}
	++DrawInstanceNum_;
	
}

void Particle::Cler()
{
	particles_.clear();
	DrawInstanceNum_=0;
	instanceCount=0;
}

void Particle::SettingVertex() {

	vertexResource_ = DirectXCommon::CreateBufferResource(direct_->GetDevice().Get(), sizeof(VertexData) * 6);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	vertexData_[0].position = { -0.5f,-0.5f,0.0f,1.0f };

	//上
	vertexData_[1].position = { -0.5f,0.5f,0.0f,1.0f };

	//右下
	vertexData_[2].position = { 0.5f,-0.5f,0.0f,1.0f };

	vertexData_[3].position = { -0.5f,0.5,0.0f,1.0f };
	vertexData_[4].position = { 0.5f,0.5f,0.0f,1.0f };
	vertexData_[5].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };
	for (int i = 0; i < 6; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}
}