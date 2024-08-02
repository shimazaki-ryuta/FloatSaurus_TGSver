#include "DirectXCommon.h"
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>



DirectXCommon::~DirectXCommon()
{
}

DirectXCommon* DirectXCommon::GetInstance()
{
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(WinApp* win, int32_t backBufferWidth, int32_t backBufferHeight)
{
	//resourceLeak = new LeakCheck();
	InitializeFixFPS();
	winApp_ = win;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
	winApp_->CreateGameWindow(L"2206_フロ~トザウルス", 1280, 720);

	// DXGIデバイス初期化
	InitializeDXGIDevice();
	CreateDebug();
	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();
	CreateSrvheap();
	// レンダーターゲット生成
	CreateFinalRenderTargets();

	CreateDepthStensil();
	// フェンス生成
	CreateFence();

}
//デバイスの作成
void DirectXCommon::InitializeDXGIDevice() {
	dxgiFactory_ = nullptr;
	//HRESULTはWindows系のエラーコードであり
	// 関数が成功したかどうかをSUCCEDEDマクロで判定できる
	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr_));
	//使用するアダプタ用の変数。さいしょにnullptrを入れておく
	useAdapter_ = nullptr;
	//いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr_));//取得できないのは一大事
		//ソフトウェアアダプタでなければ採用!

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力、wstringのほうなので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter_ = nullptr;//ソフトウェアアダプタの場合見なかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);
	device_ = nullptr;
	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featurelevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featurelevels); ++i) {
		//採用したアダプターでデバイスを生成
		hr_ = D3D12CreateDevice(useAdapter_.Get(), featurelevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr_)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("Featurelevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");//	初期化完了のログを出す

}
////コマンド関連の初期化
void DirectXCommon::InitializeCommand() {
	//コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	//コマンドアロケータを生成する
	commandAllocator_ = nullptr;
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
	//コマンドリストを生成する
	commandList_ = nullptr;
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}
//スワップチェーンを生成
void DirectXCommon::CreateSwapChain() {
	//swapChain_ = nullptr;

	swapChainDesc.Width = WinApp::kClientWidth;//画面の幅
	swapChainDesc.Height = WinApp::kClientHeight;//画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタにうつしたら中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	HRESULT	hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	//ディスクリプタヒープの生成
	rtvDescriptorHeap_ = CreateDescriptionHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//SwapChainからResourceを引っ張ってくる
	backBuffers_[0] = { nullptr };
	backBuffers_[1] = { nullptr };
	hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffers_[0]));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
	hr = swapChain_->GetBuffer(1, IID_PPV_ARGS(&backBuffers_[1]));
	assert(SUCCEEDED(hr));
}

// レンダーターゲット生成
void DirectXCommon::CreateFinalRenderTargets() {

	//RTVの設定

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	//まず一つ目を作る。一つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(backBuffers_[0].Get(), &rtvDesc, rtvHandles_[0]);
	//2つ目のディスクリプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//2つ目を作る
	device_->CreateRenderTargetView(backBuffers_[1].Get(), &rtvDesc, rtvHandles_[1]);

}
// フェンス生成
void DirectXCommon::CreateFence() {
	//初期値０でFenceを作る
	fence_ = nullptr;
	fenceVal_ = 0;

	HRESULT	hr = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
	//fenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);


}
void DirectXCommon::CreateSrvheap()
{
	srvDescriptorHeap_ = CreateDescriptionHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, knumDescriptor, true);
}
void DirectXCommon::PreDraw()
{
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier_.Transition.pResource = backBuffers_[backBufferIndex].Get();
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList_->ResourceBarrier(1, &barrier_);

	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);
	dsvhandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvhandle);
	commandList_->ClearDepthStencilView(dsvhandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::PostDraw() {
	hr_;
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//TransitonBarrierを張る
	commandList_->ResourceBarrier(1, &barrier_);


	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	//GPUにコマンドリストを準備する
	//Microsoft::WRL::ComPtr<ID3D12CommandList>commandLists[] = { commandList_.Get() };
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うように通知する
	swapChain_->Present(1, 0);

	//Fenceの値を更新
	fenceVal_++;
	//GPUがここまでたどり着いたときに、Fenceの値を指定し値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);

	//Fenceの値が指定したSignal値にたどりつうてるじゃ確認する
//GetCompletevaluseの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		//イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
	updateFixFPS();
	//次のフレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));

}

void DirectXCommon::ClearRenderTarget()
{
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);
	//指定した色で画面全体をクリアする
	float clearcolor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearcolor, 0, nullptr);

}

void DirectXCommon::Finalize() {
	CloseHandle(fenceEvent_);

#ifdef DEBUG
	winApp_->GetdebugController()->Release();
#endif // DEBUG

	CloseWindow(winApp_->GetHwnd());



}


Microsoft::WRL::ComPtr <ID3D12Resource> DirectXCommon::CreateBufferResource(ID3D12Device* device, size_t sizeInBytes)
{
	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uplodeHeapProperties{};
	uplodeHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う
	//頂点リソースの設定
	D3D12_RESOURCE_DESC ResourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDesc.Width = sizeInBytes;//リソースサイズ
	//バッファの場合はこれらは１にする決まり
	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	HRESULT hr;
	//Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	ID3D12Resource* Resource = nullptr;
	//実際に頂点リソースを作る
	hr = device->CreateCommittedResource(&uplodeHeapProperties, D3D12_HEAP_FLAG_NONE,
		&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));

	return Resource;
}
//デスクリプタヒープの作成
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptionHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescripters, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	//ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptionHeapDesc{};
	descriptionHeapDesc.Type = heapType;//レンダーターゲットビュー用
	descriptionHeapDesc.NumDescriptors = numDescripters;//ダブルバッファ用に二つ。多くても別にかまわない
	descriptionHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptionHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStenciltextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	Microsoft::WRL::ComPtr<ID3D12Resource>resource = nullptr;
	//ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}
void DirectXCommon::CreateDepthStensil() {
	depthStencilResource = CreateDepthStenciltextureResource(device_.Get(), WinApp::kClientWidth, WinApp::kClientHeight);
	dsvDescriptorHeap_ = CreateDescriptionHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc{};
	dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device_->CreateDepthStencilView(depthStencilResource.Get(), &dsvdesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::CreateDebug()
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue>infoQueue = nullptr;
	//ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//ヤバいエラーで止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するめっせーじのID 
		D3D12_MESSAGE_ID denyIds[] = {
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		//infoQueue->Release();
	}
#endif
}

void DirectXCommon::InitializeFixFPS()
{
	reference_ = std::chrono::steady_clock::now();
	
}

void DirectXCommon::updateFixFPS()
{
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);
	if (elapsed < kMinTime) {
		while (std::chrono::steady_clock::now() - reference_ < kMinTime)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	reference_ = std::chrono::steady_clock::now();
}
