#include "gameScene.h"
#include "MapManager.h"
#include "GameController.h"
#include "WaveManager.h"
#include "Audio.h"
#include <functional>
GameScene::~GameScene()
{
	enemys_.remove_if([](IEnemy* enemy) {

		delete enemy;
		return true;


		});
	bullets_.remove_if([](PlayerAimBullet* bullet) {
		delete bullet;
		return true;
		});
}

void GameScene::Initialize()
{
	

	sceneNum = 1;
	blueMoon_ = BlueMoon::GetInstance();

	directXCommon_ = DirectXCommon::GetInstance();

	textureManager_ = Texturemanager::GetInstance();
	//textureManager_->Initialize();
	enemyTex_ = textureManager_->Load("resource/black.png");
	viewProjection_.Initialize();

#pragma region
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "GameSetting";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "fallingBorder", fallingBorder_);
	globalVariables->AddItem(groupName, "upperBorder", upperBorder_);
	globalVariables->AddItem(groupName, "horizonBorder", horizonBorder_);

	globalVariables->AddItem(groupName, "transitionAnimationLength", int32_t(transitionAnimationLength_));
	globalVariables->AddItem(groupName, "transitionAnimationDelay", int32_t(transitionAnimationDelay_));
	//タイトルシーンUI
	const char* groupName2 = "Title";
	globalVariables->AddItem(groupName2, "lineScale", lineScale_);
	globalVariables->AddItem(groupName2, "linePosition", linePosition_);
	globalVariables->AddItem(groupName2, "startScale", lineScale_);
	globalVariables->AddItem(groupName2, "startPosition", linePosition_);
	//音楽のボリューム
	const char* groupNameSound = "SoundVolume";
	globalVariables->CreateGroup(groupNameSound);
	globalVariables->AddItem(groupNameSound, "IngameBGM", Audio::GetInstance()->SoundVolume[inGameBGM]);
	globalVariables->AddItem(groupNameSound, "ResultBGM", Audio::GetInstance()->SoundVolume[ResultBGM]);
	globalVariables->AddItem(groupNameSound, "JumpBGM", Audio::GetInstance()->SoundVolume[Jump]);
	globalVariables->AddItem(groupNameSound, "startBGM", Audio::GetInstance()->SoundVolume[GameStart]);
	globalVariables->AddItem(groupNameSound, "ReverceBGM", Audio::GetInstance()->SoundVolume[Reverce]);
	globalVariables->AddItem(groupNameSound, "DeathBGM", Audio::GetInstance()->SoundVolume[Death]);
	globalVariables->AddItem(groupNameSound, "DeleteEnemyBGM", Audio::GetInstance()->SoundVolume[DeleteEnemy]);
	globalVariables->AddItem(groupNameSound, "BlockBGM", Audio::GetInstance()->SoundVolume[Block]);
	globalVariables->AddItem(groupNameSound, "BoundBGM", Audio::GetInstance()->SoundVolume[Bound]);
	globalVariables->AddItem(groupNameSound, "ClearBGM", Audio::GetInstance()->SoundVolume[Clear]);
	globalVariables->AddItem(groupNameSound, "EnemyPopBGM", Audio::GetInstance()->SoundVolume[EnemyPop]);
	globalVariables->AddItem(groupNameSound, "plusWaveBGM", Audio::GetInstance()->SoundVolume[PlusWave]);
#pragma endregion ゲームの設定項目	
	
	MapManager::GetInstance()->Initialize();
	//MapManager::GetInstance()->MapRead();
	MapManager::GetInstance()->SetJoyState(&joyState_);
	MapManager::GetInstance()->SetPreJoyState(&preJoyState_);
	
	player_.reset(new Player);
	player_->Initialize();
	player_->SetJoyState(&joyState_);
	player_->SetPreJoyState(&preJoyState_);
	GameController::GetInstance()->Initialize();

	followCamera_.reset(new FollowCamera);
	followCamera_->Initialize();
	//followCamera_->SetForcusPoint({0,1.0f,100.0f});
	followCamera_->SetTarget(&player_->GetWorldTransform());
	EnemymoveSpeed_ = 0.25f;
	enemyVelocity_ = { -1.0f,0.0f,0.0f };
	type = kBullet;
	enemyTransform = { {3.0f,3.0f,3.0f},{1.0f,1.0f,1.0f},{10.0f,10.0f,0.0f} };
	enemyPop_ = false;
	for (IEnemy* enemy : enemys_ ) {
		delete enemy;
	}
	enemys_.clear();
	bullets_.clear();
	waveNum_ = 0;
	WaveManager::GetInstance()->Initialize();
	WaveManager::GetInstance()->SetEnemyList(&enemys_);
	WaveManager::GetInstance()->SetWave(waveNum_);
	WaveManager::GetInstance()->SetGameScene(this);
	WaveManager::GetInstance()->SetPlayer(player_.get());
	WaveManager::GetInstance()->LoadAllFiles();

	MapManager::GetInstance()->SetShakeCamera(std::bind(&FollowCamera::Shake, followCamera_.get()));
	transitionSprite_.reset(new Sprite);
	transitionSprite_->Initialize({0.0f,0.0f,0.0f,0.0f}, {1280.0f,720.0f,0.0f,0.0f});
	blackTextureHandle_ = textureManager_->Load("resource/startLine.png");
	
	titleLine_.reset(new Plane);
	titleLine_->Initialize();
	worldTransformLine_.Initialize();
	
	titleChar_.reset(new Plane);
	titleChar_->Initialize();
	worldTransformStart_.Initialize();



	titleTransform_.scale = {0.5f,0.5f,0.5f};
	titleTransform_.rotate = {0,0,0};
	titleTransform_.translate = {640,360,0};

	titleSprite_.reset(new Sprite);
	titleSprite_->Initialize({-960,-540,0,0}, { 960,540,0,0 });

	backGroundSprite_.reset(new Sprite);
	backGroundSprite_->Initialize({ 0,0,0,0 }, {1280,720,0,0});
	titleTextureHandle_ = textureManager_->Load("resource/title.png");
	backTextureHandle_ = textureManager_->Load("resource/back.png");
	startTextureHandle_ = textureManager_->Load("resource/gameStart.png");

	fadeTextureHandle_ = textureManager_->Load("resource/white.png");

	globalVariables->AddItem(groupName2, "TitleScale", titleTransform_.scale);
	globalVariables->AddItem(groupName2, "TitleTransform", titleTransform_.translate);

	worldTransformTutorial_.Initialize();
	globalVariables->AddItem(groupName2, "tutorialPosition", worldTransformTutorial_.translation_);
	globalVariables->AddItem(groupName2, "tutorialScale", worldTransformTutorial_.scale_);

	moveSprite_.reset(new Sprite);
	moveSprite_->Initialize({ -800.0f,-200.0f,0,0 }, { 800.0f,200.0f,0,0 });
	jumpSprite_.reset(new Sprite);
	jumpSprite_->Initialize({ -800.0f,-200.0f,0,0 }, { 800.0f,200.0f,0,0 });
	reverseSprite_.reset(new Sprite);
	reverseSprite_->Initialize({ -800.0f,-200.0f,0,0 }, { 800.0f,200.0f,0,0 });

	moveTextureHandle_ = textureManager_->Load("Resource/UI/moveUI.png");
	jumpTextureHandle_ = textureManager_->Load("Resource/UI/jumpUI.png");
	reverseTextureHandle_ = textureManager_->Load("Resource/UI/reversUI.png");
	//モデルの初期化
	ballEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy","ball.obj"));
	targetballEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy", "targetBall.obj"));
	beamEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy", "berm.obj"));
	bulletEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy", "bullet.obj"));
	reverceEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy", "revese.obj"));
	wheelEnemyModel_.reset(Model::CreateModelFromObj("Resource/Enemy", "wheel.obj"));


	const char* groupName3 = "UI";
	globalVariables->AddItem(groupName3, "moveScale", move_.scale);
	globalVariables->AddItem(groupName3, "movePosition", move_.translate);
	globalVariables->AddItem(groupName3, "jumpScale", jump_.scale);
	globalVariables->AddItem(groupName3, "jumpPosition", jump_.translate);
	globalVariables->AddItem(groupName3, "reverseScale", reverse_.scale);
	globalVariables->AddItem(groupName3, "reversePosition", reverse_.translate);
	globalVariables->AddItem(groupName3, "lifeScale", lifeScale_);
	globalVariables->AddItem(groupName3, "lifePosition0", lifeTranslates_[0]);
	globalVariables->AddItem(groupName3, "lifePosition1", lifeTranslates_[1]);
	globalVariables->AddItem(groupName3, "lifePosition2", lifeTranslates_[2]);
	const char* groupNameEnemy = "EnemyData";
	globalVariables->AddItem(groupNameEnemy, "time", BulletStartCount);
	isInGame_ = false;
	isTitle_ = true;
	isStartGame_ = false;
	isStartTutorial_ = false;
	isTutorial_ = false;
	isEndGame_ = false;
	player_->SetLife(3);
	for (int index = 0; index < 3;index++) {
		std::unique_ptr<Sprite> newSprite = std::make_unique<Sprite>();
		newSprite->Initialize({-250.0f,-172.0f,0,0} ,{ 250.0f,172.0f,0,0 });
		lifeSprites_.push_back(move(newSprite));
	}
	lifeTextureHandle_ = textureManager_->Load("Resource/UI/lifeUI.png");
	particletextureHandle= textureManager_->Load("Resource/circle.png");
	particle_ = std::make_unique<Particle>();
	particle_->Initialize(1000000000);
	Transform t = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	ApplyGlobalVariables();
	//particle_->AddParticle({ t }, 10);
	Audio::GetInstance()->SoundPlayloop(Audio::GetInstance()->handle_[inGameBGM], Audio::GetInstance()->SoundVolume[inGameBGM]);

	energyparticleCleateTime_ = -1;
	energyEmmitPoint_ = nullptr;

	lifeDrawerT_ = 0.0f;
	isLifeDecriaceAnimation_ = false;

	worldTransformInnerTutorialArie_.Initialize();
	worldTransformInnerTutorialArie_.scale_.x = 16.0f;
	worldTransformInnerTutorialArie_.scale_.y = 28.0f;
	worldTransformInnerTutorialArie_.translation_ = {46,16,0.1f};
	worldTransformInnerTutorialArie_.UpdateMatrix();
	tutorialArea_.reset(new Plane);
	tutorialArea_->Initialize();

	tutorialTextureHandle_ = textureManager_->Load("Resource/tutorial.png");

}

void GameScene::Update()
{
	

	ApplyGlobalVariables();
	GameController::GetInstance()->Update();
	

	ImGui::Begin("wave");
	ImGui::DragInt("wave", &waveNum_,1,0,24);
	if (ImGui::Button("Stert")) {
		WaveManager::GetInstance()->SetWave(waveNum_);
	}
	ImGui::End();

	ImGui::Begin("testcamera");
	ImGui::DragFloat3("rotate", &viewProjection_.rotation_.x, 0.01f);
	ImGui::DragFloat3("translate", &viewProjection_.translation_.x, 0.01f);

	ImGui::End();
	ImGui::Begin("EnemyPopData");
	if (ImGui::BeginCombo("EnemyType", "Types")) {
		if (ImGui::Selectable("Bullet", type == kBullet)) {
			type = kBullet;
		}
		if (ImGui::Selectable("Bound", type == kBound)) {
			type = kBound;
		}
		if (ImGui::Selectable("Reflect", type == kReflect)) {
			type = kReflect;
		}
		if (ImGui::Selectable("StageDown", type == kStageDown)) {
			type = kStageDown;
		}
		if (ImGui::Selectable("StageUp", type == kStageUp)) {
			type = kStageUp;
		}
		if (ImGui::Selectable("Aimheight", type == kAimBulletHeight)) {
			type = kAimBulletHeight;
		}
		if (ImGui::Selectable("AimWidth", type == kAimBulletWidth)) {
			type = kAimBulletWidth;
		}
		if (ImGui::Selectable("Tire", type == kTire)) {
			type = kTire;
		}
		if (ImGui::Selectable("Beam", type == kRaser)) {
				type = kRaser;
		}
		if (ImGui::Selectable("AimBall", type == kAimBound)) {
			type = kAimBound;
		}
		ImGui::EndCombo();

	}
	ImGui::DragFloat("speed", &EnemymoveSpeed_, 0.05f);
	ImGui::DragFloat3("velocity", &enemyVelocity_.x, 0.05f);
	ImGui::DragFloat3("translate", &enemyTransform.translate.x, 0.05f);
	ImGui::DragFloat3("scale", &enemyTransform.scale.x, 0.05f);
	ImGui::DragInt("timeCount", &BulletStartCount);
	ImGui::Checkbox("POP", &enemyPop_);
	ImGui::End();
	
	
	
	if (isTitle_) {
		Title();
	}
	else if (isTutorial_) {
		Tutorial();
	}
	else if (isInGame_) {
		InGame();
	}
	else {
		if (!isEndGame_) {
			player_->Update();
		}
		if (isStartTutorial_) {
			std::vector<std::shared_ptr<MapManager::Map>>& floors = MapManager::GetInstance()->GetFloor();
			for (std::shared_ptr<MapManager::Map> object : floors) {
				if (IsCollision(player_->GetOBB(), object->obb)) {
					object->isFrameCollision_ = player_->OnCollisionFloorVertical(object->obb);
					object->OnCollision();
				}
				if (IsCollision(player_->GetFloatTrigger(), object->obb)) {
					object->Touch();
				}
			}
			for (std::shared_ptr<MapManager::Map> object : floors) {
				if (IsCollision(player_->GetOBB(), object->obb) && (object->isFrameCollision_ == false)) {
					player_->OnCollisionFloorHorizon(object->obb);
				}
			}

			std::vector<std::shared_ptr<MapManager::Map>>& walls = MapManager::GetInstance()->GetWall();
			for (std::shared_ptr<MapManager::Map> object : walls) {
				if (IsCollision(player_->GetOBB(), object->obb)) {
					player_->OnCollisionWall(object->obb);
				}
			}
		}
		ReStartAnimation();
	}
	if (isRunAnimation_) {
		TransitionAnimation();
	}

	followCamera_->Update();

	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	Matrix4x4 toCameraRotate = Inverse(viewProjection_.matView);
	toCameraRotate.m[3][0] = 0;
	toCameraRotate.m[3][1] = 0;
	toCameraRotate.m[3][2] = 0;

	worldTransformStart_.matWorld_ = Multiply(Multiply( MakeScaleMatrix(worldTransformStart_.scale_) , toCameraRotate),MakeTranslateMatrix(worldTransformStart_.translation_));
	worldTransformStart_.TransferMatrix();
}


void GameScene::Title() {
	
	player_->Update();
	if (player_->GetWorldTransform().GetWorldPos().y < fallingBorder_) {
		followCamera_->Shake();
		ReStart();
	}

	if (player_->GetWorldTransform().GetWorldPos().y > linePosition_.y) {
		isStartGame_ = true;
		isInGame_ = true;
		isTitle_ = false;
		Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[GameStart], Audio::GetInstance()->SoundVolume[GameStart]);

		//WaveManager::GetInstance()->SetWave(0);
	}
	//チュートリアル開始判定
	if (player_->GetWorldTransform().GetWorldPos().x > 40.0f) {
		isTutorial_ = true;
		isStartTutorial_ = true;
		isTitle_ = false;
		Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[GameStart], Audio::GetInstance()->SoundVolume[GameStart]);
		
		isInGame_ = false;
		isTitle_ = false;
		isTutorial_ = false;
		frameCount_ = 60;
		isRunAnimation_ = false;
		//MapManager::GetInstance()->WaveRead(uint32_t(0));
		//WaveManager::GetInstance()->SetWave(uint32_t(0));
	}


	worldTransformLine_.translation_ = linePosition_;
	worldTransformLine_.scale_ = lineScale_;
	worldTransformLine_.UpdateMatrix();
	MapManager::GetInstance()->Update();
	std::vector<std::shared_ptr<MapManager::Map>>& floors = MapManager::GetInstance()->GetFloor();
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			object->isFrameCollision_ = player_->OnCollisionFloorVertical(object->obb);
			object->OnCollision();
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
		if (IsCollision(player_->GetFloatTrigger(), object->obb)) {
			object->Touch();
		}
	}
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb) && (object->isFrameCollision_ == false)) {
			player_->OnCollisionFloorHorizon(object->obb);
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
	}

	std::vector<std::shared_ptr<MapManager::Map>>& walls = MapManager::GetInstance()->GetWall();
	for (std::shared_ptr<MapManager::Map> object : walls) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			player_->OnCollisionWall(object->obb);
		}
	}
	
	if (isRunAnimation_) {
		resetT_ = frameCount_ / float(transitionAnimationLength_);
		resetT_ = std::powf(resetT_ * 2.0f - 1.0f, 2) * -1.0f + 1.0f;

		if (frameCount_ >= transitionAnimationLength_) {
			isRunAnimation_ = false;
		}
		frameCount_++;
	}
	worldTransformStart_.translation_ = startOffset_;
	worldTransformStart_.translation_.y += std::sin(startFloatTheta_) *0.5f;

	worldTransformTutorial_.translation_ = tutorialOffset_;
	worldTransformTutorial_.translation_.y += std::cos(startFloatTheta_) * 0.5f;
	worldTransformTutorial_.UpdateMatrix();


	startFloatTheta_ += 0.1f;
}

void GameScene::InGame() {
	WaveManager::GetInstance()->Update();
	//viewProjection_.UpdateMatrix();
	//viewProjection_.TransferMatrix();

	ImGui::Begin("Scene");

	
	ImGui::InputInt("SceneNum", &sceneNum);
	
	ImGui::End();
	player_->Update();
	if (player_->GetWorldTransform().GetWorldPos().y < fallingBorder_) {
		followCamera_->Shake();
		ReStart();
		Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

	}
	
	bullets_.remove_if([](PlayerAimBullet* bullet) {
		if (!bullet->GetIsAlive()) {
			delete bullet;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[DeleteEnemy], Audio::GetInstance()->SoundVolume[DeleteEnemy]);
			return true;
		}
		return false;
		});
	enemys_.remove_if([](IEnemy* enemy) {
		if (!enemy->GetIsAlive()) {
			delete enemy;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[DeleteEnemy], Audio::GetInstance()->SoundVolume[DeleteEnemy]);

			return true;
		}
		return false;
		});
	
	MapManager::GetInstance()->Update();
	for (IEnemy* enemy : enemys_) {
		enemy->Update();
		if (std::abs(enemy->GetWorldTransform().GetWorldPos().x) > horizonBorder_ ||
			enemy->GetWorldTransform().GetWorldPos().y > upperBorder_ ||
			enemy->GetWorldTransform().GetWorldPos().y < fallingBorder_) {
			enemy->Deth();
		}
	}
	for (PlayerAimBullet* bullet : bullets_) {

		bullet->Update();
	}
	
	std::vector<std::shared_ptr<MapManager::Map>>& floors = MapManager::GetInstance()->GetFloor();
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			object->isFrameCollision_ = player_->OnCollisionFloorVertical(object->obb);
			object->OnCollision();
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
		if (IsCollision(player_->GetFloatTrigger(), object->obb)) {
			object->Touch();
		}
	}
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb) && (object->isFrameCollision_ == false)) {
			player_->OnCollisionFloorHorizon(object->obb);
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
		for (PlayerAimBullet* bullet : bullets_) {
			if (IsCollision(bullet->GetOBB(), object->obb)) {
 				bullet->isCollision();
			}
			if (IsCollision(bullet->GetOBB(), player_->GetOBB())) {
				Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

				ReStart();
				followCamera_->Shake();
			
				return;
			}
			
		}

	}

	std::vector<std::shared_ptr<MapManager::Map>>& walls = MapManager::GetInstance()->GetWall();
	for (std::shared_ptr<MapManager::Map> object : walls) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			player_->OnCollisionWall(object->obb);
		}
	}


	//敵とオブジェクトの当たり判定
	for (IEnemy* enemy : enemys_) {
		//地面と当たった時
		for (std::shared_ptr<MapManager::Map> object : floors) {
			if (IsCollision(enemy->GetOBB(), object->obb) && !enemy->GetIsHit()) {

				if (enemy->GetType() == kStageUp) {
					if (object->worldTransform.translation_.y == 0.0f) {
						object->OnCollision();
						object->Touch();
					}
				}
				if (enemy->GetType() == kStageDown) {
					if (object->worldTransform.translation_.y != 0.0f) {
						object->OnCollision();
						object->Touch();
					}
				}
				Transform particletrans = { enemy->GetWorldTransform().scale_,enemy->GetWorldTransform().rotation_ ,enemy->GetWorldTransform().translation_ };
				if (enemy->GetType() == kTire) {
					Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
					particle_->AddParticle({ particletrans,height,color }, 3);
					
				}
				else {
					Vector4 color = { 1.0f,0.0f,0.0f,1.0f };
					particle_->AddParticle({ particletrans,random,color }, 10);
				}
				enemy->SetPartener(kflore);
				enemy->isCollision(object->obb);
				if (enemy->GetType() == kBound || enemy->GetType() == kReflect || enemy->GetType() == kAimBound) {
					Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Bound], Audio::GetInstance()->SoundVolume[Bound]);

				}
			}
		}
		//反射タイプの敵なら
		if (enemy->GetType() == kReflect) {
			for (std::shared_ptr<MapManager::Map> object : walls) {
				if (IsCollision(enemy->GetOBB(), object->obb)) {
					enemy->SetPartener(kwall);
					enemy->isCollision(object->obb);

				}
			}
		}
		//プレイヤーと当たっていたら
		if (IsCollision(enemy->GetOBB(), player_->GetOBB())) {
			#ifdef _DEBUG
			if (player_->GetColliderFlag() == false) {
				return;
			}
			#endif
			//Initialize();
			ReStart();
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

			followCamera_->Shake();
			return;
		}

	}
	if (!isRunAnimation_) {//クリア
		if (/*WaveManager::GetInstance()->GetWave() + 1 >= WaveManager::GetInstance()->GetMaxWave() && */
			WaveManager::GetInstance()->IsEnd()) {
			//sceneNum = 2;
			//isRunAnimation_;
			isInGame_ = false;
			isTitle_ = false;
			frameCount_ = 0;
			isRunAnimation_ = false;
			isStartGame_ = false;
			isEndGame_ = true;
		}
	}
	if (isRunAnimation_) {
		resetT_ = frameCount_ / float(transitionAnimationLength_);
		resetT_ = std::powf(resetT_ * 2.0f - 1.0f, 2) * -1.0f + 1.0f;
		//resetT_ = 1.0f;
		if (frameCount_ >= transitionAnimationLength_) {
			isRunAnimation_ = false;
		}
		frameCount_++;
	}
	AbsorptionEnergy();
	std::list<ParticleData>* particleData = particle_->GetParticleDate();
	for (std::list<ParticleData>::iterator particleIterator = particleData->begin(); particleIterator != particleData->end(); particleIterator++) {
		if ((*particleIterator).attribute == ABSORPTION) {
			(*particleIterator).velocity = 10.0f*Normalise(Lerp(0.2f, Normalise((*particleIterator).velocity), Normalise(player_->GetWorldTransformBack().GetWorldPos() - (Multiply((*particleIterator).emitter.transform.scale.x,(*particleIterator).transform.translate) + (*particleIterator).emitter.transform.translate))));
		}
	}
	particle_->Update();
}

void GameScene::Tutorial() {
	WaveManager::GetInstance()->TutorialUpdate();
	//viewProjection_.UpdateMatrix();
	//viewProjection_.TransferMatrix();

	ImGui::Begin("Scene");


	ImGui::InputInt("SceneNum", &sceneNum);

	ImGui::End();
	player_->Update();
	if (player_->GetWorldTransform().GetWorldPos().y < fallingBorder_) {
		followCamera_->Shake();
		ReStart();
		Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

	}

	bullets_.remove_if([](PlayerAimBullet* bullet) {
		if (!bullet->GetIsAlive()) {
			delete bullet;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[DeleteEnemy], Audio::GetInstance()->SoundVolume[DeleteEnemy]);
			return true;
		}
		return false;
		});
	enemys_.remove_if([](IEnemy* enemy) {
		if (!enemy->GetIsAlive()) {
			delete enemy;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[DeleteEnemy], Audio::GetInstance()->SoundVolume[DeleteEnemy]);

			return true;
		}
		return false;
		});

	MapManager::GetInstance()->Update();
	for (IEnemy* enemy : enemys_) {
		enemy->Update();
		if (std::abs(enemy->GetWorldTransform().GetWorldPos().x) > horizonBorder_ ||
			enemy->GetWorldTransform().GetWorldPos().y > upperBorder_ ||
			enemy->GetWorldTransform().GetWorldPos().y < fallingBorder_) {
			enemy->Deth();
		}
	}
	for (PlayerAimBullet* bullet : bullets_) {

		bullet->Update();
	}

	std::vector<std::shared_ptr<MapManager::Map>>& floors = MapManager::GetInstance()->GetFloor();
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			object->isFrameCollision_ = player_->OnCollisionFloorVertical(object->obb);
			object->OnCollision();
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
		if (IsCollision(player_->GetFloatTrigger(), object->obb)) {
			object->Touch();
		}
	}
	for (std::shared_ptr<MapManager::Map> object : floors) {
		if (IsCollision(player_->GetOBB(), object->obb) && (object->isFrameCollision_ == false)) {
			player_->OnCollisionFloorHorizon(object->obb);
		}
		for (PlayerAimBullet* bullet : bullets_) {
			if (IsCollision(bullet->GetOBB(), object->obb)) {
				bullet->isCollision();
			}
			if (IsCollision(bullet->GetOBB(), player_->GetOBB())) {
				#ifdef _DEBUG
				if (player_->GetColliderFlag() == false) {
					return;
				}
				#endif
				Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

				ReStart();
				followCamera_->Shake();

				return;
			}

		}

	}

	std::vector<std::shared_ptr<MapManager::Map>>& walls = MapManager::GetInstance()->GetWall();
	for (std::shared_ptr<MapManager::Map> object : walls) {
		if (IsCollision(player_->GetOBB(), object->obb)) {
			player_->OnCollisionWall(object->obb);
			if (player_->GetIsRecovJump()) {
				energyparticleCleateTime_ = 30;
				energyEmmitPoint_ = &object->worldTransform.translation_;
			}
		}
	}
	for (IEnemy* enemy : enemys_) {


		for (std::shared_ptr<MapManager::Map> object : floors) {
			if (IsCollision(enemy->GetOBB(), object->obb) && !enemy->GetIsHit()) {

				if (enemy->GetType() == kStageUp) {
					if (object->worldTransform.translation_.y == 0.0f) {
						object->OnCollision();
						object->Touch();
					}
				}
				if (enemy->GetType() == kStageDown) {
					if (object->worldTransform.translation_.y != 0.0f) {
						object->OnCollision();
						object->Touch();
					}
				}
				Transform particletrans = { enemy->GetWorldTransform().scale_,enemy->GetWorldTransform().rotation_ ,enemy->GetWorldTransform().translation_ };
				if (enemy->GetType() == kTire) {
					Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
					particle_->AddParticle({ particletrans,height,color }, 3);

				}
				else {
					Vector4 color = { 1.0f,0.0f,0.0f,1.0f };
					particle_->AddParticle({ particletrans,random,color }, 10);
				}
				enemy->SetPartener(kflore);
				enemy->isCollision(object->obb);
				if (enemy->GetType() == kBound || enemy->GetType() == kReflect || enemy->GetType() == kAimBound) {
					Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Bound], Audio::GetInstance()->SoundVolume[Bound]);

				}
			}
		}

		if (enemy->GetType() == kReflect) {
			for (std::shared_ptr<MapManager::Map> object : walls) {
				if (IsCollision(enemy->GetOBB(), object->obb)) {
					enemy->SetPartener(kwall);
					enemy->isCollision(object->obb);

				}
			}
		}
		if (IsCollision(enemy->GetOBB(), player_->GetOBB())) {
			#ifdef _DEBUG
			if (player_->GetColliderFlag() == false) {
				return;
			}
			#endif
			ReStart();
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Death], Audio::GetInstance()->SoundVolume[Death]);

			followCamera_->Shake();
			return;
		}

	}
	if (!isRunAnimation_) {
		if (WaveManager::GetInstance()->GetWave() + 1 >= WaveManager::GetInstance()->GetMaxWave() &&
			WaveManager::GetInstance()->IsEnd()) {
			isInGame_ = false;
			isTitle_ = false;
			frameCount_ = 0;
			isRunAnimation_ = false;
			isStartGame_ = false;
			isEndGame_ = true;
		}
	}
	if (isRunAnimation_) {
		resetT_ = frameCount_ / float(transitionAnimationLength_);
		resetT_ = std::powf(resetT_ * 2.0f - 1.0f, 2) * -1.0f + 1.0f;
		//resetT_ = 1.0f;
		if (frameCount_ >= transitionAnimationLength_) {
			isRunAnimation_ = false;
		}
		frameCount_++;
	}
	AbsorptionEnergy();
	std::list<ParticleData>* particleData = particle_->GetParticleDate();
	for (std::list<ParticleData>::iterator particleIterator = particleData->begin(); particleIterator != particleData->end(); particleIterator++) {
		if ((*particleIterator).attribute == ABSORPTION) {
			(*particleIterator).velocity = 1.0f * (Lerp(0.2f, ((*particleIterator).velocity), (player_->GetWorldTransformBack().GetWorldPos() - (Multiply((*particleIterator).emitter.transform.scale.x, (*particleIterator).transform.translate) + (*particleIterator).emitter.transform.translate))));
		}
	}
	particle_->Update();
	if(WaveManager::GetInstance()->IsClearTutorial()) {
		for (IEnemy* enemy : enemys_) {
			delete enemy;
		}
		for (PlayerAimBullet* bullet : bullets_) {
			delete bullet;
		}
		particle_->Cler();
		bullets_.clear();
		enemys_.clear();
		isInGame_ = false;
		isTitle_ = false;
		isTutorial_ = false;
		frameCount_ = 0;
		isRunAnimation_ = false;
	}
}

void GameScene::Draw()
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

void GameScene::Draw3D()
{
	blueMoon_->ModelPreDraw();

	MapManager::GetInstance()->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	for (IEnemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}
	for (PlayerAimBullet* bullet : bullets_) {

		bullet->Draw(viewProjection_);
	}
	if (!isStartGame_ && !isEndGame_ && !isStartTutorial_) {
		titleLine_->Draw(worldTransformLine_, viewProjection_, { 1.0f,1.0f ,1.0f ,1.0f }, blackTextureHandle_);
		titleChar_->Draw(worldTransformStart_, viewProjection_,{1.0f,1.0f,1.0f,1.0f},startTextureHandle_);

		tutorialArea_->Draw(worldTransformInnerTutorialArie_, viewProjection_, { 0.8f,0.0f,0.0f,0.8f }, fadeTextureHandle_);
		titleChar_->Draw(worldTransformTutorial_, viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, tutorialTextureHandle_);
	}
	WaveManager::GetInstance()->Draw3D(viewProjection_);
	blueMoon_->PariclePreDraw();
	
		particle_->Draw(viewProjection_, { 1.0f,1.0f,1.0f,1.0f }, particletextureHandle);
	
	blueMoon_->ModelPreDrawWireFrame();


}

void GameScene::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "GameSetting";
	fallingBorder_ = globalVariables->GetFloatValue(groupName, "fallingBorder");
	upperBorder_ = globalVariables->GetFloatValue(groupName, "upperBorder");
	horizonBorder_ = globalVariables->GetFloatValue(groupName, "horizonBorder");
	transitionAnimationLength_ = globalVariables->GetIntValue(groupName, "transitionAnimationLength");
	transitionAnimationDelay_ = globalVariables->GetIntValue(groupName, "transitionAnimationDelay");


	const char* groupName2 = "Title";
	lineScale_ = globalVariables->GetVector3Value(groupName2, "lineScale");
	linePosition_ = globalVariables->GetVector3Value(groupName2, "linePosition");

	titleTransform_.scale = globalVariables->GetVector3Value(groupName2, "TitleScale");
	titleTransform_.translate = globalVariables->GetVector3Value(groupName2, "TitleTransform");
	worldTransformStart_.scale_ = globalVariables->GetVector3Value(groupName2, "startScale");
	startOffset_ = globalVariables->GetVector3Value(groupName2, "startPosition");

	tutorialOffset_ = globalVariables->GetVector3Value(groupName2, "tutorialPosition");
	worldTransformTutorial_.scale_ = globalVariables->GetVector3Value(groupName2, "tutorialScale");


	const char* groupName3 = "UI";
	move_.scale = globalVariables->GetVector3Value(groupName3, "moveScale");
	move_.translate = globalVariables->GetVector3Value(groupName3, "movePosition");
	jump_.scale = globalVariables->GetVector3Value(groupName3, "jumpScale");
	jump_.translate = globalVariables->GetVector3Value(groupName3, "jumpPosition");
	reverse_.scale = globalVariables->GetVector3Value(groupName3, "reverseScale");
	reverse_.translate = globalVariables->GetVector3Value(groupName3, "reversePosition");

	lifeScale_ = globalVariables->GetVector3Value(groupName3, "lifeScale");

	lifeLeftTopPosition_ = globalVariables->GetVector3Value(groupName3, "lifePosition1");



	const char* groupNameEnemy = "EnemyData";
	
	BulletStartCount = globalVariables->GetIntValue(groupNameEnemy, "time");

	
	const char* groupNameSound = "SoundVolume";

	Audio::GetInstance()->SoundVolume[inGameBGM] = globalVariables->GetFloatValue(groupNameSound, "IngameBGM");
	Audio::GetInstance()->SoundVolume[ResultBGM] = globalVariables->GetFloatValue(groupNameSound, "ResultBGM");
	Audio::GetInstance()->SoundVolume[Jump] = globalVariables->GetFloatValue(groupNameSound, "JumpBGM");
	Audio::GetInstance()->SoundVolume[GameStart] = globalVariables->GetFloatValue(groupNameSound, "startBGM");
	Audio::GetInstance()->SoundVolume[Reverce] = globalVariables->GetFloatValue(groupNameSound, "ReverceBGM");
	Audio::GetInstance()->SoundVolume[Death] = globalVariables->GetFloatValue(groupNameSound, "DeathBGM");
	Audio::GetInstance()->SoundVolume[DeleteEnemy] = globalVariables->GetFloatValue(groupNameSound, "DeleteEnemyBGM");
	
	Audio::GetInstance()->SoundVolume[Block] = globalVariables->GetFloatValue(groupNameSound, "BlockBGM");
	Audio::GetInstance()->SoundVolume[Bound] = globalVariables->GetFloatValue(groupNameSound, "BoundBGM");
	Audio::GetInstance()->SoundVolume[Clear] = globalVariables->GetFloatValue(groupNameSound, "ClearBGM");
	Audio::GetInstance()->SoundVolume[EnemyPop] = globalVariables->GetFloatValue(groupNameSound, "EnemyPopBGM");
	Audio::GetInstance()->SoundVolume[PlusWave] = globalVariables->GetFloatValue(groupNameSound, "plusWaveBGM");
	
	
}

void GameScene::Draw2D() {
	blueMoon_->SetBlendMode(blendCount_);
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };

	moveSprite_->Draw(move_, uv, {1.0f,1.0f,1.0f,1.0f},moveTextureHandle_);
	Vector4 jumpmaterial= { 1.0f,1.0f,1.0f,1.0f };
	if (!player_->IsJumpAble()) {
		jumpmaterial.w = 0.5f;
	}
	jumpSprite_->Draw(jump_, uv, jumpmaterial, jumpTextureHandle_);
	Vector4 reversMaterial= { 1.0f,1.0f,1.0f,1.0f };
	if (!MapManager::GetInstance()->IsReversAble()) {
		reversMaterial.w = 0.5f;
	}
	reverseSprite_->Draw(reverse_, uv, reversMaterial, reverseTextureHandle_);
	if (!isStartGame_ && !isEndGame_ && !isStartTutorial_){
		titleSprite_->Draw(titleTransform_, uv, {1.0f,1.0f,1.0f,1.0f},titleTextureHandle_);
	}
	else if(isStartGame_){
		if (isLifeDecriaceAnimation_) {
			Transform lifeTransform;
			lifeTransform.scale = lifeScale_;
			lifeTransform.rotate = { 0,0,0 };
			lifeTranslates_[1] = Lerp(lifeDrawerT_, lifeLeftTopPosition_, { 640,360,0 });
			int life = std::clamp(player_->GetLife(), 0, 3);
			if (isLifeDecriaceAnimation_) {
				life++;
				lifeTranslates_[1].y += lifeUpDown_;
			}
			lifeTranslates_[0] = lifeTranslates_[1];
			lifeTranslates_[0].x -= 100.0f;
			lifeTranslates_[2] = lifeTranslates_[1];
			lifeTranslates_[2].x += 100.0f;
			for (int index = 0; index < life; index++) {
				lifeTransform.translate = lifeTranslates_[index];
				Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
				if (index >= player_->GetLife()) {
					color.w = decriaceAnimationAlpha_;
				}
				lifeSprites_[size_t(index)]->Draw(lifeTransform, uv, color, lifeTextureHandle_);
			}
		}
	}
	if (isRunAnimation_) {
		Vector4 fadematerial = {0.0f,0.0f,0.0f,1.0f};
		if (isEndGame_) {
			fadematerial = { 1.0f,1.0f,1.0f,1.0f };
			fadematerial.w = (float(frameCount_) / float(transitionAnimationLength_)) * 2.0f;
		}
		Transform pos = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{transitionSpritePosition_.x,transitionSpritePosition_.y,transitionSpritePosition_.z} };
		transitionSprite_->Draw(pos, uv,fadematerial , fadeTextureHandle_);
	}
}

void GameScene::DrawBackGround() {
	blueMoon_->SetBlendMode(blendCount_);
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform pos = { {1.0f,1.0f,0.0f},{0.0f,0.0f,0.0f},{0,0,0} };
	backGroundSprite_->Draw(pos, uv, { 1.0f,1.0f,1.0f,1.0f }, backTextureHandle_);

	if (isStartGame_ && !isLifeDecriaceAnimation_) {
		Transform lifeTransform;
		lifeTransform.scale = lifeScale_;
		lifeTransform.rotate = { 0,0,0 };
		lifeTranslates_[1] = Lerp(lifeDrawerT_, lifeLeftTopPosition_, { 640,360,0 });
		int life = std::clamp(player_->GetLife(), 0, 3);
		if (isLifeDecriaceAnimation_) {
			life++;
			lifeTranslates_[1].y += lifeUpDown_;
		}
		lifeTranslates_[0] = lifeTranslates_[1];
		lifeTranslates_[0].x -= 100.0f;
		lifeTranslates_[2] = lifeTranslates_[1];
		lifeTranslates_[2].x += 100.0f;
		for (int index = 0; index < life; index++) {
			lifeTransform.translate = lifeTranslates_[index];
			Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
			if (index >= player_->GetLife()) {
				color.w = decriaceAnimationAlpha_;
			}
			lifeSprites_[size_t(index)]->Draw(lifeTransform, uv, color, lifeTextureHandle_);
		}
	}

	if (isStartGame_ || isEndGame_) {
		WaveManager::GetInstance()->Draw();
	}
	if (isTutorial_) {
		WaveManager::GetInstance()->DrawTutorial();
	}
}

void GameScene::Finalize()
{
	enemys_.remove_if([](IEnemy* enemy) {

		delete enemy;
		return true;


		});
}

void GameScene::AddEnemyBullet(PlayerAimBullet* enemyBullet)
{
	bullets_.push_back(enemyBullet);
}

void GameScene::ReStartWave()
{
	for (IEnemy* enemy : enemys_) {
		delete enemy;
	}
	for (PlayerAimBullet* bullet : bullets_) {
		delete bullet;
	}
	enemys_.clear();
	bullets_.clear();
	if (player_->GetLife() <= 0) {
		sceneNum = 2;
		Audio::GetInstance()->StopAudio(Audio::GetInstance()->handle_[inGameBGM]);

	}
	if (WaveManager::GetInstance()->IsEnd()) {
		Audio::GetInstance()->StopAudio(Audio::GetInstance()->handle_[inGameBGM]);

		sceneNum = 2;
	}
	size_t num = WaveManager::GetInstance()->GetWave();
	MapManager::GetInstance()->WaveRead(uint32_t(num));
	WaveManager::GetInstance()->SetWave(uint32_t(num));
	player_->Reset(MapManager::GetInstance()->GetCenterHeight());
}

void GameScene::ReStartTutorial()
{
	for (IEnemy* enemy : enemys_) {
		delete enemy;
	}
	for (PlayerAimBullet* bullet : bullets_) {
		delete bullet;
	}
	enemys_.clear();
	bullets_.clear();
	
	size_t num = WaveManager::GetInstance()->GetWave();
	if (WaveManager::GetInstance()->IsClearTutorial()) {
		num++;
	}
	if (WaveManager::GetInstance()->IsEnd()) {
		//チュートリアル終了、InGameへ
		//Audio::GetInstance()->StopAudio(Audio::GetInstance()->handle_[inGameBGM]);
		WaveManager::GetInstance()->SetIsEnd(false);
		//sceneNum = 2;
		isStartGame_ = true;
		isInGame_ = true;
		isStartTutorial_ = false;
		num = 0;
	}


	MapManager::GetInstance()->WaveReadTutorial(uint32_t(num));
	WaveManager::GetInstance()->SetWave(uint32_t(num));
	player_->Reset(MapManager::GetInstance()->GetCenterHeight());
}

void GameScene::ReStart()
{
	for (IEnemy* enemy : enemys_) {
		delete enemy;
	}
	for (PlayerAimBullet* bullet : bullets_) {
		delete bullet;
	}
	particle_->Cler();
	bullets_.clear();
	enemys_.clear();
	player_->DethAnimation();
	if (isInGame_ && isStartGame_) {
		int life = player_->GetLife();
		player_->SetLife(life - 1);
	}
	isInGame_ = false;
	isTitle_ = false;
	isTutorial_ = false;
	frameCount_ = 0;
	isRunAnimation_ = false;
}

void GameScene::ReStartAnimation() {
	if (!isRunAnimation_ && frameCount_ >= transitionAnimationDelay_){
		isRunAnimation_ = true;
		frameCount_ = 0;
		lifeDrawerT_ = 1.0f;
	}
	if (!isRunAnimation_ && !isEndGame_) {
		lifeDrawerT_ = frameCount_ / float(transitionAnimationDelay_);
		lifeDrawerT_ *= 4.0f;
		lifeDrawerT_ = std::clamp(lifeDrawerT_, 0.0f, 1.0f);
		isLifeDecriaceAnimation_ = true;
		lifeUpDown_ = (frameCount_ - float(transitionAnimationDelay_ * 0.75f)) / float(transitionAnimationDelay_*0.25f);
		lifeUpDown_ = std::clamp(lifeUpDown_,0.0f,1.0f);
		decriaceAnimationAlpha_ = 1.0f - lifeUpDown_;
		lifeUpDown_ *= 2.0f;
		lifeUpDown_ -= 1.0f;
		lifeUpDown_ *= lifeUpDown_;
		lifeUpDown_ -= 1.0f;
		lifeUpDown_ *= -80.0f;
	}
	if (isRunAnimation_) {
		resetT_ = 1.0f;
		//isLifeDecriaceAnimation_ = false;
		if (!isEndGame_) {
			resetT_ = frameCount_ / float(transitionAnimationLength_);
			resetT_ = std::powf(resetT_ * 2.0f - 1.0f, 2) * -1.0f + 1.0f;

		}
		
		if (frameCount_ == transitionAnimationLength_/2) {
			lifeDrawerT_ = 0.0f;
			isLifeDecriaceAnimation_ = false;
			if (isStartTutorial_) {
				ReStartTutorial();
			}
			else {
				ReStartWave();
				lifeDrawerT_ = 0.0f;
			}
			if (isStartGame_) {
				isInGame_ = true;
			}
			else if (isStartTutorial_) {
				isTutorial_ = true;
			}
			else {
				isTitle_ = true;
			}
		}

		if (frameCount_ >= transitionAnimationLength_) {
			isRunAnimation_ = false;
			isInGame_ = true;
		}
	}
	frameCount_++;
}

void GameScene::TransitionAnimation() {

	transitionSpritePosition_.x = (1.0f - resetT_) * transitionStartPosition_.x + resetT_ * transitionEndPosition_.x;
	transitionSpritePosition_.y = (1.0f - resetT_) * transitionStartPosition_.y + resetT_ * transitionEndPosition_.y;
	transitionSpritePosition_.z = (1.0f - resetT_) * transitionStartPosition_.z + resetT_ * transitionEndPosition_.z;
}

void GameScene::AbsorptionEnergy() {
	if (energyparticleCleateTime_ >0) {
		Transform particletrans = { {2.0f,2.0f,2.0f },{0,0,0}, *energyEmmitPoint_};
		Vector4 color = { 0.0f,1.0f,0.0f,1.0f };
		particle_->AddParticle({ particletrans,absorption,color }, 1);

		energyparticleCleateTime_--;
	}
}