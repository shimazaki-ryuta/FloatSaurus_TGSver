#include "ImGuiManger.h"

ImGuiManger* ImGuiManger::GetInstance()
{
	static ImGuiManger instance;
	return &instance;
}

void ImGuiManger::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice().Get(),
		dxCommon_->GetbackBufferCount(),
		dxCommon_->getRtvDesc().Format,
		dxCommon_->GetSrvHeap().Get(),
		dxCommon_->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManger::Finalize()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void ImGuiManger::Begin()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetSrvHeap().Get()};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void ImGuiManger::End()
{
	ImGui::Render();
	
}

void ImGuiManger::Draw()
{
	
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList().Get());
	
}
