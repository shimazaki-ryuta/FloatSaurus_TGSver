#pragma once


#include "DirectXCommon.h"

#include"MyMath.h"
#pragma region standardInclude
#include <dxcapi.h>
#include<vector>
#pragma comment(lib,"dxcompiler.lib")
#pragma endregion
enum BlendMode {
	//通常のαブレンド
	kBlendModeNormal,
	//加算ブレンド
	kBlendModeAdd,
	//減産ブレンド
	kBlendModeSubtract,
	//乗算ブレンド
	kBlendModeMultiply,
	//スクリーンブレンド
	kBlendModeScreen,
};
class BlueMoon
{
public:

	static BlueMoon* GetInstance();

	void variableInitialize();
	void Initialize(int32_t width, int32_t height);
	void BeginFrame();
	void EndFrame();
	void Finalize();
	/*void Update();
	void Draw();*/
	~BlueMoon();

	DirectXCommon* GetDirectXCommon() { return direct_; }


	void ModelPreDraw();
	void ModelPreDrawWireFrame();
	void PariclePreDraw();
	void SpritePreDraw();
	void SetBlendMode(int BlendModeNum);
private:


	WinApp* win_;
	DirectXCommon* direct_;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDescParticle{};



	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	D3D12_BLEND_DESC blendDesc_[5]{};
	//3Dパイプライン
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob3D_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob3D_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature3D_;
	IDxcBlob* vertexShaderBlob3D_;
	IDxcBlob* pixelShaderBlob3D_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState3D_;
	//3Dパイプラインワイヤーフレーム
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState3DWireFrame_;
	//2Dパイプライン
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlob2D_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlob2D_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignature2D_;
	IDxcBlob* vertexShaderBlob2D_;
	IDxcBlob* pixelShaderBlob2D_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineState2D_[5];
	D3D12_RASTERIZER_DESC rasterizerDesc2D_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs2D_[2];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc2D_{};
	D3D12_RASTERIZER_DESC rasterizerDesc3D_{};
	D3D12_RASTERIZER_DESC rasterizerDesc3DWireFrame_{};
	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs3D_[3];

	//Particle用のパイプライン
	Microsoft::WRL::ComPtr<ID3DBlob>signatureBlobParticle_;
	Microsoft::WRL::ComPtr<ID3DBlob>errorBlobParticle_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>rootSignatureParticle_;
	IDxcBlob* vertexShaderBlobParticle_;
	IDxcBlob* pixelShaderBlobParticle_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>graphicsPipelineStateParticle_;
	D3D12_RASTERIZER_DESC rasterizerDescParticle_{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescsParticle_[3];

	//頂点リソースにデータを書き込む
	Vector4* vertexData_;
	int PSO2DCount_;


	IDxcBlob* CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePath,
		//Compielerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);
	void InitializeDxcCompiler();
	void CreateRootSignature3D();
	void CreateInputlayOut();
	void SettingBlendState();
	void SettingRasterizerState3D();
	void InitializePSO3D();
	void InitializePSO3DWireFrame();
	void SettingViePort();
	void SettingScissor();
	void SettingDepth();

	void CreateRootSignature2D();
	void SettingRasterizerState2D();
	void InitializePSO2D();
	void CreateInputlayOut2D();

	void CreateRootSignatureParticle();
	void SettingRasterizerStateParticle();
	void InitializePSOParticle();
	void CreateInputlayOutParticle();
};



