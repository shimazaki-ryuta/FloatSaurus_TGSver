#pragma once
#include <stdint.h>
#include "WorldTransform.h"
#include "model.h"
#include <memory>
#include <vector>
#include "Mymath.h"
#include "Input.h"
class MapManager
{
public:
	enum class MapState {
		None,
		Block,
		Wall,
		Ceiling,
	};
	struct Map {
		MapState mapState;
		OBB obb;
		WorldTransform worldTransform;
		bool isCollision_=false;
		bool isTouch_ = false;
		bool moveFlag_ = false;
		bool isMove_ = false;
		uint32_t countUp_;
		WorldTransform from;
		WorldTransform to;
		uint32_t moveAnimationLength_;
		void Update();
		void Move();
		void OnCollision() ;
		void Touch();
		float moveDirection_;
		void Reverse();
		uint32_t delay_;
		bool isFrameCollision_ = false;
		uint32_t id;
	};

	static MapManager* GetInstance();
	void Initialize();
	
	void Update();
	//CSVからマップ情報を格納する
	void MapRead();
	//マップ情報からWorldtransformをさくせいしてリストする
	void MapBuild();

	//指定ウェーブの床の位置を読み取って移動させる
	void WaveRead(uint32_t wave);

	void WaveReadTutorial(uint32_t wave);


	void Draw(const ViewProjection&);

	std::vector<std::shared_ptr<Map>>& GetMapObject() { return mapObject_; };
	std::vector<std::shared_ptr<Map>>& GetFloor() { return floor_; };
	std::vector<std::shared_ptr<Map>>& GetWall() { return wall_; };

	void ApplyGlobalVariables();

	void SetJoyState(XINPUT_STATE* joystate) { joyState_ = joystate; };
	void SetPreJoyState(XINPUT_STATE* joystate) { preJoyState_ = joystate; };
	void SetShakeCamera(std::function<void()> cameraShake) { cameraShake_ = cameraShake; };

	bool IsReversAble() { return reverseCoolTime_ <= 0; };

	//マップの読み込み最大幅
	static const uint32_t kMapWidth = 30;
	//マップの読み込み最大高さ
	static const uint32_t kMapHeight = 20;

	//ブロックの浮き上がりの高さ
	static uint32_t kBlockFloatForce;
	static uint32_t kBlocckFloatAnimationLength;
	static uint32_t kBlocckFloatAnimationDelay;
	static uint32_t kReverseFloatAnimationDelay;
	void Clear() {
		mapObject_.clear();
		floor_.clear();
		wall_.clear();
	};
	//中心のブロックが上1か下0か
	int GetCenterHeight();
private:
	MapManager() = default;
	~MapManager() = default;
	MapManager(const MapManager&) = delete;
	MapManager& operator=(const MapManager&) = delete;

	XINPUT_STATE* joyState_;
	XINPUT_STATE* preJoyState_;

	std::unique_ptr<Model> modelBlock_;
	std::unique_ptr<Model> modelArie_;

	std::vector <std::shared_ptr<Map>> mapObject_;

	std::vector<std::shared_ptr<Map>> floor_;
	std::vector<std::shared_ptr<Map>> wall_;
	std::vector<std::shared_ptr<Map>> ceiling_;//天井


	MapState map[kMapHeight][kMapWidth];
	uint32_t mapWidth_;
	uint32_t mapHeight_;
	std::function<void()> cameraShake_;
	int32_t reverseCoolTime_;
	int32_t kReverseCoolTime_;
};

