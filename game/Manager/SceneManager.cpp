#include "SceneManager.h"
#include"ImGuiManger.h"
SceneManager::SceneManager()
{



}
SceneManager::~SceneManager()
{
	imguiManager_->Finalize();
	audio_->xAudio2.Reset();
	audio_->SoundUnload(&audio_->soundDatas[0]);
}
void SceneManager::Run() {
	Initialize();

	while (true) {
		// メッセージ処理
		if (winApp_->Procesmessage()) {
			break;
		}
		imguiManager_->Begin();
		input->Update();
		glovalVariables_->Update();
		directionallight_->Update();
		blueMoon_->BeginFrame();

		preSceneNum_ = sceneNum_;
		sceneNum_ = sceneArr_[sceneNum_]->GetSceneNum();

		if (sceneNum_ != preSceneNum_) {
			sceneArr_[preSceneNum_]->Finalize();
			sceneArr_[sceneNum_]->Initialize();

		}
		sceneArr_[sceneNum_]->Update();
		

		sceneArr_[sceneNum_]->Draw();


#ifdef _DEBUG
		imguiManager_->End();

		imguiManager_->Draw();

#endif // _DEBUG


		
		blueMoon_->EndFrame();
		if (sceneArr_[sceneNum_]->GetIsEnd()) {
			break;
		}
	}
	CoUninitialize();
	blueMoon_->Finalize();
	sceneArr_[sceneNum_]->Finalize();

}

void SceneManager::Initialize()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	blueMoon_ = BlueMoon::GetInstance();
	blueMoon_->Initialize(1280, 720);
	winApp_ = WinApp::GetInstance();
	input = Input::GetInstance();
	input->Initialize(winApp_);
	textureManager_ = Texturemanager::GetInstance();
	textureManager_->Initialize();
	imguiManager_ = ImGuiManger::GetInstance();
	imguiManager_->Initialize(winApp_, blueMoon_->GetDirectXCommon());
	glovalVariables_ = GlovalVariables::GetInstance();
	glovalVariables_->LoadFiles();
	directionallight_ = DirectionalLight::GetInstance();
	directionallight_->Initialize();
	audio_ = Audio::GetInstance();
	audio_->Initialize();
	sceneArr_[TITLE_SCENE] = std::make_unique <TitleScene>();
	sceneArr_[GAME_SCENE] = std::make_unique <GameScene>();
	sceneArr_[RESULT_SCENE] = std::make_unique <ResultScene>();

	sceneNum_ = GAME_SCENE;
	
	
	audio_->handle_[EnemyPop] = audio_->SoundLoadWave("resource/SE/enemyPop.wav");

	audio_->handle_[GameStart] = audio_->SoundLoadWave("resource/SE/gameStart.wav");
	audio_->handle_[Death] = audio_->SoundLoadWave("resource/SE/death.wav");
	audio_->handle_[Reverce] = audio_->SoundLoadWave("resource/SE/reverce.wav");
	
	audio_->handle_[Jump] = audio_->SoundLoadWave("resource/SE/jump.wav");
	audio_->handle_[DeleteEnemy] = audio_->SoundLoadWave("resource/SE/deleteEnemy.wav");
	audio_->handle_[ResultBGM] = audio_->SoundLoadWave("resource/SE/resultBGM.wav");
	audio_->handle_[inGameBGM] = audio_->SoundLoadWave("resource/SE/inGameBGM.wav");
	audio_->handle_[Block] = audio_->SoundLoadWave("resource/SE/block.wav");
	audio_->handle_[Clear] = audio_->SoundLoadWave("resource/SE/clear.wav");
	audio_->handle_[Bound] = audio_->SoundLoadWave("resource/SE/bound.wav");
	audio_->handle_[PlusWave] = audio_->SoundLoadWave("resource/SE/plusWeve.wav");
	sceneArr_[sceneNum_]->Initialize();

}


