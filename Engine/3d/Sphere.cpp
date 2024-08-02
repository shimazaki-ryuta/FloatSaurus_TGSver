#include "Sphere.h"
#include<cmath>
void Sphere::Initialize()
{
	dxCommon_ = DirectXCommon::GetInstance();
	engine_ = BlueMoon::GetInstance();
	textureManager_ = Texturemanager::GetInstance();
	directionalLight_ = DirectionalLight::GetInstance();
	kSubDivision = 32;
	vertexCount = kSubDivision * kSubDivision * 6;
	CreateVartexData();

	SetColor();

}



void Sphere::Draw(const Vector4& material, const WorldTransform& transform, uint32_t texIndex, const ViewProjection& viewProjection)
{


	Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

	Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));



	*materialData_ = {material,materialData_->enableLighting };
	materialData_->uvTransform = uvtransformMtrix;


	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//形状を設定。PS0にせっていしているものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//worldTransform
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transform.constBuff_->GetGPUVirtualAddress());
	//viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	//Light
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLight_->GetResource()->GetGPUVirtualAddress());
	//texture
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texIndex));
	//Draw
	dxCommon_->GetCommandList()->DrawInstanced(vertexCount, 1, 0, 0);


}

void Sphere::Finalize()
{

}

void Sphere::CreateVartexData()
{
	vertexResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(VertexData) * vertexCount);


	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	vertexBufferView.SizeInBytes = sizeof(VertexData) * vertexCount;

	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));


	//経度一つ分の角度
	const float kLonEvery = pi * 2.0f / float(kSubDivision);
	//緯度一つ分の角度
	const float kLatEvery = pi / float(kSubDivision);
	//緯度方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubDivision + lonIndex) * 6;

			float lon = lonIndex * kLonEvery;
			/*頂点a*/vertexData_[start].position = { cos(lat) * cos(lon),sin(lat),cos(lat) * sin(lon),1.0f };
			vertexData_[start].texcoord = { float(lonIndex) / float(kSubDivision),1.0f - float(latIndex) / kSubDivision };

			/*頂点b*/vertexData_[start + 1].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 1].texcoord = { vertexData_[start].texcoord.x,vertexData_[start].texcoord.y - 1.0f / float(kSubDivision) };

			/*頂点c*/vertexData_[start + 2].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 2].texcoord = { vertexData_[start].texcoord.x + 1.0f / float(kSubDivision),vertexData_[start].texcoord.y };

			/*頂点c*/vertexData_[start + 3].position = { cos(lat) * cos(lon + kLonEvery),sin(lat),cos(lat) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 3].texcoord = { vertexData_[start].texcoord.x + 1.0f / float(kSubDivision),vertexData_[start].texcoord.y };

			/*頂点b*/vertexData_[start + 4].position = { cos(lat + kLatEvery) * cos(lon),sin(lat + kLatEvery),cos(lat + kLatEvery) * sin(lon),1.0f };
			vertexData_[start + 4].texcoord = { vertexData_[start].texcoord.x,vertexData_[start].texcoord.y - 1.0f / float(kSubDivision) };

			/*頂点d*/vertexData_[start + 5].position = { cos(lat + kLatEvery) * cos(lon + kLonEvery),sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery),1.0f };
			vertexData_[start + 5].texcoord = { vertexData_[start].texcoord.x + 1.0f / float(kSubDivision),vertexData_[start].texcoord.y - 1.0f / float(kSubDivision) };

			vertexData_[start].normal.x = vertexData_[start].position.x;
			vertexData_[start].normal.y = vertexData_[start].position.y;
			vertexData_[start].normal.z = vertexData_[start].position.z;
			vertexData_[start + 1].normal.x = vertexData_[start + 1].position.x;
			vertexData_[start + 1].normal.y = vertexData_[start + 1].position.y;
			vertexData_[start + 1].normal.z = vertexData_[start + 1].position.z;
			vertexData_[start + 2].normal.x = vertexData_[start + 2].position.x;
			vertexData_[start + 2].normal.y = vertexData_[start + 2].position.y;
			vertexData_[start + 2].normal.z = vertexData_[start + 2].position.z;
			vertexData_[start + 3].normal.x = vertexData_[start + 3].position.x;
			vertexData_[start + 3].normal.y = vertexData_[start + 3].position.y;
			vertexData_[start + 3].normal.z = vertexData_[start + 3].position.z;
			vertexData_[start + 4].normal.x = vertexData_[start + 4].position.x;
			vertexData_[start + 4].normal.y = vertexData_[start + 4].position.y;
			vertexData_[start + 4].normal.z = vertexData_[start + 4].position.z;
			vertexData_[start + 5].normal.x = vertexData_[start + 5].position.x;
			vertexData_[start + 5].normal.y = vertexData_[start + 5].position.y;
			vertexData_[start + 5].normal.z = vertexData_[start + 5].position.z;




		}
	}

}



void Sphere::TransformMatrix()
{
	wvpResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Transformmatrix));
	wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
}

void Sphere::SetColor() {
	materialResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Material));

	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->uvTransform = MakeIdentity4x4();
}
