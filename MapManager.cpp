#include "MapManager.h"
#include "Globalvariables.h"
#include "Easing.h"
#include "GameController.h"
#include "Audio.h"
//#include "Input.h"
uint32_t MapManager::kBlockFloatForce = 10;//4;
uint32_t MapManager::kBlocckFloatAnimationLength = 60;
uint32_t MapManager::kBlocckFloatAnimationDelay = 2;
uint32_t MapManager::kReverseFloatAnimationDelay = 2;
MapManager* MapManager::GetInstance()
{
	static MapManager instance;
	return &instance;
}

void MapManager::Initialize() {
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "Map";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "FloatForce", 4);
	globalVariables->AddItem(groupName, "FloatAnimationLength", 60);
	globalVariables->AddItem(groupName, "FloatAnimationDelay", 2);
	globalVariables->AddItem(groupName, "ReverseAnimationDelay", 2);
	globalVariables->AddItem(groupName, "ReverseCoolTime", 60);


	MapRead();
	modelBlock_.reset(Model::CreateModelFromObj("Resource/block", "block.obj"));
	modelArie_.reset(Model::CreateModelFromObj("Resource/cube", "cube.obj"));
	cameraShake_ = nullptr;
}

void MapManager::MapRead()
{
	char readString[256];
	char* ptr;
	char* context = nullptr;
	FILE* fp = NULL;
	fopen_s(&fp, "Resource/Map/mapSample.csv", "rt");
	if (fp == NULL) {
		return;
	}
	mapWidth_ = 1;
	mapHeight_ = 1;
	uint32_t x = 0, y = 0;
	while (fgets(readString, sizeof(readString) / sizeof(char), fp) != NULL && y < kMapHeight) {
		ptr = strtok_s(readString, ",", &context);
		map[y][0] = MapState(atoi(ptr));
		
		x = 1;
		while (ptr != NULL && x < kMapWidth) {
			ptr = strtok_s(NULL, ",", &context);
			if (ptr != NULL) {
				map[y][x] = MapState(atoi(ptr));
				
			}
			if (map[y][x] == MapState::Wall) {
				//mapWidth_ = x+1;
				if (mapWidth_ < x) {
					mapWidth_ = x + 1;
				}
			}
			x++;
			
		}
		y++;
		/*if (mapWidth_ < x) {
			mapWidth_ = x;
		}*/
		mapHeight_ = y;
	}
	fclose(fp);	
	MapBuild();
}

void MapManager::MapBuild() {
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{0,0,0});
	mapObject_.clear();
	//mapObject_.shrink_to_fit();
	floor_.clear();
	//floor_.shrink_to_fit();
	wall_.clear();
	//wall_.shrink_to_fit();
	ceiling_.clear();
	uint32_t floorID = 0;
	for (uint32_t y = 0; y < mapHeight_;y++) {
		for (uint32_t x = 0; x < mapWidth_;x++) {
			if (map[y][x] != MapState::None) {
				/*WorldTransform worldTransform;
				worldTransform.Initialize();
				worldTransform.scale_ = {2.0f,2.0f,2.0f};
				worldTransform.translation_.x = float(int(x) - int(mapWidth_/2))*2.0f*worldTransform.scale_.x;
				worldTransform.translation_.y = (float( mapHeight_) - float(y+1))*2.0f * worldTransform.scale_.y;
				//worldTransform.UpdateMatrix();
				OBB obb;
				obb.size = { object->worldTransform.scale_.x,object->worldTransform.scale_.y,object->worldTransform.scale_.z };
				GetOrientations(rotateMatrix, obb.orientation);
				obb.center = object->worldTransform.translation_;*/
				std::shared_ptr<Map> object;
				object.reset( new Map);
				object->mapState = map[y][x];
				object->worldTransform.Initialize();
				object->worldTransform.scale_ = { 2.0f,2.0f,2.0f };
				object->worldTransform.translation_.x = float(int(x) - int(mapWidth_ / 2)) * 2.0f * object->worldTransform.scale_.x;
				object->worldTransform.translation_.y = (float(mapHeight_) - float(y + 1)) * 2.0f * object->worldTransform.scale_.y;
				object->worldTransform.UpdateMatrix();
				object->obb.size = { object->worldTransform.scale_.x,object->worldTransform.scale_.y,object->worldTransform.scale_.z };
				GetOrientations(rotateMatrix, object->obb.orientation);
				object->obb.center = object->worldTransform.translation_;
				object->moveDirection_ = 1.0f;
				mapObject_.push_back((object));
				if (map[y][x] == MapState::Block) {
					object->id = floorID++;
					floor_.push_back((object));
				}
				else if (map[y][x] == MapState::Wall) {
					wall_.push_back((object));
				}
				else if (map[y][x] == MapState::Ceiling) {
					ceiling_.push_back((object));
				}
			}
		}
	}
}

void MapManager::WaveRead(uint32_t wave) {
	char readString[256];
	int wavestate[32] = {0};
	char* ptr;
	char* context = nullptr;
	FILE* fp = NULL;
	fopen_s(&fp, "Resource/Map/waveMap.csv", "rt");
	if (fp == NULL) {
		return;
	}
	uint32_t column=0;
	uint32_t x = 0, y = 0;
	while (fgets(readString, sizeof(readString) / sizeof(char), fp) != NULL && y < kMapHeight) {
		if (column != wave) {
			column++;
			continue;
		}
		ptr = strtok_s(readString, ",", &context);
		wavestate[x] = atoi(ptr);
		x = 1;
		while (ptr != NULL && x < 32) {
			ptr = strtok_s(NULL, ",", &context);
			if (ptr != NULL) {
				wavestate[x] = atoi(ptr);
			}
			x++;
		}
		break;
	}
	fclose(fp);
	MapBuild();
	x = 0;
	for (std::shared_ptr<Map> object : floor_) {
		if (wavestate[x] == 1) {
			object->worldTransform.translation_.y = float(kBlockFloatForce);
			object->moveDirection_ = -1.0f;
		}
		x++;
	}
}

void MapManager::WaveReadTutorial(uint32_t wave) {
	char readString[256];
	int wavestate[32] = { 0 };
	char* ptr;
	char* context = nullptr;
	FILE* fp = NULL;
	fopen_s(&fp, "Resource/Map/waveTutorialMap.csv", "rt");
	if (fp == NULL) {
		return;
	}
	uint32_t column = 0;
	uint32_t x = 0, y = 0;
	while (fgets(readString, sizeof(readString) / sizeof(char), fp) != NULL && y < kMapHeight) {
		if (column != wave) {
			column++;
			continue;
		}
		ptr = strtok_s(readString, ",", &context);
		wavestate[x] = atoi(ptr);
		x = 1;
		while (ptr != NULL && x < 32) {
			ptr = strtok_s(NULL, ",", &context);
			if (ptr != NULL) {
				wavestate[x] = atoi(ptr);
			}
			x++;
		}
		break;
	}
	fclose(fp);
	MapBuild();
	x = 0;
	for (std::shared_ptr<Map> object : floor_) {
		if (wavestate[x] == 1) {
			object->worldTransform.translation_.y = float(kBlockFloatForce);
			object->moveDirection_ = -1.0f;
		}
		x++;
	}
}

void MapManager::Update() {
	ApplyGlobalVariables();
	if ((*mapObject_.begin()) == (*wall_.begin())) {
		for (std::shared_ptr<Map> object : floor_) {
			object->Update();
		}
		if (GameController::GetInstance()->Reverse() && reverseCoolTime_ <= 0) {
			Audio::GetInstance()->SoundPlayWave( Audio::GetInstance()->handle_[Reverce], Audio::GetInstance()->SoundVolume[Reverce]);

			for (std::shared_ptr<Map> object : floor_) {
				object->Reverse();
				object->delay_ = kReverseFloatAnimationDelay;
				cameraShake_();
			}
			reverseCoolTime_ = kReverseCoolTime_;
		}
		for (std::shared_ptr<Map> object : wall_) {
			//object->Update();
		}
	}
	reverseCoolTime_--;
}

void MapManager::Draw(const ViewProjection& viewProjection) {
	/*for (Map& object : mapObject_) {
		modelBlock_->Draw(object.worldTransform,viewProjection);
	}*/
	for (std::shared_ptr<Map> object : floor_) {
		modelBlock_->Draw(object->worldTransform, viewProjection);
	}
	for (std::shared_ptr<Map> object : wall_) {
		modelArie_->Draw(object->worldTransform,viewProjection);
	}
	for (std::shared_ptr<Map> object : ceiling_) {
		modelArie_->Draw(object->worldTransform, viewProjection);
	}
}

int MapManager::GetCenterHeight() {
	for (std::shared_ptr<Map> object : floor_) {
		if (object->id == 13) {
			if (object->worldTransform.translation_.y > 1.0f) {
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

void MapManager::Map::Update() {
	isFrameCollision_ = false;
	if (moveFlag_ && !isTouch_) {
		//移動開始
		if (isCollision_) {
			Audio::GetInstance()->SoundPlayWave(Audio::GetInstance()->handle_[Block], Audio::GetInstance()->SoundVolume[Block]);
			Reverse();
			delay_ = kBlocckFloatAnimationDelay;
		}
		else {
			/*moveFlag_ = false;
			from.translation_ = worldTransform.translation_;
			to.translation_ = worldTransform.translation_;
			to.translation_.y -= float(kBlockFloatForce);
			moveAnimationLength_ = kBlocckFloatAnimationLength;
			countUp_ = 0;
			isMove_ = true;
			isCollision_ = false;*/
			//isMove_ = true;
			isCollision_ = false;
		}
		moveFlag_ = false;
	}
	//isCollision_ = false;
	isTouch_ = false;
	if (isMove_) {
		
		Move();
	}
	obb.center = worldTransform.translation_;
	worldTransform.translation_ = obb.center;
	worldTransform.UpdateMatrix();
}

void MapManager::Map::Reverse() {
	moveFlag_ = false;
	from.translation_ = worldTransform.translation_;
	to.translation_ = worldTransform.translation_;
	to.translation_.y = float(kBlockFloatForce) * (((moveDirection_) + 1.0f) / 2.0f) + 0.0f;
	moveAnimationLength_ = kBlocckFloatAnimationLength;
	countUp_ = 0;
	isMove_ = true;
	isCollision_ = false;
	moveDirection_ *= -1.0f;
}

void MapManager::Map::Move() {
	float t = float(countUp_) / float(moveAnimationLength_);
	float easedT = EaseIn(t,delay_);
	worldTransform.translation_ = Lerp(easedT,from.translation_,to.translation_);
	if (countUp_ >= moveAnimationLength_) {
		countUp_ = 0;
		isMove_ = false;
	}
	countUp_++;
}

void MapManager::Map::OnCollision() {
	isCollision_ = true;
	//isFrameCollision_ = true;
}

void MapManager::Map::Touch() {
	if (!isMove_) {
		isTouch_ = true;
		moveFlag_ = true;
	}
}

void MapManager::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "Map";
	kBlockFloatForce = globalVariables->GetIntValue(groupName, "FloatForce");
	kBlocckFloatAnimationLength = globalVariables->GetIntValue(groupName, "FloatAnimationLength");
	kBlocckFloatAnimationDelay = globalVariables->GetIntValue(groupName, "FloatAnimationDelay");
	kReverseFloatAnimationDelay = globalVariables->GetIntValue(groupName, "ReverseAnimationDelay");
	kReverseCoolTime_ = globalVariables->GetIntValue(groupName, "ReverseCoolTime");
}
