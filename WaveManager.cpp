#include "WaveManager.h"
#include "MapManager.h"
#include"game/Enemy/IEnemy.h"
#include"game/Enemy/BoundEnemy.h"
#include"game/Enemy/ReflectEnemy.h"
#include"game/Enemy/BulletEnemy.h"
#include"game/Enemy/StageChangeEnemy.h"
#include"game/Enemy/AimBulletEnemy.h"
#include"game/Enemy/PlayerAimBullet.h"
#include "game/Scenes/gameScene.h"
#include "Audio.h"
#include <fstream>
#include <sstream>
WaveManager* WaveManager::GetInstance() {
	static WaveManager instance;
	return &instance;
}

void WaveManager::LoadAllFiles() {
	maxWaveNum_ = 0;
	waves_.clear();
	waves_.shrink_to_fit();
	LoadFile("Resource/Wave/waveData.wave");
	LoadFile("Resource/Wave/waveData2.wave");
	LoadFile("Resource/Wave/waveData3.wave");

	tutorialWaves_.clear();
	tutorialWaves_.shrink_to_fit();
	LoadTutorialFile("Resource/Wave/waveTutorialData.wave");
	LoadTutorialFile("Resource/Wave/waveTutorialData2.wave");
	LoadTutorialFile("Resource/Wave/waveTutorialData3.wave");
}

void WaveManager::LoadFile(const char filename[]) {
	std::string line;
	std::ifstream file(filename);
	//assert(file.is_open());
	if (!file.is_open()) {
		return;
	}
	EnemyData* newEnemyData = nullptr;
	Wave* newWave = nullptr;
	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;
		if (identifilter == "wave") {
			newWave = &waves_.emplace_back();
			maxWaveNum_++;
		}
		else if (identifilter == "enemy") {
			if (newWave) {
				newEnemyData = &newWave->enemyDatas.emplace_back();
			}
		}
		else if (identifilter == "position") {
			Vector3 position;
			s >> position.x >> position.y >> position.z;
			if (newEnemyData) {
				//マップからトランスフォーム値に変換
				newEnemyData->translate.x = float(position.x) * 2.0f * 2.0f;
				newEnemyData->translate.y = float(position.y) * 2.0f * 2.0f;
				newEnemyData->translate.z = float(position.z) * 2.0f * 2.0f;
			}
		}
		else if (identifilter == "velocity") {
			if (newEnemyData) {
				s >> newEnemyData->velocity.x >> newEnemyData->velocity.y >> newEnemyData->velocity.z;
			}
		}
		else if (identifilter == "speed") {
			if (newEnemyData) {
				s >> newEnemyData->speed;
			}
		}
		else if (identifilter == "type") {
			int32_t type;
			s >> type;
			if (newEnemyData) {
				newEnemyData->type = EnemyType(type);
			}
		}
		else if (identifilter == "frame") {
			if (newEnemyData) {
				s >> newEnemyData->frame;
			}
		}
		else if (identifilter == "length") {
			if (newWave) {
				s >> newWave->length;
			}
		}
	}

}

void WaveManager::LoadTutorialFile(const char filename[]) {
	std::string line;
	std::ifstream file(filename);
	//assert(file.is_open());
	if (!file.is_open()) {
		return;
	}
	EnemyData* newEnemyData = nullptr;
	Wave* newWave = nullptr;
	while (std::getline(file, line)) {
		std::string identifilter;
		std::istringstream s(line);
		s >> identifilter;
		if (identifilter == "wave") {
			newWave = &tutorialWaves_.emplace_back();
			maxWaveNum_++;
		}
		else if (identifilter == "enemy") {
			if (newWave) {
				newEnemyData = &newWave->enemyDatas.emplace_back();
			}
		}
		else if (identifilter == "position") {
			Vector3 position;
			s >> position.x >> position.y >> position.z;
			if (newEnemyData) {
				//マップからトランスフォーム値に変換
				newEnemyData->translate.x = float(position.x) * 2.0f * 2.0f;
				newEnemyData->translate.y = float(position.y) * 2.0f * 2.0f;
				newEnemyData->translate.z = float(position.z) * 2.0f * 2.0f;
			}
		}
		else if (identifilter == "velocity") {
			if (newEnemyData) {
				s >> newEnemyData->velocity.x >> newEnemyData->velocity.y >> newEnemyData->velocity.z;
			}
		}
		else if (identifilter == "speed") {
			if (newEnemyData) {
				s >> newEnemyData->speed;
			}
		}
		else if (identifilter == "type") {
			int32_t type;
			s >> type;
			if (newEnemyData) {
				newEnemyData->type = EnemyType(type);
			}
		}
		else if (identifilter == "frame") {
			if (newEnemyData) {
				s >> newEnemyData->frame;
			}
		}
		else if (identifilter == "length") {
			if (newWave) {
				s >> newWave->length;
			}
		}
	}

}

void WaveManager::Initialize() {
	lowNum0.reset(new Sprite);
	lowNum0->Initialize({ -500.0f,-500.0f,0,0 }, { 500.0f,500.0f,0,0 });
	lowNum1.reset(new Sprite);
	lowNum1->Initialize({ -500.0f,-500.0f,0,0 }, { 500.0f,500.0f,0,0 });
	highNum0.reset(new Sprite);
	highNum0->Initialize({ -500.0f,-500.0f,0,0 }, { 500.0f,500.0f,0,0 });
	highNum1.reset(new Sprite);
	highNum1->Initialize({ -500.0f,-500.0f,0,0 }, { 500.0f,500.0f,0,0 });
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

	drawerWaveNum_ = 0;
	centerPos0_ = right_;
	topPos0_ = right_;
	topPos0_.y -= 500;
	downPos0_ = right_;
	downPos0_.y += 500;
	centerPos1_ = left_;
	topPos1_ = left_;
	topPos1_.y -= 500;
	downPos1_ = left_;
	downPos1_.y += 500;
	s0 = centerPos0_;
	s1 = topPos0_;
	s2 = centerPos1_;
	s3 = topPos1_;
	num0 = 1;
	num1 = 2;
	num2 = 0;
	num3 = 0;

	waveSprite_.reset(new Sprite);
	waveSprite_->Initialize({ -1150.0f,-500.0f,0,0 }, { 1150.0f,500.0f,0,0 });
	waveTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/wavetex/wave.png");
	isChangeNum_ = false;
	isChange10 = false;
	isEnd_ = false;

	finalSprite_.reset(new Sprite);
	finalSprite_->Initialize({ -500,-160,0,0 }, { 500,160,0,0 });
	finalTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/final.png");
	t_ = 0.0f;
	isFinal_ = false;

	yokeroTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/yokero.png");
	isFirst_ = false;

	tutorialSprite_.reset(new Sprite);
	tutorialSprite_->Initialize({ -800,-400.0,0 }, { 800,400.0,0 });

	tutorialSprite1_.reset(new Sprite);
	tutorialSprite1_->Initialize({ -800,-300.0,0 }, { 800,300.0,0 });

	//tutorialSprite2_.reset(new Sprite);
	//tutorialSprite2_->Initialize({ -800,-300.0,0 }, { 800,400.0,0 });


	tutorialTextureHandles_[0] = Texturemanager::GetInstance()->Load("Resource/UI/bigJumpUI.png");
	tutorialTextureHandles_[1] = Texturemanager::GetInstance()->Load("Resource/UI/Jump.png");
	tutorialTextureHandles_[2] = Texturemanager::GetInstance()->Load("Resource/UI/reversUI.png");
	tutorialTextureHandles_[3] = Texturemanager::GetInstance()->Load("Resource/UI/jumpUI.png");

	cortionPlane_.reset(new Plane);
	cortionPlane_->Initialize();
	cortionTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/mark.png");
	arrowTextureHandle_ = Texturemanager::GetInstance()->Load("Resource/yajirushi.png");
	for (size_t index = 0; index < kCortionMax_; index++) {
		worldTransformCortions_[index].Initialize();
		worldTransformArrows_[index].Initialize();
	}
}

void WaveManager::Update() {
	isChangeWave_ = false;
	isClearTutorialWave_ = false;
	cortion_.clear();
	bool isPopWait = false;//出現待ちのエネミーがいるかどうか
	for (EnemyData& enemy : waves_[size_t(waveNum_)].enemyDatas) {
		if (currentFrame_ >= enemy.frame - cortionDrawFrame_ && currentFrame_ < enemy.frame && cortion_.size() < kCortionMax_ - 1) {
			cortion_.push_back(enemy.translate);
		}
		if (currentFrame_ == enemy.frame) {
			//Enemyの生成処理	
			IEnemy* newEnemy;

			Transform transform;
			transform.scale = { 1.0f,1.0f,1.0f };
			transform.rotate = { 0,0,0 };
			transform.translate = enemy.translate;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[EnemyPop], Audio::GetInstance()->SoundVolume[EnemyPop]);
			Vector4 color = { 0.0f,1.0f,0.0f,1.0f };
			gameScene_->particle_->AddParticle({ transform,random,color }, 10);
			switch (enemy.type)
			{
			case kBullet://0
				newEnemy = new BulletEnemy();
				newEnemy->SetStartCount(gameScene_->BulletStartCount);
				transform.scale = { 1.5, 2, 2 };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->bulletEnemyModel_.get());

				enemyList_->push_back(newEnemy);
				break;
			case kReflect://1
				//使われていない
				/*newEnemy = new ReflectEnemy();
				newEnemy->SetStartCount(gameScene_->BulletStartCount);
				transform.scale = { 3.0f,3.0f,3.0f };

				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->reverceEnemyModel_.get());
				enemyList_->push_back(newEnemy);*/
				break;
			case kBound://2
				//バウンド敵
				newEnemy = new BoundEnemy();
				transform.scale = { 3.0f,3.0f,3.0f };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->ballEnemyModel_.get());
				enemyList_->push_back(newEnemy); break;
			case kTire://3
				//タイヤ
				newEnemy = new TireEnemy();
				transform.scale = { 3.0f,3.0f,3.0f };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->wheelEnemyModel_.get());
				enemyList_->push_back(newEnemy);
				break;
			case kSpear://4
				break;
			case kRaser://5
				//使われていない
				/*newEnemy = new BeamEnemy();
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->reverceEnemyModel_.get());
				newEnemy->SetStartCount(gameScene_->BulletStartCount);
				enemyList_->push_back(newEnemy);*/
				break;
			case kAimBulletWidth://6
				//自分を狙う敵(縦移動)
				newEnemy = new AImBulletWidthEnemy();
				transform.scale = { 3.0f,3.0f,3.0f };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->bulletEnemyModel_.get());
				newEnemy->SetPlayer(player_);
				newEnemy->SetGameScene(gameScene_);
				enemyList_->push_back(newEnemy);
				break;
			case kAimBulletHeight://7
				//自分を狙う敵(横移動)
				newEnemy = new AimBulletEnemy();
				transform.scale = { 3.0f,3.0f,3.0f };
				newEnemy->SetPlayer(player_);
				newEnemy->SetGameScene(gameScene_);
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->bulletEnemyModel_.get());
				enemyList_->push_back(newEnemy);
				break;
			case kAimBound://8
				//プレイヤーを追いかけて突進する敵
				newEnemy = new PlayerAimBallEnemy();
				transform.scale = { 3.0f,3.0f,3.0f };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->targetballEnemyModel_.get());
				newEnemy->SetPlayer(player_);
				newEnemy->SetGameScene(gameScene_);
				enemyList_->push_back(newEnemy);
				break;
			case kStageUp://9
				//使われていない
				/*newEnemy = new StageChangeEnemy();
				newEnemy->SetType(kStageUp);
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->reverceEnemyModel_.get());
				enemyList_->push_back(newEnemy);*/
				break;
			case kStageDown://10
				//使われていない
				//newEnemy = new StageChangeEnemy();
				//newEnemy->SetType(kStageDown);
				//newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->reverceEnemyModel_.get());
				//enemyList_->push_back(newEnemy);
				break;
			case kHoming://11
				break;
			default://else
				//使われていない
				//newEnemy = new ReflectEnemy();
				//newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->ballEnemyModel_.get());
				//enemyList_->push_back(newEnemy);
				break;
			}

		}
		if (!isPopWait && currentFrame_ <= enemy.frame) {
			isPopWait = true;
		}
	}
	currentFrame_++;

	if (waveNum_ < 5) {
		isFirst_ = true;
		firstAlpha_ = float(5 - waveNum_) / 5.0f;
		t_ += 0.1f;
		if (t_ > 1.0f) {
			t_ = 1.0f;
		}
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;

		finalScale_ = float(1 + c3 * std::pow(t_ - 1.0f, 3) + c1 * std::pow(t_ - 1.0f, 2));

	}
	else if (!(waves_.size() - 1 > waveNum_)) {
		isFinal_ = true;
		t_ += 0.1f;
		if (t_ > 1.0f) {
			t_ = 1.0f;
		}
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;

		finalScale_ = float(1 + c3 * std::pow(t_ - 1.0f, 3) + c1 * std::pow(t_ - 1.0f, 2));

	}
	else {
		isFirst_ = false;
		t_ = 0.0f;
	}
	if (currentFrame_ >= waves_[size_t(waveNum_)].length + waveInterval_ || ((!isPopWait && enemyList_->size() == 0) && (waves_.size() - 1 != waveNum_))) {
		if (waves_.size() - 1 > waveNum_) {
			isEnd_ = false;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[PlusWave], Audio::GetInstance()->SoundVolume[PlusWave]);
			isChangeWave_ = true;
			waveNum_++;
		}
		else {
			isEnd_ = true;
		}
		currentFrame_ = 0;
	}
	if (waveNum_ > drawerWaveNum_ && !isChangeNum_) {
		num0 = int32_t(drawerWaveNum_ + 1) % 10;
		num1 = num0 + 1;
		if (num1 >= 10) {
			num1 = 0;
			num3 += 1;
			isChange10 = true;
		}
		isChangeNum_ = true;
		changeAnimationCount_ = 0;
		drawerWaveNum_++;
	}
	if (isChangeNum_) {
		ChangeNumAnimation();
	}
}

void WaveManager::TutorialUpdate() {
	cortion_.clear();
	for (EnemyData& enemy : tutorialWaves_[size_t(waveNum_)].enemyDatas) {
		if (currentFrame_ >= enemy.frame - cortionDrawFrame_ && currentFrame_ < enemy.frame && cortion_.size() < kCortionMax_ - 1) {
			cortion_.push_back(enemy.translate);
		}
		if (currentFrame_ == enemy.frame) {
			//Enemyの生成処理	
			IEnemy* newEnemy;

			Transform transform;
			transform.scale = { 1.0f,1.0f,1.0f };
			transform.rotate = { 0,0,0 };
			transform.translate = enemy.translate;
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[EnemyPop], Audio::GetInstance()->SoundVolume[EnemyPop]);
			Vector4 color = { 0.0f,1.0f,0.0f,1.0f };
			gameScene_->particle_->AddParticle({ transform,random,color }, 10);
			switch (enemy.type)
			{
			case kBullet://0
				newEnemy = new BulletEnemy();
				newEnemy->SetStartCount(gameScene_->BulletStartCount);
				transform.scale = { 1.5, 2, 2 };
				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->bulletEnemyModel_.get());

				enemyList_->push_back(newEnemy);
				break;
			default://else
				newEnemy = new ReflectEnemy();

				newEnemy->Initialize(transform, enemy.velocity, enemy.speed, 2, gameScene_->ballEnemyModel_.get());
				enemyList_->push_back(newEnemy);
				break;
			}

		}
	}
	currentFrame_++;

	if (waveNum_ < 5) {
		//isFirst_ = true;
		//firstAlpha_ = float(5 - waveNum_) / 5.0f;
		t_ += 0.1f;
		if (t_ > 1.0f) {
			t_ = 1.0f;
		}
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;

		finalScale_ = float(1 + c3 * std::pow(t_ - 1.0f, 3) + c1 * std::pow(t_ - 1.0f, 2));

	}

	else {
		isFirst_ = false;
		t_ = 0.0f;
	}
	isClearTutorialWave_ = false;
	if (currentFrame_ == tutorialWaves_[size_t(waveNum_)].length + waveInterval_) {
		Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[PlusWave], Audio::GetInstance()->SoundVolume[PlusWave]);
	}
	if (currentFrame_ >= tutorialWaves_[size_t(waveNum_)].length + waveInterval_) {
		isClearTutorialWave_ = true;
		if (tutorialWaves_.size() - 1 > waveNum_) {
			isEnd_ = false;
			//Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[PlusWave], Audio::GetInstance()->SoundVolume[PlusWave]);
			//waveNum_++;
		}
		else {
			isEnd_ = true;
		}
		//currentFrame_ = 0;
	}
}

void WaveManager::Draw() {
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform transform = { { 0.1f,0.1f,0.1f },{0,0,0},{0,0,0} };
	transform.translate = s0;
	lowNum0->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[num0]);
	transform.translate = s1;
	highNum0->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[num1]);
	transform.translate = s2;
	lowNum1->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[num2]);
	transform.translate = s3;
	highNum1->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, numberTextureHandle_[num3]);
	transform.translate = { 640.0f,250.0f,0 };
	waveSprite_->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, waveTextureHandle_);

	if (isFinal_) {
		transform.translate = { 700.0f,200.0f,0 };
		transform.rotate.z = 0.3f;
		transform.scale = Vector3{ 0.3f,0.3f,0.3f } *finalScale_;
		finalSprite_->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, finalTextureHandle_);
	}
	if (isFirst_) {
		transform.translate = { 700.0f,200.0f,0 };
		transform.rotate.z = 0.3f;
		transform.scale = Vector3{ 0.3f,0.3f,0.3f } *finalScale_;
		finalSprite_->Draw(transform, uv, { 1.0f,1.0f,1.0f,firstAlpha_ }, yokeroTextureHandle_);
	}
}

void WaveManager::DrawTutorial() {
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform transform = { { 0.3f,0.3f,0.1f },{0,0,0},{640,250,0} };
	Transform transform2 = { { 0.3f,0.2f,0.1f },{0,0,0},{640,90,0} };

	if (waveNum_ == 0) {
		tutorialSprite1_->Draw(transform2, uv, { 1.0f,1.0f,1.0f,1.0f }, tutorialTextureHandles_[3]);

	}
	if (waveNum_ == 2) {
		tutorialSprite1_->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, tutorialTextureHandles_[waveNum_]);
	}
	else {
		tutorialSprite_->Draw(transform, uv, { 1.0f,1.0f,1.0f,1.0f }, tutorialTextureHandles_[waveNum_]);
	}
}

void WaveManager::Draw3D(const ViewProjection& viewProjection) {
	if (cortion_.empty()) {
		return;
	}
	Transform uv = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	Transform transform = { { 0.3f,0.3f,0.1f },{0,0,0},{640,250,0} };
	size_t index = 0;
	Matrix4x4 toCameraRotate = Inverse(viewProjection.matView);
	toCameraRotate.m[3][0] = 0;
	toCameraRotate.m[3][1] = 0;
	toCameraRotate.m[3][2] = 0;
	for (Vector3& position : cortion_) {
		worldTransformCortions_[index].scale_ = { 3.0f,3.0f,1.0f };
		worldTransformCortions_[index].translation_ = position;
		if (std::abs(position.x) > 52.0f || (position.y > 52.0f || position.y < 0)) {
			worldTransformCortions_[index].translation_.x = std::clamp(position.x, -52.0f, 52.0f);
			worldTransformCortions_[index].translation_.y = std::clamp(position.y, 0.0f, 52.0f);
			Matrix4x4 rotate = DirectionToDirection({ 1.0f,0,0.0f }, Normalise(position - worldTransformCortions_[index].translation_));
			if (Normalise(position - worldTransformCortions_[index].translation_).x < 0 && Normalise(position - worldTransformCortions_[index].translation_).y == 0) {
				rotate = Multiply(rotate, MakeRotateXMatrix(3.141592f));
			}
			worldTransformArrows_[index].rotation_ = TransformNormal(worldTransformArrows_[index].rotation_, rotate);
			worldTransformArrows_[index].scale_ = { 3.0f,3.0f,1.0f };
			worldTransformArrows_[index].translation_ = worldTransformCortions_[index].translation_;
			worldTransformArrows_[index].translation_.z += 0.1f;
			worldTransformArrows_[index].matWorld_ = Multiply(Multiply(Multiply(MakeScaleMatrix(worldTransformCortions_[index].scale_), MakeTranslateMatrix({ 2.0f,0.0f,0.0f })), rotate), MakeTranslateMatrix(worldTransformCortions_[index].translation_));
			worldTransformArrows_[index].TransferMatrix();
			//worldTransformArrows_[index].UpdateMatrix();
			cortionPlane_->Draw(worldTransformArrows_[index], viewProjection, { 1.0f,1.0f,1.0f,1.0f }, arrowTextureHandle_);
		}
		worldTransformCortions_[index].translation_.z -= 1.0f;
		worldTransformCortions_[index].matWorld_ = Multiply(Multiply(MakeScaleMatrix(worldTransformCortions_[index].scale_), toCameraRotate), MakeTranslateMatrix(worldTransformCortions_[index].translation_));
		worldTransformCortions_[index].TransferMatrix();
		cortionPlane_->Draw(worldTransformCortions_[index], viewProjection, { 1.0f,1.0f,1.0f,1.0f }, cortionTextureHandle_);
		index++;
	}
	cortion_.clear();
}

void WaveManager::ChangeNumAnimation() {
	float t = float(changeAnimationCount_) / float(changeAnimationLength_);
	s0 = Lerp(t, centerPos0_, downPos0_);
	s1 = Lerp(t, topPos0_, centerPos0_);
	if (isChange10) {
		s2 = Lerp(t, centerPos1_, downPos1_);
		s3 = Lerp(t, topPos1_, centerPos1_);
	}
	if (changeAnimationCount_ == changeAnimationLength_) {
		s0 = centerPos0_;
		s1 = topPos0_;
		num0 = num1;
		if (isChange10) {
			s2 = centerPos1_;
			s3 = topPos1_;
			num2 = num3;
		}
		isChangeNum_ = false;
		isChange10 = false;
	}
	changeAnimationCount_++;
}
