#include "Model.h"
#include<fstream>
#include<sstream>
void Model::Initialize(const std::string& directoryPath, const std::string& filename)
{
    dxCommon_ = DirectXCommon::GetInstance();
    engine_ = BlueMoon::GetInstance();
    textureManager_ = Texturemanager::GetInstance();
    modelData_ = LoadObjFile(directoryPath, filename);
    texture_ = textureManager_->Load(modelData_.material.textureFilePath);
    directionalLight_ = DirectionalLight::GetInstance();
    CreateVartexData();
    SetColor();
    material_->enableLighting = true;

}

void Model::Draw(const WorldTransform& transform, const ViewProjection& viewProjection)
{
    Transform uvTransform = { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };

    Matrix4x4 uvtransformMtrix = MakeScaleMatrix(uvTransform.scale);
    uvtransformMtrix = Multiply(uvtransformMtrix, MakeRotateZMatrix(uvTransform.rotate.z));
    uvtransformMtrix = Multiply(uvtransformMtrix, MakeTranslateMatrix(uvTransform.translate));


    *material_ = { color,material_->enableLighting };
    material_->uvTransform = uvtransformMtrix;

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
    dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));
    //Draw
    dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);


}
void Model::Finalize()
{

}

Model* Model::CreateModelFromObj(const std::string& directoryPath, const std::string& filename)
{
    Model* model = new Model();
    model->Initialize(directoryPath, filename);
    return model;
}
ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
    ModelData modelData;//構築するモデルデータ
    std::vector<Vector4> positions;//位置
    std::vector<Vector3> normals;//法線
    std::vector<Vector2> texcoords;//テクスチャ座標
    std::string line;//ファイルから読んだ一行を格納する

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;//先頭の識別子を読む

        //identifierに応じた処理
        if (identifier == "v") {
            Vector4 position;
            s >> position.x >> position.y >> position.z;
            position.z *= -1.0f;
            position.w = 1.0f;
            positions.push_back(position);
        }
        else if (identifier == "vt") {
            Vector2 texcoord;
            s >> texcoord.x >> texcoord.y;
            texcoord.y = 1.0f - texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (identifier == "vn") {
            Vector3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normal.z *= -1.0f;
            normals.push_back(normal);
        }
        else if (identifier == "f") {
            VertexData triangle[3];
            //面は三角形限定 その他は未対応
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                s >> vertexDefinition;
                //頂点の要素へのIndexは[位置/UV/法線]で格納されているので、分解してIndexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndeices[3];
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(v, index, '/');//  /区切りでIndexを積んでいく
                    elementIndeices[element] = std::stoi(index);
                }
                //要素へのIndexから、実際の要素の値を取得して、頂点を構築する
                Vector4 position = positions[elementIndeices[0] - 1];
                Vector2 texcoord = texcoords[elementIndeices[1] - 1];
                Vector3 normal = normals[elementIndeices[2] - 1];
                VertexData vertex = { position,texcoord,normal };
                //modelData.vertices.push_back(vertex);
                triangle[faceVertex] = { position,texcoord,normal };

            }
            modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
        }
        else if (identifier == "mtllib") {
            //materialTemplateLibraryファイルの名前を取得
            std::string materialFilname;
            s >> materialFilname;
            //基本的にobjファイルと同一階層にmtlは存在させるから、ディレクトリ名とファイル名を渡す
            modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilname);
        }

    }
    return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
    MaterialData materialData;//構築するマテリアルデータ
    std::string line;
    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());
    while (std::getline(file, line))
    {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        //identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFilname;
            s >> textureFilname;
            //連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFilname;
        }
    }
    return materialData;
}

void Model::CreateVartexData()
{

    vertexResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(VertexData) * modelData_.vertices.size());


    vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

    vertexBufferView.SizeInBytes = sizeof(VertexData) * (UINT)modelData_.vertices.size();

    vertexBufferView.StrideInBytes = sizeof(VertexData);

    vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::SetColor()
{
    materialResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Material));

    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->uvTransform = MakeIdentity4x4();

}

void Model::TransformMatrix()
{
    wvpResource_ = DirectXCommon::CreateBufferResource(dxCommon_->GetDevice().Get(), sizeof(Transformmatrix));
    wvpResource_->Map(0, NULL, reinterpret_cast<void**>(&wvpData_));
    wvpData_->WVP = MakeIdentity4x4();
}


