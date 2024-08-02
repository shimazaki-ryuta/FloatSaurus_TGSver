#include "SrvDescriptorHeap.h"

void SrvDescriptorHeap::Initialize()
{
	dirctXCommon_ = DirectXCommon::GetInstance();
	descriptorSizeSRV = dirctXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Index_ = 2;
}

SrvDescriptorHeap* SrvDescriptorHeap::GetInstance()
{
	static SrvDescriptorHeap instance;
	return &instance;
}

void SrvDescriptorHeap::Update()
{

}

const D3D12_GPU_DESCRIPTOR_HANDLE SrvDescriptorHeap::GetGPUHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = SrvHandleGPU_[index];
	return GPUHandle;
}

const D3D12_CPU_DESCRIPTOR_HANDLE SrvDescriptorHeap::GetCPUHandle(uint32_t textureHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = SrvHandleCPU_[textureHandle];
	return CPUHandle;
}

void SrvDescriptorHeap::SetGPUHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index)
{
	SrvHandleGPU_[index] = GetSrvHandleGPU(descriptorheap, descriptorSize, index + 1);
}

void SrvDescriptorHeap::SetCPUHandle(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index)
{
	SrvHandleCPU_[index] = GetSrvHandleCPU(descriptorheap, descriptorSize, index + 1);
}

void SrvDescriptorHeap::AddPtr(uint32_t index, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	SrvHandleCPU_[index].ptr += dirctXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(type);
	SrvHandleGPU_[index].ptr += dirctXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(type);
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvDescriptorHeap::GetSrvHandleCPU(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorheap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvDescriptorHeap::GetSrvHandleGPU(ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorheap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}
