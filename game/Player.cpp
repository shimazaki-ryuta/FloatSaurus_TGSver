#include "Player.h"
#include "Input.h"
#include <algorithm>
#include <math.h>
#include "Mymath.h"
#include "Globalvariables.h"
#include "GameController.h"
#include "RandomEngine.h"
#include"Audio.h"
void Player::Initialize(){

	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "Player";
	//jumpAccerelation_ = globalVariables->GetVector3Value(groupName, "jump");
	//moveSpeed_ = globalVariables->GetFloatValue(groupName, "speed");
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "gravity", gravity_);
	globalVariables->AddItem(groupName, "jump", jumpAccerelation_);
	globalVariables->AddItem(groupName, "jumpCouninueStrength", jumpContinueAccerelation_);
	globalVariables->AddItem(groupName, "jumpCoolTime", kJumpCoolTime_);

	globalVariables->AddItem(groupName, "speed", moveSpeed_);
	globalVariables->AddItem(groupName, "jumpDampingX", jumpDampingX_);
	globalVariables->AddItem(groupName, "jumpReceptionLength",jumpReceptionLength_);

	const char* groupName2 = "PlayerModel";
	globalVariables->AddItem(groupName2, "antena", antenaOffset_);
	globalVariables->AddItem(groupName2, "code", codeOffset_);
	globalVariables->AddItem(groupName2, "head", headOffset_);
	globalVariables->AddItem(groupName2, "body", bodyOffset_);
	globalVariables->AddItem(groupName2, "back", backOffset_);
	globalVariables->AddItem(groupName2, "left", leftOffset_);
	globalVariables->AddItem(groupName2, "right", rightOffset_);
	globalVariables->AddItem(groupName2, "fire", fireOffset_);

	globalVariables->AddItem(groupName2, "legRotate", legRotate_);
	globalVariables->AddItem(groupName2, "bodyIdle", floatBodyIdle_);
	globalVariables->AddItem(groupName2, "bodyMove", floatBodyMove_);
	globalVariables->AddItem(groupName2, "charactorScale", charctorScale_);

	globalVariables->AddItem(groupName2, "antenaTarget", antenaTargetOffset_);

	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.0f,1.0f,1.0f};
	worldTransform_.translation_.x = 0.0f;
	worldTransform_.translation_.y = 2.0f;

	worldTransformOBB_.Initialize();
	worldTransformOBB_.parent_ = &worldTransform_;
	worldTransformOBB_.scale_ = {1.0f,2.0f,1.0f};
	worldTransformOBB_.UpdateMatrix();

	velocity_ = {0,0,0};
	acceleration_ = {0,0,0};
	//gravity_ = {0,-0.005f,0};
	direction_ = 1.0f;
	model_.reset(Model::CreateModelFromObj("Resource/cube", "cube.obj"));
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0,0,0 });
	obb_.size = { worldTransformOBB_.scale_.x ,worldTransformOBB_.scale_.y,worldTransformOBB_.scale_.z };
	obb_.center = worldTransformOBB_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);

	obbFloatTrigger_.size = { worldTransformOBB_.scale_.x ,worldTransform_.scale_.y*2,worldTransformOBB_.scale_.z };
	obbFloatTrigger_.center = worldTransform_.translation_;
	obbFloatTrigger_.center.y -= 2.5;
	GetOrientations(rotateMatrix, obbFloatTrigger_.orientation);
	jumpAble_ = true;
	isJumpReception_ = false;

	//model
	antena_.reset(Model::CreateModelFromObj("Resource/player/antena", "PlayerBall.obj"));
	code_.reset(Model::CreateModelFromObj("Resource/player/antena", "PlayerAntena.obj"));
	body_.reset(Model::CreateModelFromObj("Resource/player/body", "PlayerBody.obj"));
	back_.reset(Model::CreateModelFromObj("Resource/player/back", "PlayerBack.obj"));
	back2_.reset(Model::CreateModelFromObj("Resource/player/back2", "PlayerBack.obj"));
	head_.reset(Model::CreateModelFromObj("Resource/player/head", "PlayerHead.obj"));
	leg_.reset(Model::CreateModelFromObj("Resource/player/leg", "PlayerLeg.obj"));
	fire_.reset(Model::CreateModelFromObj("Resource/player/fire", "Fire.obj"));

	
	worldTransformModels_.clear();
	DethAnimationParamator paramator;
	worldTransformModel_.Initialize();
	worldTransformModel_.scale_ = charctorScale_;
	worldTransformModel_.parent_ = &worldTransform_;
	paramator = { &worldTransformModel_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);

	worldTransformAntena_.Initialize();
	paramator = { &worldTransformAntena_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator); 
	worldTransformbody_.Initialize();
	worldTransformbody_.parent_ = &worldTransformModel_;
	paramator = { &worldTransformbody_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);
	worldTransformback_.Initialize();
	worldTransformback_.parent_ = &worldTransformbody_;
	worldTransformback_.scale_ = { 2.0f,2.0f,1.5f };
	paramator = { &worldTransformback_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);
	worldTransformCode_.Initialize();
	paramator = { &worldTransformCode_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);
	worldTransformHead_.Initialize();
	worldTransformHead_.parent_ = &worldTransformbody_;
	paramator = { &worldTransformHead_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);
	worldTransformLeftLeg_.Initialize();
	worldTransformLeftLeg_.parent_ = &worldTransformModel_;
	paramator = { &worldTransformLeftLeg_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);
	worldTransformRightLeg_.Initialize();
	worldTransformRightLeg_.parent_ = &worldTransformModel_;
	paramator = { &worldTransformRightLeg_ ,{0,0,0}, {0,0,0} };
	worldTransformModels_.push_back(paramator);

	worldTransformFire_.Initialize();
	worldTransformFire_.parent_ = &worldTransformback_;

	worldTransformAntenaTarget_.Initialize();
	worldTransformAntenaTarget_.parent_ = &worldTransformHead_;

	Vector3 antenaPos = vectorTransform(antenaOffset_, worldTransformHead_.matWorld_);
	worldTransformAntena_.translation_ = Lerp(1.0f, worldTransformAntena_.translation_, antenaPos);
	worldTransformAntena_.UpdateMatrix();
	worldTransformCode_.translation_ = Lerp(0.5f, worldTransformAntena_.translation_, worldTransformHead_.GetWorldPos());
	Matrix4x4 rotateCode = DirectionToDirection({ 0,0,1.0f }, Normalise(worldTransformHead_.GetWorldPos() - worldTransformAntena_.translation_));
	worldTransformCode_.matWorld_ = Multiply(Multiply(MakeScaleMatrix(worldTransformCode_.scale_), rotateCode), MakeTranslateMatrix(worldTransformCode_.translation_));
	worldTransformCode_.TransferMatrix();
	isMove_ = false;
	theta_ = 0;
	isDead_ = false;
}

void Player::Reset(int height) {
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.translation_.x = 0.0f;
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "Map";
	worldTransform_.translation_.y = 2.0f + float(height) * globalVariables->GetIntValue(groupName, "FloatForce");
	worldTransform_.UpdateMatrix();
	velocity_ = { 0,0,0 };
	acceleration_ = { 0,0,0 };
	//gravity_ = {0,-0.005f,0};
	direction_ = 1.0f;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0,0,0 });
	obb_.size = { worldTransformOBB_.scale_.x ,worldTransformOBB_.scale_.y,worldTransformOBB_.scale_.z };
	obb_.center = worldTransform_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);

	obbFloatTrigger_.size = { worldTransformOBB_.scale_.x ,worldTransform_.scale_.y * 2,worldTransformOBB_.scale_.z };
	obbFloatTrigger_.center = worldTransform_.translation_;
	obbFloatTrigger_.center.y -= 2.5;
	GetOrientations(rotateMatrix, obbFloatTrigger_.orientation);
	jumpAble_ = true;
	isJumpReception_ = false;

	worldTransformModel_.parent_ = &worldTransform_;
	worldTransformModel_.translation_ = { 0,0,0 };
	worldTransformModel_.rotation_ = { 0,0,0 };
	worldTransformModel_.UpdateMatrix();
	worldTransformbody_.parent_ = &worldTransformModel_;
	worldTransformbody_.scale_ = {1.0f,1.0f,1.0f};
	worldTransformbody_.rotation_ = {0,0,0};
	worldTransformbody_.translation_ = bodyOffset_;
	worldTransformbody_.UpdateMatrix();
	worldTransformback_.parent_ = &worldTransformbody_;
	worldTransformback_.scale_ = { 2.0f,2.0f,1.5f };
	worldTransformback_.rotation_ = { 0,0,0 };
	worldTransformHead_.parent_ = &worldTransformbody_;
	worldTransformHead_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransformHead_.rotation_ = { 0,0,0 };
	worldTransformHead_.translation_ = headOffset_;
	worldTransformHead_.UpdateMatrix();
	worldTransformLeftLeg_.parent_ = &worldTransformModel_;
	worldTransformLeftLeg_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransformLeftLeg_.rotation_ = { 0,0,0 };
	worldTransformRightLeg_.parent_ = &worldTransformModel_;
	worldTransformRightLeg_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransformRightLeg_.rotation_ = { 0,0,0 };

	worldTransformAntenaTarget_.Initialize();
	worldTransformAntenaTarget_.parent_ = &worldTransformHead_;

	Vector3 antenaPos = vectorTransform(antenaOffset_, worldTransformHead_.matWorld_);
	worldTransformAntena_.translation_ = Lerp(1.0f, worldTransformAntena_.translation_, antenaPos);
	worldTransformAntena_.UpdateMatrix();
	worldTransformCode_.translation_ = Lerp(0.5f, worldTransformAntena_.translation_, worldTransformHead_.GetWorldPos());
	Matrix4x4 rotateCode = DirectionToDirection({ 0,0,1.0f }, Normalise(worldTransformHead_.GetWorldPos() - worldTransformAntena_.translation_));
	worldTransformCode_.matWorld_ = Multiply(Multiply(MakeScaleMatrix(worldTransformCode_.scale_), rotateCode), MakeTranslateMatrix(worldTransformCode_.translation_));
	worldTransformCode_.TransferMatrix();
	isMove_ = false;
	theta_ = 0;

	isDead_ = false;
}

void Player::Update() {
		ApplyGlobalVariables();
		isRecovJump_ = false;
	if (!isDead_) {
		prePosition_ = worldTransform_.translation_;
		if (GameController::GetInstance()->Jump() && jumpAble_ && jumpCoolTime_ <= 0) {
			Audio::GetInstance()->SoundPlayWave( Audio::GetInstance()->handle_[Jump], Audio::GetInstance()->SoundVolume[Jump]);

			velocity_.y = 0.0f;
			velocity_.y = jumpAccerelation_.y;
			jumpAble_ = false;
			isJumpReception_ = true;
			jumpReceptionRest_ = jumpReceptionLength_;
			jumpCoolTime_ = kJumpCoolTime_;
		}
		if ((GameController::GetInstance()->ContinueJump()) && isJumpReception_) {
			//velocity_.y *= 0.6f;
			//isJumpReception_ = false;
			velocity_.y = jumpAccerelation_.y * jumpContinueAccerelation_.y;
			//*(float(jumpReceptionRest_) / float(jumpReceptionLength_))
			jumpReceptionRest_--;
			if (jumpReceptionRest_ < 0) {
				isJumpReception_ = false;
			}
		}
		else {
			isJumpReception_ = false;
			jumpReceptionRest_=0;
			if (jumpReceptionRest_ < 0) {
				isJumpReception_ = false;
			}
			//velocity_.y = 0.0f;
			//isJumpReception_ = false;
		}

		float kSpeed = 0.1f;
		acceleration_ = gravity_;
		velocity_ = velocity_ + acceleration_;
		//velocity_.x = direction_ * kSpeed;
		if (isCollisionFloor_) {
			velocity_.x *= jumpDampingX_;
			if (std::abs(velocity_.x) <= 0.01f) {
				//velocity_.x = 0;
			}
		}
		isMove_ = false;
		if (GameController::GetInstance()->Left()) {
			//velocity_.y = 0.0f;
			//acceleration_ = { 0 ,0.06f,0 };
			velocity_.x = -1.0f * moveSpeed_;
			direction_ = -1.0f;
			isMove_ = true;
		}
		if (GameController::GetInstance()->Right()) {
			//velocity_.y = 0.0f;
			//acceleration_ = { 0 ,0.06f,0 };
			velocity_.x = 1.0f * moveSpeed_;
			direction_ = 1.0f;
			isMove_ = true;
		}
		if (isMove_ && !isJumpReception_ && !isCollisionFloor_) {
			theta_ += legRotate_;
			worldTransformLeftLeg_.rotation_.x = std::sin(theta_);
			worldTransformRightLeg_.rotation_.x = -std::sin(theta_);
			floatAnimetion_ += floatBodyMove_;
		}
		else if (!isJumpReception_ && !isCollisionFloor_) {
			theta_ = 0;
			floatAnimetion_ += floatBodyIdle_;
			worldTransformLeftLeg_.rotation_.x = 0;
			worldTransformRightLeg_.rotation_.x = 0;
		}
		else {
			//theta_ = 0;
			floatAnimetion_ = 0;
			if (velocity_.y > 0.0f) {
				worldTransformLeftLeg_.rotation_.x = worldTransformLeftLeg_.rotation_.x + (0.3f - worldTransformLeftLeg_.rotation_.x) * 0.1f;
				worldTransformRightLeg_.rotation_.x = worldTransformRightLeg_.rotation_.x + (0.3f - worldTransformRightLeg_.rotation_.x) * 0.1f;
			}
			else {
				worldTransformLeftLeg_.rotation_.x = worldTransformLeftLeg_.rotation_.x + (-0.3f - worldTransformLeftLeg_.rotation_.x) * 0.1f;
				worldTransformRightLeg_.rotation_.x = worldTransformRightLeg_.rotation_.x + (-0.3f - worldTransformRightLeg_.rotation_.x) * 0.1f;
			}

		}
		//velocity_.y = std::clamp(velocity_.y, -0.8f, 0.8f);
		worldTransform_.translation_ = worldTransform_.translation_ + velocity_;
		worldTransform_.rotation_.y = 1.57f * direction_;
		worldTransform_.UpdateMatrix();
		worldTransformOBB_.UpdateMatrix();
		obb_.center = worldTransformOBB_.GetWorldPos();
		obbFloatTrigger_.center = worldTransformOBB_.GetWorldPos();
		obbFloatTrigger_.center.y -= 2.5;
		isCollision_ = true;
		isCollisionFloor_ = true;
		isCollisionWall_ = true;

		worldTransformModel_.scale_ = charctorScale_;
		worldTransformModel_.UpdateMatrix();

		worldTransformbody_.translation_ = bodyOffset_;
		worldTransformbody_.translation_.y += std::sin(floatAnimetion_) * 0.5f;
		worldTransformbody_.UpdateMatrix();
		worldTransformback_.translation_ = backOffset_;
		worldTransformback_.UpdateMatrix();
		worldTransformHead_.translation_ = headOffset_;
		worldTransformHead_.UpdateMatrix();
		//worldTransformLeftLeg_.rotation_.x = std::sin(theta_);
		worldTransformLeftLeg_.translation_ = leftOffset_;
		worldTransformLeftLeg_.UpdateMatrix();
		//worldTransformRightLeg_.rotation_.x = -std::sin(theta_);
		worldTransformRightLeg_.translation_ = rightOffset_;
		worldTransformRightLeg_.UpdateMatrix();
		worldTransformFire_.translation_ = fireOffset_;
		worldTransformFire_.UpdateMatrix();

		worldTransformAntenaTarget_.translation_ = antenaTargetOffset_;
		worldTransformAntenaTarget_.UpdateMatrix();

		Vector3 antenaPos = vectorTransform(antenaOffset_, worldTransformAntenaTarget_.matWorld_);
		worldTransformAntena_.translation_ = Lerp(0.1f, worldTransformAntena_.translation_, antenaPos);
		worldTransformAntena_.scale_ = { 0.7f,0.7f,0.7f };
		worldTransformAntena_.UpdateMatrix();
		worldTransformCode_.scale_ = { 0.5f,0.5f,0.7f };
		worldTransformCode_.translation_ = Lerp(0.5f, worldTransformAntena_.translation_, worldTransformAntenaTarget_.GetWorldPos());
		Matrix4x4 rotateCode = DirectionToDirection({ 0,0,1.0f }, Normalise(worldTransformAntenaTarget_.GetWorldPos() - worldTransformAntena_.translation_));
		worldTransformCode_.matWorld_ = Multiply(Multiply(MakeScaleMatrix(worldTransformCode_.scale_), rotateCode), MakeTranslateMatrix(worldTransformCode_.translation_));
		worldTransformCode_.TransferMatrix();
		}
	else {
		for (DethAnimationParamator& worldTransform : worldTransformModels_) {
			worldTransform.velocity = worldTransform.velocity + worldTransform.acceleration;
			worldTransform.worldTransform->translation_ = worldTransform.worldTransform->translation_ + worldTransform.velocity;
			worldTransform.worldTransform->UpdateMatrix();
		}
	}
	jumpCoolTime_--;
}

void Player::OnCollision(OBB& partner) {
	if (isCollision_) {
		if (std::abs(obb_.center.x - partner.center.x) < std::abs(obb_.center.y - partner.center.y)) {
			if ((obb_.center.y - prePosition_.y) < 0.0f) {
				//上に載ってるときの処理
				acceleration_ = { 0 ,0,0 };
				velocity_ = { 0,0,0 };
				worldTransform_.translation_.y = partner.center.y + 1.0f;
			}
			else {
				//デバッグ用
				//worldTransform_.translation_.y += 10.0f;
			}
		}
		else {
			//横方向から当たったときの処理
			worldTransform_.translation_.x = partner.center.x + Normalise(obb_.center - partner.center).x * 1.0f;
			direction_ *= -1.0f;
		}
		isCollision_ = false;
	}
	
}

bool Player::OnCollisionFloorVertical(OBB& partner) {
	if (isCollisionFloor_ || 1) {
		if (std::abs(obb_.center.x - partner.center.x) <= std::abs(obb_.center.y - partner.center.y)) {
			if (!jumpAble_) {
				isRecovJump_ = true;
			}
			jumpAble_ = true;
			//acceleration_ = { 0 ,0,0 };
			velocity_ = { 0,0,0 };
			if ((obb_.center.y - partner.center.y) / (std::sqrtf(std::powf(obb_.center.y - partner.center.y, 2))) * (obb_.size.y + partner.size.y) < 0) {
				worldTransform_.UpdateMatrix();
			}
			worldTransform_.translation_.y = partner.center.y + (obb_.center.y - partner.center.y) / (std::sqrtf(std::powf(obb_.center.y - partner.center.y, 2))) * (obb_.size.y + partner.size.y);
			isCollisionFloor_ = false;
			worldTransform_.UpdateMatrix();
			worldTransformOBB_.UpdateMatrix();
			obb_.center = worldTransformOBB_.GetWorldPos();
			return true;
		}
		else {
			//横方向から当たったときの処理
			//worldTransform_.translation_.x = partner.center.x + (obb_.center.x - partner.center.x)/(std::sqrtf(std::powf(obb_.center.x - partner.center.x,2))) * (obb_.size.x + partner.size.x);
			//direction_ *= -1.0f;
			//obb_.center = worldTransform_.translation_;
		}
		
	}
	return false;

}

void Player::OnCollisionFloorHorizon(OBB& partner) {
	if (isCollisionFloor_ || 1) {
		if (std::abs(obb_.center.x - partner.center.x) <= std::abs(obb_.center.y - partner.center.y)) {
			//jumpAble_ = true;
			//acceleration_ = { 0 ,0,0 };
			//velocity_ = { 0,0,0 };
			//worldTransform_.translation_.y = partner.center.y + (obb_.center.y - partner.center.y) / (std::sqrtf(std::powf(obb_.center.y - partner.center.y, 2))) * (obb_.size.y + partner.size.y);
			//obb_.center = worldTransform_.translation_;
			//isCollisionFloor_ = false;
			//worldTransform_.UpdateMatrix();
		}
		else {
			if (!jumpAble_) {
				isRecovJump_ = true;
			}
			jumpAble_ = true;
			//横方向から当たったときの処理
			worldTransform_.translation_.x = partner.center.x + (obb_.center.x - partner.center.x)/(std::sqrtf(std::powf(obb_.center.x - partner.center.x,2))) * (obb_.size.x + partner.size.x);
			//direction_ *= -1.0f;
			isCollisionFloor_ = false;
			worldTransform_.UpdateMatrix();
			worldTransformOBB_.UpdateMatrix();
			obb_.center = worldTransformOBB_.GetWorldPos();
		}

	}

}

void Player::OnCollisionWall(OBB& partner) {
	if (isCollisionWall_) {
	
		{
			//横方向から当たったときの処理
			worldTransform_.translation_.x = partner.center.x + (obb_.center.x - partner.center.x) / (std::sqrtf(std::powf(obb_.center.x - partner.center.x, 2))) * (obb_.size.x + partner.size.x);
			//direction_ *= -1.0f;
			if (isCollisionFloor_) {
				//velocity_.y = 0.0f;
				//acceleration_ = { 0 ,0.06f,0 };
			}
		}
		isCollisionWall_ = false;
		worldTransform_.UpdateMatrix();
	}

}

void Player::OnCollisionEnemy() {

}

void Player::Draw(const ViewProjection& viewProjection) {
	//model_->Draw(worldTransformOBB_,viewProjection);
	body_->Draw(worldTransformbody_,viewProjection);
	if (jumpAble_ && jumpCoolTime_ <= 0) {
		back2_->setIsLighting(false);
		back2_->Draw(worldTransformback_, viewProjection);
	}
	else {
		back_->Draw(worldTransformback_, viewProjection);
	}
	if (!isDead_ && isJumpReception_) {
		fire_->setIsLighting(false);
		fire_->Draw(worldTransformFire_,viewProjection);
	}
	head_->Draw(worldTransformHead_, viewProjection);
	leg_->Draw(worldTransformLeftLeg_, viewProjection);
	leg_->Draw(worldTransformRightLeg_, viewProjection);

	antena_->Draw(worldTransformAntena_,viewProjection);
	code_->Draw(worldTransformCode_, viewProjection);
}

void Player::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "Player";
	gravity_ = globalVariables->GetVector3Value(groupName, "gravity");
	jumpAccerelation_ = globalVariables->GetVector3Value(groupName, "jump");
	jumpContinueAccerelation_ = globalVariables->GetVector3Value(groupName, "jumpCouninueStrength");
	moveSpeed_ = globalVariables->GetFloatValue(groupName, "speed");
	jumpDampingX_ = globalVariables->GetFloatValue(groupName, "jumpDampingX");
	jumpReceptionLength_ = globalVariables->GetIntValue(groupName, "jumpReceptionLength");
	kJumpCoolTime_ = globalVariables->GetIntValue(groupName, "jumpCoolTime");

	const char* groupName2 = "PlayerModel";
	antenaOffset_ = globalVariables->GetVector3Value(groupName2, "antena");
	codeOffset_ = globalVariables->GetVector3Value(groupName2, "code");
	headOffset_ = globalVariables->GetVector3Value(groupName2, "head");
	bodyOffset_ = globalVariables->GetVector3Value(groupName2, "body");
	backOffset_ = globalVariables->GetVector3Value(groupName2, "back");
	leftOffset_ = globalVariables->GetVector3Value(groupName2, "left");
	rightOffset_ = globalVariables->GetVector3Value(groupName2, "right");
	fireOffset_ = globalVariables->GetVector3Value(groupName2, "fire");

	antenaTargetOffset_ = globalVariables->GetVector3Value(groupName2, "antenaTarget");

	legRotate_ = globalVariables->GetFloatValue(groupName2, "legRotate");
	floatBodyIdle_ = globalVariables->GetFloatValue(groupName2, "bodyIdle");
	floatBodyMove_ = globalVariables->GetFloatValue(groupName2, "bodyMove");
	charctorScale_ = globalVariables->GetVector3Value(groupName2, "charactorScale");
}

void Player::DethAnimation() {
	isDead_ = true;
	
	for (DethAnimationParamator& worldTransform : worldTransformModels_) {
		if (worldTransform.worldTransform->parent_) {
			worldTransform.worldTransform->rotation_ = worldTransform.worldTransform->rotation_ + worldTransform.worldTransform->parent_->rotation_;
		}
		worldTransform.worldTransform->translation_.x = worldTransform.worldTransform->matWorld_.m[3][0];
		worldTransform.worldTransform->translation_.y = worldTransform.worldTransform->matWorld_.m[3][1];
		worldTransform.worldTransform->translation_.z = worldTransform.worldTransform->matWorld_.m[3][2];
		worldTransform.worldTransform->scale_ = charctorScale_;
		worldTransform.worldTransform->parent_ = nullptr;
		worldTransform.velocity.x = RandomEngine::GetRandom(-1.0f,1.0f);
		worldTransform.velocity.y = RandomEngine::GetRandom(0.0f, 2.0f);
		worldTransform.velocity.z = RandomEngine::GetRandom(-1.0f, 1.0f);
		worldTransform.acceleration = {0.0f,-0.08f,0.0f};
	}
	//worldTransform_.translation_ = {2.0f,0.0f,0.0f};
}
