#include "PlayerAimBallEnemy.h"
#include "game/Player.h"

PlayerAimBallEnemy::PlayerAimBallEnemy()
{
}

PlayerAimBallEnemy::~PlayerAimBallEnemy()
{
}

void PlayerAimBallEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model)
{
	/*sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();*/

	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;
	worldTransform_.Initialize();
	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;
	velocity_.x = std::clamp(velocity_.x, -1.0f, 1.0f);
	velocity_.y = std::clamp(velocity_.y, -1.0f, 1.0f);
	velocity_.z = std::clamp(velocity_.z, -1.0f, 1.0f);
	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);

	behavior_ = Behavior::kstandBy;
	isAlive_ = true;
	ishit_ = false;
	model_ = model;
	SetType(kAimBound);
	model_->setIsLighting(false);
	count = 0;
}

void PlayerAimBallEnemy::Update()
{
	prePos_ = worldTransform_.translation_;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(worldTransform_.rotation_);
	obb_.size = { worldTransform_.scale_.x / 2.0f  ,worldTransform_.scale_.y / 2.0f  ,worldTransform_.scale_.z / 2.0f };
	obb_.center = worldTransform_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);
	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();
		switch (behavior_) {
		case Behavior::kstandBy:
		default:
			BehaviorStandbyInitialize();
			break;
		case Behavior::kAtack:
			BehaviorAtackInitialize();
			break;
		case Behavior::kLeave:
			BehaviorLeaveInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}
	if (count > 2) {
		isAlive_ = false;
	}
	switch (behavior_) {
	case Behavior::kstandBy:
	default:
		BehaviorStandbyUpdate();
		break;
	case Behavior::kAtack:

		BehaviorAtackUpdate();
		
		break;
	case Behavior::kLeave:
		BehaviorLeaveUpdate();
		break;
	}
	
	worldTransform_.UpdateMatrix();
}


void PlayerAimBallEnemy::Draw(const ViewProjection& viewProjection)
{
	/*sphere_->setIsLighting(false);
	sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, 3, viewProjection);*/
	model_->Draw(worldTransform_, viewProjection);
}

void PlayerAimBallEnemy::isCollision(OBB pertner)
{
	if (behavior_ == Behavior::kAtack) {
		worldTransform_.translation_ = prePos_;
		behaviorRequest_ = Behavior::kLeave;
	}
}



void PlayerAimBallEnemy::BehaviorStandbyUpdate()
{


	if (BehaviorChangeCount >= 30) {
		standBycount++;
		worldTransform_.rotation_.z = standBycount / 5.0f;
		if (standBycount >= 30) {
			behaviorRequest_ = Behavior::kAtack;
		}
	}
	else {
		BehaviorChangeCount++;
	}
}

void PlayerAimBallEnemy::BehaviorAtackUpdate()
{
	if (BehaviorChangeCount >= 30) {
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	}
	else {
		BehaviorChangeCount++;
	}
}

void PlayerAimBallEnemy::BehaviorLeaveUpdate()
{
	float t = float(countUp_) / float(kLeaveCount_);
	float it = 1.0f - t;
	worldTransform_.translation_.x =it* startPos_.x + t * target_.x;
	worldTransform_.translation_.y = it * startPos_.y + t * target_.y;
	t *= 2.0f;
	t -= 1.0f;
	t *= t;
	t *= -1;
	t += 1.0f;
	float easedT = 1.0f - std::powf(1.0f - t, 2.0f);
	worldTransform_.translation_.z = easedT * 10.0f;
	standBycount--;
	worldTransform_.rotation_.z = standBycount / 5.0f;
	if (countUp_ >= kLeaveCount_) {
		behaviorRequest_ = Behavior::kstandBy;
	}
	countUp_++;
}

void PlayerAimBallEnemy::BehaviorStandbyInitialize()
{
	
	standBycount = 0;
	BehaviorChangeCount = 0;
}

void PlayerAimBallEnemy::BehaviorLeaveInitialize()
{
	count++;
	Vector3 target = player_->GetWorldTransform().translation_;
	target.y = 50.0f;
	velocity_ = Subtract(target, worldTransform_.translation_);
	velocity_ = Normalise(velocity_);
	float leaveSpeed;
	leaveSpeed = MoveSpeed_ * 2.0f / 3.0f;
	velocity_ = Multiply(leaveSpeed, velocity_);
	BehaviorChangeCount = 0;
	target_ = target;
	startPos_ = worldTransform_.translation_;
	countUp_ = 0;
}

void PlayerAimBallEnemy::BehaviorAtackInitialize()
{
	BehaviorChangeCount = 0;
	velocity_ = Subtract(player_->GetWorldTransform().translation_, worldTransform_.translation_);
	velocity_ = Normalise(velocity_);
	velocity_ = Multiply(MoveSpeed_, velocity_);
}
