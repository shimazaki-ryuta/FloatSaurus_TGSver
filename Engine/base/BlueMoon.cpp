#include "BlueMoon.h"
#include <assert.h>

void BlueMoon::Initialize(int32_t width, int32_t height) {
	win_ = WinApp::GetInstance();
	direct_ = DirectXCommon::GetInstance();
	direct_->Initialize(win_, win_->kClientWidth, win_->kClientHeight);


	PSO2DCount_ = 0;
	InitializeDxcCompiler();
	CreateRootSignature3D();
	CreateInputlayOut();
	SettingBlendState();
	SettingRasterizerState3D();
	SettingDepth();
	InitializePSO3D();
	InitializePSO3DWireFrame();

	CreateRootSignature2D();
	CreateInputlayOut2D();
	SettingRasterizerState2D();
	for (int i = 0; i < 5; i++) {

		InitializePSO2D();
	}
	CreateRootSignatureParticle();
	CreateInputlayOutParticle();
	SettingRasterizerStateParticle();
	InitializePSOParticle();
	SettingViePort();

	SettingScissor();









}



IDxcBlob* BlueMoon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	//これからシェーダーをコンパイルする旨をログに出す
	Log(ConvertString(std::format(L"Begin CompileShader, path:{},profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	direct_->SetHr(dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource));
	//嫁なかったら決める
	assert(SUCCEEDED(direct_->GetHr()));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
	LPCWSTR arguments[] = {
		filePath.c_str(),//コンパイル対象のhlslファイル名
		L"-E",L"main",//エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile,//ShaderProflieの設定
		L"-Zi",L"-Qembed_debug",//デバッグ用の情報を埋め込む
		L"-Od", //最適化を外しておく
		L"-Zpr",//メモリレイアウトは行優先
	};
	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	direct_->SetHr(dxcCompiler->Compile(
		&shaderSourceBuffer,//読み込んだファイル
		arguments,//コンパイルオプション
		_countof(arguments),//コンパイルオプションの数
		includeHandler, // includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)//コンパイル結果
	));
	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(direct_->GetHr()));

	//警告・エラーが出たらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		//警告・エラーダメ絶対
		assert(false);
	}
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	direct_->SetHr(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr));
	assert(SUCCEEDED(direct_->GetHr()));
	//成功したログを出す
	Log(ConvertString(std::format(L"Compile Succeeded, path:{},profile:{}\n", filePath, profile)));
	//もう使わないリソースを開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
}

void BlueMoon::InitializeDxcCompiler()
{
	HRESULT hr;
	dxcUtils_ = nullptr;
	dxcCompiler_ = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));
	//現時点でincludeはしないが、includeに対応するための設定を行っていく
	includeHandler_ = nullptr;
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

}
void BlueMoon::CreateRootSignature3D() {
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	//worldtransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	//viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号を1にバインド

	D3D12_DESCRIPTOR_RANGE descriptoraRange[1] = {};
	descriptoraRange[0].BaseShaderRegister = 0;
	descriptoraRange[0].NumDescriptors = 1;
	descriptoraRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使用
	descriptoraRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Descriptortableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixcelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptoraRange;//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRange);

rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixcelShaderを使う
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//０～１の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	


	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//シリアライズしてバイナリにする
	signatureBlob3D_ = nullptr;
	errorBlob3D_ = nullptr;
	HRESULT hr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob3D_, &errorBlob3D_);
	if (FAILED(direct_->GetHr())) {
		Log(reinterpret_cast<char*>(errorBlob3D_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	
	hr = direct_->GetDevice()->CreateRootSignature(0, signatureBlob3D_->GetBufferPointer(),
		signatureBlob3D_->GetBufferSize(), IID_PPV_ARGS(&rootSignature3D_));
	assert(SUCCEEDED(hr));
}
void BlueMoon::CreateInputlayOut() {
	//inputElementDescsをメンバ変数にすると治った
	inputElementDescs3D_[0].SemanticName = "POSITION";
	inputElementDescs3D_[0].SemanticIndex = 0;
	inputElementDescs3D_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs3D_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs3D_[1].SemanticName = "TEXCOORD";
	inputElementDescs3D_[1].SemanticIndex = 0;
	inputElementDescs3D_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs3D_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs3D_[2].SemanticName = "NORMAL";
	inputElementDescs3D_[2].SemanticIndex = 0;
	inputElementDescs3D_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs3D_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs3D_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs3D_);
}
void BlueMoon::SettingBlendState() {

	//すべての色要素を書き込む
	//ノーマルブレンド
	blendDesc_[kBlendModeNormal].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_[kBlendModeNormal].RenderTarget[0].BlendEnable = TRUE;
	blendDesc_[kBlendModeNormal].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_[kBlendModeNormal].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeNormal].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc_[kBlendModeNormal].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeNormal].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeNormal].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//加算ブレンド
	blendDesc_[kBlendModeAdd].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_[kBlendModeAdd].RenderTarget[0].BlendEnable = TRUE;
	blendDesc_[kBlendModeAdd].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_[kBlendModeAdd].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeAdd].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeAdd].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeAdd].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeAdd].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//減算ブレンド
	blendDesc_[kBlendModeSubtract].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].BlendEnable = TRUE;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeSubtract].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//乗算ブレンド
	blendDesc_[kBlendModeMultiply].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].BlendEnable = TRUE;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_COLOR;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeMultiply].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	//スクリーンブレンド
	blendDesc_[kBlendModeScreen].RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_[kBlendModeScreen].RenderTarget[0].BlendEnable = TRUE;
	blendDesc_[kBlendModeScreen].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc_[kBlendModeScreen].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeScreen].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeScreen].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_[kBlendModeScreen].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_[kBlendModeScreen].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}
void BlueMoon::SettingRasterizerState3D() {

	//裏面（時計回り）を表示しない
	rasterizerDesc3D_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc3D_.FillMode = D3D12_FILL_MODE_SOLID;
	//裏面（時計回り）を表示しない
	rasterizerDesc3DWireFrame_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc3DWireFrame_.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//Shaderをコンパイルする
	vertexShaderBlob3D_ = CompileShader(L"Resource/hlsl/Object3d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob3D_ != nullptr);


	pixelShaderBlob3D_ = CompileShader(L"Resource/hlsl/Object3d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob3D_ != nullptr);
}
void BlueMoon::InitializePSO3D() {

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature3D_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob3D_->GetBufferPointer(),
		vertexShaderBlob3D_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob3D_->GetBufferPointer(),
		pixelShaderBlob3D_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_[kBlendModeNormal];//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc3D_;//rasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	
	HRESULT hr = direct_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState3D_));
	assert(SUCCEEDED(hr));
}

void BlueMoon::InitializePSO3DWireFrame()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature3D_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob3D_->GetBufferPointer(),
		vertexShaderBlob3D_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob3D_->GetBufferPointer(),
		pixelShaderBlob3D_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_[kBlendModeNormal];//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc3DWireFrame_;//rasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成

	HRESULT hr = direct_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState3DWireFrame_));
	assert(SUCCEEDED(hr));
}

void BlueMoon::SettingViePort() {
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApp::kClientWidth;
	viewport_.Height = WinApp::kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}
void BlueMoon::SettingScissor() {
	//基本的にビューポートと同じく敬が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}
void BlueMoon::SettingDepth()
{
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	depthStencilDescParticle.DepthEnable = true;
	depthStencilDescParticle.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDescParticle.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	
}
BlueMoon* BlueMoon::GetInstance()
{
	static BlueMoon instance;
	return &instance;
}
void BlueMoon::variableInitialize()
{
	
}



void BlueMoon::BeginFrame() {
	//imguiManager_->Begin();
	
	
	direct_->GetCommandList()->RSSetViewports(1, &viewport_);//viewportを設定
	direct_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);//scirssorを設定
	//RootSignatureを設定。PS0に設定しているけど別途設定が必要
	
	
	direct_->PreDraw();
	//direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//direct_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());//PS0を設定

}
void BlueMoon::EndFrame() {
	
	//imguiManager_->End();
	
	//imguiManager_->Draw();
	direct_->PostDraw();
}

void BlueMoon::Finalize()
{
	
	
}
//void BlueMoon::Update()
//{
//
//}
//void BlueMoon::Draw()
//{
//	//ImGui::ShowDemoWindow();
//
//}

BlueMoon::~BlueMoon()
{
	//imguiManager_->Finalize();
	

	pixelShaderBlob3D_->Release();
	vertexShaderBlob3D_->Release();
	pixelShaderBlob2D_->Release();
	vertexShaderBlob2D_->Release();
	direct_->Finalize();
	

}

void BlueMoon::ModelPreDraw()
{
	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature3D_.Get());
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineState3D_.Get());//PS0を設定
	
}

void BlueMoon::ModelPreDrawWireFrame()
{
	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature3D_.Get());
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineState3DWireFrame_.Get());//PS0を設定
}

void BlueMoon::SpritePreDraw()
{
	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature2D_.Get());
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineState2D_[kBlendModeNormal].Get());//PS0を設定
}
void BlueMoon::PariclePreDraw()
{
	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignatureParticle_.Get());
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineStateParticle_.Get());//PS0を設定
}
void BlueMoon::SetBlendMode(int BlendModeNum)
{
	if (BlendModeNum > 4) {
		BlendModeNum = 4;
	}
	if (BlendModeNum < 0) {
		BlendModeNum = 0;
	}
	direct_->GetCommandList()->SetGraphicsRootSignature(rootSignature2D_.Get());
	direct_->GetCommandList()->SetPipelineState(graphicsPipelineState2D_[BlendModeNum].Get());//PS0を設定
}
#pragma region 2D用のパイプライン
void BlueMoon::InitializePSO2D() {

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature2D_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc2D_;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob2D_->GetBufferPointer(),
		vertexShaderBlob2D_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob2D_->GetBufferPointer(),
		pixelShaderBlob2D_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_[PSO2DCount_];//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc2D_;//rasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDescParticle;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	graphicsPipelineState2D_[PSO2DCount_] = nullptr;
	HRESULT hr = direct_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState2D_[PSO2DCount_]));
	assert(SUCCEEDED(hr));
	PSO2DCount_++;

}
void BlueMoon::SettingRasterizerState2D() {

	//裏面（時計回り）を表示しない
	rasterizerDesc2D_.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc2D_.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	vertexShaderBlob2D_ = CompileShader(L"Resource/hlsl/Object2d.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlob2D_ != nullptr);


	pixelShaderBlob2D_ = CompileShader(L"Resource/hlsl/Object2d.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob2D_ != nullptr);
}
void BlueMoon::CreateRootSignature2D() {
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//RootParameter作成。複数設定できるので配列。今回は結果1つだけなので長さ１の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderを使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	D3D12_DESCRIPTOR_RANGE descriptoraRange[1] = {};
	descriptoraRange[0].BaseShaderRegister = 0;
	descriptoraRange[0].NumDescriptors = 1;
	descriptoraRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使用
	descriptoraRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Descriptortableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixcelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptoraRange;//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRange);


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//０～１の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//シリアライズしてバイナリにする
	signatureBlob2D_ = nullptr;
	errorBlob2D_ = nullptr;
	HRESULT hr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob2D_, &errorBlob2D_);
	if (FAILED(direct_->GetHr())) {
		Log(reinterpret_cast<char*>(errorBlob2D_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	rootSignature2D_ = nullptr;
	hr = direct_->GetDevice()->CreateRootSignature(0, signatureBlob2D_->GetBufferPointer(),
		signatureBlob2D_->GetBufferSize(), IID_PPV_ARGS(&rootSignature2D_));
	assert(SUCCEEDED(hr));
	}
void BlueMoon::CreateInputlayOut2D() {
	//inputElementDescsをメンバ変数にすると治った
	inputElementDescs2D_[0].SemanticName = "POSITION";
	inputElementDescs2D_[0].SemanticIndex = 0;
	inputElementDescs2D_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs2D_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs2D_[1].SemanticName = "TEXCOORD";
	inputElementDescs2D_[1].SemanticIndex = 0;
	inputElementDescs2D_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs2D_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	

	inputLayoutDesc2D_.pInputElementDescs = inputElementDescs2D_;
	inputLayoutDesc2D_.NumElements = _countof(inputElementDescs2D_);
}


	
#pragma endregion
#pragma region Particle用のパイプライン
void BlueMoon::CreateRootSignatureParticle()
{
	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//RootParameter作成。複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixelShaderを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	//worldtransform
	D3D12_DESCRIPTOR_RANGE descriptoraRangeParticle[1] = {};
	descriptoraRangeParticle[0].BaseShaderRegister = 0;
	descriptoraRangeParticle[0].NumDescriptors = 1;
	descriptoraRangeParticle[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使用
	descriptoraRangeParticle[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderを使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptoraRangeParticle;//レジスタ番号0とバインド
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRangeParticle);
	//viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号を1にバインド

	D3D12_DESCRIPTOR_RANGE descriptoraRange[1] = {};
	descriptoraRange[0].BaseShaderRegister = 1;
	descriptoraRange[0].NumDescriptors = 1;
	descriptoraRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使用
	descriptoraRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//Descriptortableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixcelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptoraRange;//tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptoraRange);

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//pixcelShaderを使う
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//０～１の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのmipmapを使う
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;




	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//シリアライズしてバイナリにする

	HRESULT hr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlobParticle_, &errorBlobParticle_);
	if (FAILED(direct_->GetHr())) {
		Log(reinterpret_cast<char*>(errorBlobParticle_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成

	hr = direct_->GetDevice()->CreateRootSignature(0, signatureBlobParticle_->GetBufferPointer(),
		signatureBlobParticle_->GetBufferSize(), IID_PPV_ARGS(&rootSignatureParticle_));
	assert(SUCCEEDED(hr));
}

void BlueMoon::SettingRasterizerStateParticle()
{
	//裏面（時計回り）を表示しない
	rasterizerDescParticle_.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDescParticle_.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	vertexShaderBlobParticle_ = CompileShader(L"Resource/hlsl/Particle.VS.hlsl",
		L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(vertexShaderBlobParticle_ != nullptr);


	pixelShaderBlobParticle_ = CompileShader(L"Resource/hlsl/Particle.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlobParticle_ != nullptr);
}

void BlueMoon::InitializePSOParticle()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignatureParticle_.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;//Inputlayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlobParticle_->GetBufferPointer(),
		vertexShaderBlobParticle_->GetBufferSize() };//vertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlobParticle_->GetBufferPointer(),
		pixelShaderBlobParticle_->GetBufferSize() };//pixcelShader
	graphicsPipelineStateDesc.BlendState = blendDesc_[kBlendModeAdd];//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDescParticle_;//rasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むのかの設定（気にしなく良い）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDescParticle;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//実際に生成
	graphicsPipelineStateParticle_ = nullptr;
	HRESULT hr = direct_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineStateParticle_));
	assert(SUCCEEDED(hr));
}

void BlueMoon::CreateInputlayOutParticle()
{
	//inputElementDescsをメンバ変数にすると治った
	inputElementDescsParticle_[0].SemanticName = "POSITION";
	inputElementDescsParticle_[0].SemanticIndex = 0;
	inputElementDescsParticle_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescsParticle_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescsParticle_[1].SemanticName = "TEXCOORD";
	inputElementDescsParticle_[1].SemanticIndex = 0;
	inputElementDescsParticle_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescsParticle_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescsParticle_[2].SemanticName = "NORMAL";
	inputElementDescsParticle_[2].SemanticIndex = 0;
	inputElementDescsParticle_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescsParticle_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescsParticle_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescsParticle_);
}
#pragma endregion