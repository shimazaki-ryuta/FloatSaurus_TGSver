#pragma once
#include <wrl.h>
#include <array>
#include "DirectXCommon.h"
#include <vector>
class SrvDescriptorHeap
{
public:
	static const int max = 1052;
	void Initialize();
	static SrvDescriptorHeap* GetInstance();
	void Update();
	void AddIndex() { Index_++; }
	uint32_t GetIndex() { return Index_; }
	uint32_t GetSizeSRV() { return descriptorSizeSRV; }
	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);
	const D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t textureHandle);
	void SetGPUHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);
	void SetCPUHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);
	void AddPtr(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type);
	D3D12_CPU_DESCRIPTOR_HANDLE GetSrvHandleCPU(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);
private:
	DirectXCommon* dirctXCommon_;
	D3D12_CPU_DESCRIPTOR_HANDLE SrvHandleCPU_[max];
	D3D12_GPU_DESCRIPTOR_HANDLE SrvHandleGPU_[max];
	uint32_t Index_;
	uint32_t descriptorSizeSRV;
};

