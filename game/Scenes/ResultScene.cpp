#include "ResultScene.h"
#include "MapManager.h"
#include "WaveManager.h"
#include "GameController.h"
#include "Audio.h"
ResultScene::~ResultScene() {

}

void ResultScene::Finalize() {

}

void ResultScene::Initialize() {
	//SoundVolume[ResultBGM] = 0.0f; 
	sceneNum = 2;
	blueMoon_ = BlueMoon::GetInstance();

	directXCommon_ = DirectXCommon::GetInstance();

	textureManager_ = Texturemanager::GetInstance();


	transitionSprite_.reset(new Sprite);
	transitionSprite_->Initialize({ 0.0f,0.0f,0.0f,0.0f }, { 1280.0f,720.0f,0.0f,0.0f });
	blackTextureHandle_ = textureManager_->Load("resource/white.png");

	backGroundSprite_.reset(new Sprite);
	backGroundSprite_->Initialize({ 0,0,0,0 }, { 1280,720,0,0 });
	backTextureHandle_ = textureManager_->Load("resource/back.png");

	viewProjection_.Initialize();

	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "ResultSetting";
	globalVariables->AddItem(groupName, "cameraRotate", viewProjection_.rotation_);
	globalVariables->AddItem(groupName, "cameraTranslate", viewProjection_.translation_);

	numberTextureHandle_[0] = Texturemanager::GetInstance()->Load("Resource/nums/0.png");
	numberTextureHandle_[1] = Texturemanager::GetInstance()->Load("Resource/nums/1.png");
	numberTextureHandle_[2] = Texturemanager::GetInstance()->Load("Resource/nums/2.png");
	numberTextureHandle_[3] = Texturemanager::GetInstance()->Load("Resource/nums/3.png");
	numberTextureHandle_[4] = Texturemanager::GetInstance()->Load("Resource/nums/4.png");
	numberTextureHandle_[5] = Texturemanager::GetInstance()->Load("Resource/nums/5.png");
	numberTextureHandle_[6] = Texturemanager::GetInstance()->Load("Resource/nums/6.png");
	numberTextureHandle_[7] = Texturemanager::GetInstance()->Load("Resource/nums/7.png");
	numberTextureHandle_[8] = Texturemanager::GetInstance()->Load("Resource/nums/8.png");
	numberTextureHandle_[9] = Texturemanager::GetInstance()->Load("Resource/nums/9.png");
	
	reachTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/wavetex/goal.png");
	waveTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/wavetex/wave.png");
	bestTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/wavetex/best.png");

	worldTransformReach_.Initialize();
	globalVariables->AddItem(groupName, "reachRotate", worldTransformReach_.rotation_);
	globalVariables->AddItem(groupName, "reachTranslate", worldTransformReach_.translation_);
	worldTransformReachChar_.Initialize();
	worldTransformReachChar_.parent_ = &worldTransformReach_;
	globalVariables->AddItem(groupName, "reachCharScale", worldTransformReachChar_.scale_);
	globalVariables->AddItem(groupName, "reachCharTranslate", worldTransformReachChar_.translation_);
	worldTransformReachWave_.Initialize();
	worldTransformReachWave_.parent_ = &worldTransformReach_;
	globalVariables->AddItem(groupName, "reachWaveScale", worldTransformReachWave_.scale_);
	globalVariables->AddItem(groupName, "reachWaveTranslate", worldTransformReachWave_.translation_);
	worldTransformReach10_.Initialize();
	worldTransformReach10_.parent_ = &worldTransformReach_;
	globalVariables->AddItem(groupName, "reach10Scale", worldTransformReach10_.scale_);
	globalVariables->AddItem(groupName, "reach10Translate", worldTransformReach10_.translation_);
	worldTransformReach1_.Initialize();
	worldTransformReach1_.parent_ = &worldTransformReach_;
	globalVariables->AddItem(groupName, "reach1Scale", worldTransformReach1_.scale_);
	globalVariables->AddItem(groupName, "reach1Translate", worldTransformReach1_.translation_);

	worldTransformBest_.Initialize();
	globalVariables->AddItem(groupName, "BestRotate", worldTransformBest_.rotation_);
	globalVariables->AddItem(groupName, "BestTranslate", worldTransformBest_.translation_);
	worldTransformBestChar_.Initialize();
	worldTransformBestChar_.parent_ = &worldTransformBest_;
	globalVariables->AddItem(groupName, "BestCharScale", worldTransformBestChar_.scale_);
	globalVariables->AddItem(groupName, "BestCharTranslate", worldTransformBestChar_.translation_);
	worldTransformBestWave_.Initialize();
	worldTransformBestWave_.parent_ = &worldTransformBest_;
	globalVariables->AddItem(groupName, "BestWaveScale", worldTransformBestWave_.scale_);
	globalVariables->AddItem(groupName, "BestWaveTranslate", worldTransformBestWave_.translation_);
	worldTransformBest10_.Initialize();
	worldTransformBest10_.parent_ = &worldTransformBest_;
	globalVariables->AddItem(groupName, "Best10Scale", worldTransformBest10_.scale_);
	globalVariables->AddItem(groupName, "Best10Translate", worldTransformBest10_.translation_);
	worldTransformBest1_.Initialize();
	worldTransformBest1_.parent_ = &worldTransformBest_;
	globalVariables->AddItem(groupName, "Best1Scale", worldTransformBest1_.scale_);
	globalVariables->AddItem(groupName, "Best1Translate", worldTransformBest1_.translation_);

	worldTransformReach_.UpdateMatrix();
	//worldTransformReachChar_.rotation_.y = 3.14f;
	worldTransformReachChar_.UpdateMatrix();
	worldTransformReachWave_.UpdateMatrix();
	worldTransformReach10_.UpdateMatrix();
	worldTransformReach1_.UpdateMatrix();

	worldTransformBest_.UpdateMatrix();
	worldTransformBestChar_.UpdateMatrix();
	worldTransformBestWave_.UpdateMatrix();
	worldTransformBest10_.UpdateMatrix();
	worldTransformBest1_.UpdateMatrix();

	plane_.reset(new Plane);
	plane_->Initialize();

	returnTitleSprite_.reset(new Sprite);
	returnTitleSprite_->Initialize({-800.0f,-200.0f,0,0}, { 800.0f,200.0f,0,0 });

	titleTransform_ = {{ 1.0f,1.0f,1.0f, },
		{0,0,0},{0,0,0}};
	globalVariables->AddItem(groupName, "TitleScale", titleTransform_.scale);
	globalVariables->AddItem(groupName, "TitlePosition", titleTransform_.translate);

	returnTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/UI/backUI.png");
	
	newRSprite_.reset(new Sprite);
	newRSprite_->Initialize({ -500,-160,0,0 }, { 500,160,0,0 });
	clearSprite_.reset(new Sprite);
	clearSprite_->Initialize({ -500,-160,0,0 }, { 500,160,0,0 });
	globalVariables->AddItem(groupName, "ClearScale", clearTransform_.scale);
	globalVariables->AddItem(groupName, "ClaerRotate", clearTransform_.rotate);
	globalVariables->AddItem(groupName, "ClearTranslate", clearTransform_.translate);

	globalVariables->AddItem(groupName, "newRScale", newRTransform_.scale);
	globalVariables->AddItem(groupName, "newRRotate", newRTransform_.rotate);
	globalVariables->AddItem(groupName, "newRTranslate", newRTransform_.translate);


	newRTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/newR.png");
	clearTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/clear.png");


	//単体テスト用
#ifdef _DEBUG
	//MapManager::GetInstance()->Initialize();
	//WaveManager::GetInstance()->SetWave(29);
#endif // _DEBUG
	drawerWaveNum_ = 0;
	bestWaveNum_ = WaveManager::GetInstance()->GetBestWave();
	MapManager::GetInstance()->WaveRead(0);

	isRunAnimation_ = false;
	phase_ = FROMGAME;
	theta_ = 0;
	alpha_ = 0;

	player_.reset(new ResultPlayer);
	player_->Initialize();
	ApplyGlobalVariables();
		Audio::GetInstance()->SoundPlayloop(Audio::GetInstance()->handle_[ResultBGM], Audio::GetInstance()->SoundVolume[ResultBGM]);

}

void ResultScene::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "ResultSetting";
	viewProjection_.rotation_ = globalVariables->GetVector3Value(groupName, "cameraRotate");
	viewProjection_.translation_ = globalVariables->GetVector3Value(groupName, "cameraTranslate");

	worldTransformReach_.rotation_ = globalVariables->GetVector3Value(groupName, "reachRotate");
	worldTransformReach_.translation_ = globalVariables->GetVector3Value(groupName, "reachTranslate");
	worldTransformReachChar_.scale_ = globalVariables->GetVector3Value(groupName, "reachCharScale");
	worldTransformReachChar_.translation_ = globalVariables->GetVector3Value(groupName, "reachCharTranslate");
	worldTransformReachWave_.scale_ = globalVariables->GetVector3Value(groupName, "reachWaveScale");
	worldTransformReachWave_.translation_ = globalVariables->GetVector3Value(groupName, "reachWaveTranslate");
	worldTransformReach10_.scale_ = globalVariables->GetVector3Value(groupName, "reach10Scale");
	worldTransformReach10_.translation_ = globalVariables->GetVector3Value(groupName, "reach10Translate");
	worldTransformReach1_.scale_ = globalVariables->GetVector3Value(groupName, "reach1Scale");
	worldTransformReach1_.translation_ = globalVariables->GetVector3Value(groupName, "reach1Translate");

	worldTransformBest_.rotation_ = globalVariables->GetVector3Value(groupName, "BestRotate");
	worldTransformBest_.translation_ = globalVariables->GetVector3Value(groupName, "BestTranslate");
	worldTransformBestChar_.scale_ = globalVariables->GetVector3Value(groupName, "BestCharScale");
	worldTransformBestChar_.translation_ = globalVariables->GetVector3Value(groupName, "BestCharTranslate");
	worldTransformBestWave_.scale_ = globalVariables->GetVector3Value(groupName, "BestWaveScale");
	worldTransformBestWave_.translation_ = globalVariables->GetVector3Value(groupName, "BestWaveTranslate");
	worldTransformBest10_.scale_ = globalVariables->GetVector3Value(groupName, "Best10Scale");
	worldTransformBest10_.translation_ = globalVariables->GetVector3Value(groupName, "Best10Translate");
	worldTransformBest1_.scale_ = globalVariables->GetVector3Value(groupName, "Best1Scale");
	worldTransformBest1_.translation_ = globalVariables->GetVector3Value(groupName, "Best1Translate");

	titleTransform_.scale = globalVariables->GetVector3Value(groupName, "TitleScale");
	titleTransform_.translate = globalVariables->GetVector3Value(groupName, "TitlePosition");

	//if (phase_ == FROMGAME) {
		clearTransform_.scale = globalVariables->GetVector3Value(groupName, "ClearScale");
		clearTransform_.rotate = globalVariables->GetVector3Value(groupName, "ClaerRotate");
		clearTransform_.translate = globalVariables->GetVector3Value(groupName, "ClearTranslate");
	//}
		newRTransform_.scale = globalVariables->GetVector3Value(groupName, "newRScale");
		newRTransform_.rotate = globalVariables->GetVector3Value(groupName, "newRRotate");
		newRTransform_.translate = globalVariables->GetVector3Value(groupName, "newRTranslate");
		
}

void ResultScene::Update() {
	

	ApplyGlobalVariables();
	GameController::GetInstance()->Update();
	switch (phase_)
	{
	case ResultScene::FROMGAME:
		FromGame();
		break;
	case ResultScene::RESULT:
		Result();
		break;
	case ResultScene::TOTITLE:
		ToTitle();
		break;
	default:
		break;
	}
	if (isRunAnimation_) {
		TransitionAnimation();
	}

	player_->Update();

	worldTransformReach_.UpdateMatrix();
	worldTransformReachChar_.UpdateMatrix();
	worldTransformReachWave_.UpdateMatrix();
	worldTransformReach10_.UpdateMatrix();
	worldTransformReach1_.UpdateMatrix();

	worldTransformBest_.UpdateMatrix();
	worldTransformBestChar_.UpdateMatrix();
	worldTransformBestWave_.UpdateMatrix();
	worldTransformBest10_.UpdateMatrix();
	worldTransformBest1_.UpdateMatrix();

	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();
}

void ResultScene::FromGame() {
	if (!isRunAnimation_) {
		isRunAnimation_ = true;
		frameCount_ = 0;
	}
	if (isRunAnimation_) {
		resetT_ = frameCount_ / float(transitionAnimationLength_);
		//resetT_ = std::powf(resetT_ * 2.0f - 1.0f, 2) * -1.0f + 1.0f;
		resetT_ = 1.0f - resetT_;
		if (WaveManager::GetInstance()->IsEnd()) {
			resetT_ = 1.0f;
		}
		
		
		if (frameCount_ >= transitionAnimationLength_) {
			isRunAnimation_ = false;
			isFinCount_ = false;
			isUpdate_ = false;
			plus_ = clearTransform_;
			phase_ = RESULT;
			frameCount_ = 0;
		}
	}
	frameCount_++;
}

float easeback(float t) {
	float c1 = 1.70158f;
	float c3 = c1 + 1.0f;

	return float(1 + c3 * std::pow(t - 1.0f, 3) + c1 * std::pow(t - 1.0f, 2));
}

void ResultScene::Result() {
	if (drawerWaveNum_ < WaveManager::GetInstance()->GetWave() + 1) {
		theta_ = 0;
		alpha_ = 0;
		frameCount2_ = 0;
		isFinClearAnime_ = false;
		if (frameCount_ > numChangeLength_) {
			drawerWaveNum_++;
			if (bestWaveNum_ < drawerWaveNum_) {
				isUpdate_ = true;
				bestWaveNum_ = drawerWaveNum_;
				WaveManager::GetInstance()->SetBestWave(uint32_t(bestWaveNum_));
			}
			frameCount_ = 0;
		}
		frameCount_++;
	}
	else if (isFinClearAnime_ == false && (WaveManager::GetInstance()->IsEnd())) {
		if (!WaveManager::GetInstance()->ISClear() && WaveManager::GetInstance()->IsEnd()) {
			isUpdate_ = true;
		}
		WaveManager::GetInstance()->GameClear();
		if (frameCount_ <= clearAnimationLength_) {
			float t = float(frameCount_) / float(clearAnimationLength_);
			float easedT = easeback(t);
			plus_.scale = clearTransform_.scale * easedT;
			if (frameCount_ <= clearAnimationLength_/2) {
				plus_.rotate = Lerp(t*2.0f, { 0,0,0 }, clearTransform_.rotate);
			}
			if (frameCount_ == clearAnimationLength_ / 2) {
				///ここにクリア音を入れる
				Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Clear], Audio::GetInstance()->SoundVolume[Clear]);
				///
			}
			//plus_.rotate = clearTransform_.rotate;
		}
		else {
			isFinClearAnime_ = true;
		}
		isFinCount_ = true;
		frameCount_++;
	}
	else {
		isFinCount_ = true;
		isFinClearAnime_ = true;
		if (GameController::GetInstance()->Enter()) {
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[GameStart], Audio::GetInstance()->SoundVolume[GameStart]);

			phase_ = TOTITLE;
		}
		alpha_ = (std::sin(theta_) + 1.0f) * 0.5f;
		theta_ += 0.05f;
	}
}

void ResultScene::ToTitle() {
	if (!isRunAnimation_) {
		isRunAnimation_ = true;
		frameCount_ = 0;
	}
	if (isRunAnimation_) {
		resetT_ = frameCount_ / float(transitionAnimationLength_);
		

		if (frameCount_ >= transitionAnimationLength_) {
			//isRunAnimation_ = false;
			Audio::GetInstance()->StopAudio(Audio::GetInstance()->handle_[ResultBGM]);

			sceneNum = 1;
		}
	}
	frameCount_++;
}

void ResultScene::Draw()
{
	//2D描画準備
	blueMoon_->SpritePreDraw();
	DrawBackGround();
	//3D描画準備
	blueMoon_->ModelPreDraw();
	Draw3D();
	//2D描画準備
	blueMoon_->SpritePreDraw();
	Draw2D();
}

void ResultScene::Draw3D() {
	blueMoon_->ModelPreDraw();

	MapManager::GetInstance()->Draw(viewProjection_);
	player_->Draw(viewProjection_);

	plane_->Draw(worldTransformReachChar_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, reachTextureHandle_);
	plane_->Draw(worldTransformReachWave_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, waveTextureHandle_);
	plane_->Draw(worldTransformReach10_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[drawerWaveNum_/10]);
	plane_->Draw(worldTransformReach1_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[drawerWaveNum_%10]);

	plane_->Draw(worldTransformBestChar_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, bestTextureHandle_);
	plane_->Draw(worldTransformBestWave_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, waveTextureHandle_);
	plane_->Draw(worldTransformBest10_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[bestWaveNum_ / 10]);
	plane_->Draw(worldTransformBest1_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[bestWaveNum_ % 10]);

}

void ResultScene::Draw2D() {
	blueMoon_->SetBlendMode(blendCount_);
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	if (phase_ != FROMGAME) {
		returnTitleSprite_->Draw(titleTransform_, uv, {1.0f,1.0f,1.0f,alpha_},returnTextureHandle_);
		if (isFinCount_ && WaveManager::GetInstance()->IsEnd()) {
			//plus_.rotate = clearTransform_.rotate;
			clearSprite_->Draw(plus_, uv, {1.0f,1.0f,1.0f,1.0f},clearTextureHandle_);
		}
		if (isFinClearAnime_ && isUpdate_) {
			newRSprite_->Draw(newRTransform_, uv, { 1.0f,1.0f,1.0f,alpha_ },newRTextureHandle_);
		}
	}
	
	if (isRunAnimation_) {
		Vector4 material = {0,0,0,1.0f};
		if (phase_ == FROMGAME && WaveManager::GetInstance()->IsEnd()) {
			material = {1.0f,1.0f,1.0f,1.0f};
			material.w =1.0f - frameCount_ / float(transitionAnimationLength_);
		}
		Transform pos = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{transitionSpritePosition_.x,transitionSpritePosition_.y,transitionSpritePosition_.z} };
		transitionSprite_->Draw(pos, uv, material, blackTextureHandle_);
	}
}

void ResultScene::DrawBackGround() {
	blueMoon_->SetBlendMode(blendCount_);
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform pos = { {1.0f,1.0f,0.0f},{0.0f,0.0f,0.0f},{0,0,0} };
	backGroundSprite_->Draw(pos, uv, { 1.0f,1.0f,1.0f,1.0f }, backTextureHandle_);
	
}

void ResultScene::TransitionAnimation() {

	transitionSpritePosition_.x = (1.0f - resetT_) * transitionStartPosition_.x + resetT_ * transitionEndPosition_.x;
	transitionSpritePosition_.y = (1.0f - resetT_) * transitionStartPosition_.y + resetT_ * transitionEndPosition_.y;
	transitionSpritePosition_.z = (1.0f - resetT_) * transitionStartPosition_.z + resetT_ * transitionEndPosition_.z;
}
