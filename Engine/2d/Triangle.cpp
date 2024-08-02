#include "Triangle.h"
#include <assert.h>
#include "BlueMoon.h"

void Triangle::Initialize()
{
	direct_ = DirectXCommon::GetInstance();
	Engine = BlueMoon::GetInstance();
	textureManager_ = Texturemanager::GetInstance();
	directionalLight_ = DirectionalLight::GetInstance();
	SettingVertex();
	SetColor();

}
void Triangle::TransformMatrix()
{
	wvpResource_ = DirectXCommon::CreateBufferResource(direct_->GetDevice().Get(), sizeof(Transformmatrix));

	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();

}
void Triangle::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(direct_->GetDevice().Get(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

}

void Triangle::Draw(const WorldTransform& transform, const ViewProjection& viewProjection, const Vector4& material)
{
	Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));



	*materialData_ = { material,false };
	materialData_->uvTransform = uvtransformMtrix;

	direct_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定すると考えておけばいい
	direct_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//material
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//worldTransform
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform.constBuff_->GetGPUVirtualAddress());

	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	//Light
	direct_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLight_->GetResource()->GetGPUVirtualAddress());

	//texture
	direct_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(0));


	//描画！(DrawCall/ドローコール)・3頂点で1つのインスタンス。インスタンスについては今後
	direct_->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}
void Triangle::Finalize()
{

}

void Triangle::SettingVertex() {

	vertexResource_ = DirectXCommon::CreateBufferResource(direct_->GetDevice().Get(), sizeof(VertexData) * 3);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	vertexData_[0].position = { -0.5f,0.0f,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };

	//上
	vertexData_[1].position = { 0.0f,1.0f,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.5f,0.0f };

	//右下
	vertexData_[2].position = { -0.5f,0.0f,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
}
