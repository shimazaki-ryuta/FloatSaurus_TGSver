#include "TitleScene.h"
#include "ImGuiManger.h"

TitleScene::~TitleScene()
{
}
void TitleScene::Initialize()
{	
	input = Input::GetInstance();
	count = 0;

}

void TitleScene::Update()
{
	
	ImGui::Begin("SceneManager");
	ImGui::InputInt("SceneNum", &sceneNum);
	ImGui::Text("count %d",count);
	ImGui::End();
	if (input->PushKey(DIK_SPACE)) {
		sceneNum = GAME_SCENE;
		count++;
	}
	
	if (sceneNum < 0) {
		sceneNum = 0;
		
	}
	/*if (count >= 60) {
		sceneNum=GAME_SCENE;
	}*/
}

void TitleScene::Draw()
{
}

void TitleScene::Finalize()
{
}
