#include "Sprite.h"

void Sprite::Initialize(const Vector4& a, const Vector4& b)
{
	dxCommon_ = DirectXCommon::GetInstance();
	engine_ = BlueMoon::GetInstance();
	textureManager_ = Texturemanager::GetInstance();
	CreateVartexData(a, b);
	SetColor();
	CreateTransform();

}
void Sprite::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->uvTransform = MakeIdentity4x4();
}
void Sprite::Draw(const Transform& transform, const Transform& uvTransform, const Vector4& material, uint32_t texIndex)
{

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionmatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)dxCommon_->GetWin()->kClientWidth, (float)dxCommon_->GetWin()->kClientHeight, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionmatrix));


	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));
	*materialData_ = { material,false };
	materialData_->uvTransform = uvtransformMtrix;
	*wvpData_ = worldViewProjectionMatrix;

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);
	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texIndex));
	//dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
	dxCommon_->GetCommandList()->DrawInstanced(6, 1, 0, 0);

}


void Sprite::Finalize()
{

}

void Sprite::CreateVartexData(const Vector4& a, const Vector4& b)
{
	vertexResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(VertexData) * 6);

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;

	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	indexResource_ = dxCommon_->CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(uint32_t) * 6);

	indexBufferView.BufferLocation = indexResource_->GetGPUVirtualAddress();

	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;

	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	////座標
	vertexData_[0].position = { a.x,b.y,0.0f,1.0f };
	vertexData_[1].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[2].position = { b.x,b.y,0.0f,1.0f };
	vertexData_[3].position = { a.x,a.y,0.0f,1.0f };
	vertexData_[4].position = { b.x,a.y,0.0f,1.0f };
	vertexData_[5].position = { b.x,b.y,0.0f,1.0f };

	//texcoord
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].texcoord = { 0.0f,0.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };
	vertexData_[3].texcoord = { 0.0f,0.0f };
	vertexData_[4].texcoord = { 1.0f,0.0f };
	vertexData_[5].texcoord = { 1.0f,1.0f };
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 4;
	indexData_[4] = 2;
	indexData_[5] = 3;
	for (int i = 0; i < 6; i++) {
		vertexData_[i].normal = { 0.0f,0.0f,-1.0f };
	}

}

void Sprite::CreateTransform()
{

	wvpResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Matrix4x4));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	*wvpData_ = MakeIdentity4x4();
}
