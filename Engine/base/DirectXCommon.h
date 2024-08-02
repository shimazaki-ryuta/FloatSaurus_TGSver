#pragma once
#pragma region standardInclude
#include <chrono>
#include <cstdlib>
#include <dxgi1_6.h>
#include<chrono>

#include <wrl.h>
#include <dxgidebug.h>
#include<memory>
#pragma endregion
#include "WinApp.h"
#include "combert.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

class DirectXCommon
{
public:
	~DirectXCommon();
	static DirectXCommon* GetInstance();
	void Initialize(
		WinApp* win, int32_t backBufferWidth = WinApp::kClientWidth,
		int32_t backBufferHeight = WinApp::kClientHeight);

	void PreDraw();
	void PostDraw();
	void ClearRenderTarget();
	void Finalize();
	HRESULT GetHr() { return  hr_; }
	void SetHr(HRESULT a) { this->hr_ = a; }

	Microsoft::WRL::ComPtr<ID3D12Device>GetDevice() { return device_; }

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>GetCommandList() { return commandList_; }
	static Microsoft::WRL::ComPtr <ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
	WinApp* GetWin() { return winApp_; }
	UINT GetbackBufferCount() { return swapChainDesc.BufferCount; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>CreateDescriptionHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescripters, bool shaderVisible);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvHeap() { return srvDescriptorHeap_; }

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvHeap() { return dsvDescriptorHeap_; }
	D3D12_RENDER_TARGET_VIEW_DESC getRtvDesc() { return rtvDesc; }


private:
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;

	WinApp* winApp_;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;

	Microsoft::WRL::ComPtr<IDXGIFactory7>dxgiFactory_;

	Microsoft::WRL::ComPtr<ID3D12Device>device_;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>commandQueue_;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>commandAllocator_;


	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>commandList_;


	Microsoft::WRL::ComPtr<IDXGISwapChain4>swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	uint32_t knumDescriptor = 100000;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE dsvhandle;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers_[2];
	UINT64 fenceVal_;
	int32_t backBufferWidth_;
	int32_t backBufferHeight_;
	D3D12_RESOURCE_BARRIER barrier_{};
	//初期値０でFenceを作る

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	HANDLE fenceEvent_;
	HRESULT hr_;

	Microsoft::WRL::ComPtr<ID3D12Resource>  depthStencilResource;

	std::chrono::steady_clock::time_point reference_;
private:


	void InitializeDXGIDevice();
	void CreateSwapChain();
	void InitializeCommand();
	void CreateFinalRenderTargets();
	void CreateFence();
	void CreateSrvheap();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStenciltextureResource(ID3D12Device* device, int32_t width, int32_t height);
	void CreateDepthStensil();
	void CreateDebug();
	void InitializeFixFPS();

	void updateFixFPS();
};

