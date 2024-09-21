#pragma once
#include "MapManager.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"
#include "Mymath.h"
#include <memory>

class Player {
public:

	enum class Phase {
		Idle,
		Move,
		Break,
	};

	struct DethAnimationParamator
	{
		WorldTransform* worldTransform;
		Vector3 velocity;
		Vector3 acceleration;
	};

	void Initialize();

	
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void OnCollision(OBB& partner);
	bool OnCollisionFloorVertical(OBB& partner);
	void OnCollisionFloorHorizon(OBB& partner);
	void OnCollisionWall(OBB& partner);
	void OnCollisionEnemy();

	OBB& GetOBB() { return obb_; };
	OBB& GetFloatTrigger() { return obbFloatTrigger_; };
	//敵との当たり判定をとるかどうかのフラグを取得
	bool GetColliderFlag() { return isColliderFlag; };

	void SetJoyState(XINPUT_STATE* joystate) { joyState_ = joystate; };
	void SetPreJoyState(XINPUT_STATE* joystate) { preJoyState_ = joystate; };

	void ApplyGlobalVariables();
	WorldTransform& GetWorldTransform() { return worldTransform_; };
	//死んだときに位置をリセットする用
	void Reset(int height);
	//死亡時のアニメーション
	void DethAnimation();
	void SetLife(int32_t life) { life_ = life; };
	int32_t GetLife() { return life_; };
	bool IsJumpAble() { return (jumpAble_ && jumpCoolTime_ <= 0); };
	bool GetIsRecovJump() { return isRecovJump_; };
	WorldTransform& GetWorldTransformBack() { return worldTransformback_; };
	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

private:

	void ImGui();

private:
	Input* input_ = nullptr;
	XINPUT_STATE* joyState_;
	XINPUT_STATE* preJoyState_;
	std::unique_ptr<Model> model_;
	
	WorldTransform worldTransform_;
	WorldTransform worldTransformOBB_;


	OBB obb_;
	OBB obbFloatTrigger_;
	//敵との当たり判定をとるかどうかのフラグ
	bool isColliderFlag = true;

	float direction_;
	Vector3 velocity_;
	Vector3 acceleration_;
	//Vector3 gravity_;
	Vector3 prePosition_;

	bool isCollision_;
	bool isCollisionFloor_;
	bool isCollisionWall_;

	bool jumpAble_;

	//長押し入力が途切れるまで
	bool isJumpReception_;
	int32_t jumpReceptionRest_;

	//パラメータ群
	Vector3 gravity_;
	Vector3 jumpAccerelation_;
	Vector3 jumpContinueAccerelation_ = {0,0.05f,0};//長押しの加速度
	float moveSpeed_;
	float jumpDampingX_;
	int32_t jumpReceptionLength_;

	//
	std::unique_ptr<Model> antena_;
	std::unique_ptr<Model> code_;
	std::unique_ptr<Model> head_;
	std::unique_ptr<Model> body_;
	std::unique_ptr<Model> back_;
	std::unique_ptr<Model> back2_;
	std::unique_ptr<Model> leg_;
	std::unique_ptr<Model> fire_;

	
	WorldTransform worldTransformModel_;
	WorldTransform worldTransformAntena_;
	WorldTransform worldTransformCode_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformbody_;
	WorldTransform worldTransformback_;
	WorldTransform worldTransformLeftLeg_;
	WorldTransform worldTransformRightLeg_;
	WorldTransform worldTransformFire_;

	//追加
	WorldTransform worldTransformAntenaTarget_;

	Vector3 charctorScale_ = {1.0f,1.0f,1.0f};

	Vector3 antenaOffset_;
	
	Vector3 codeOffset_;
	Vector3 headOffset_;
	Vector3 bodyOffset_;
	Vector3 backOffset_;
	Vector3 leftOffset_;
	Vector3 rightOffset_;
	Vector3 fireOffset_;

	Vector3 antenaTargetOffset_;

	bool isMove_;
	float theta_;
	float floatAnimetion_;

	//アニメーション用変数
	float legRotate_;
	float floatBodyIdle_;
	float floatBodyMove_;

	bool isDead_;
	std::list<DethAnimationParamator> worldTransformModels_;
	int32_t jumpCoolTime_;
	int32_t kJumpCoolTime_;

	int32_t life_;


	//ジャンプ権が回復したときにtrue
	bool isRecovJump_;
};
