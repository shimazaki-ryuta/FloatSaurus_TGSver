#include "ResultPlayer.h"
#include "Globalvariables.h"
void ResultPlayer::Initialize() {

	//model
	antena_.reset(Model::CreateModelFromObj("Resource/player/antena", "PlayerBall.obj"));
	code_.reset(Model::CreateModelFromObj("Resource/player/antena", "PlayerAntena.obj"));
	body_.reset(Model::CreateModelFromObj("Resource/player/body", "PlayerBody.obj"));
	back_.reset(Model::CreateModelFromObj("Resource/player/back", "PlayerBack.obj"));
	back2_.reset(Model::CreateModelFromObj("Resource/player/back2", "PlayerBack.obj"));
	head_.reset(Model::CreateModelFromObj("Resource/player/head", "PlayerHead.obj"));
	leg_.reset(Model::CreateModelFromObj("Resource/player/leg", "PlayerLeg.obj"));
	//fire_.reset(Model::CreateModelFromObj("Resource/player/fire", "Fire.obj"));


	//worldTransformModels_.clear();
	worldTransformModel_.Initialize();
	worldTransformModel_.scale_ = charctorScale_;
	worldTransformModel_.rotation_.y = 3.14f * 0.5f;
	//worldTransformModel_.parent_ = &worldTransform_;
	
	worldTransformAntena_.Initialize();
	worldTransformbody_.Initialize();
	worldTransformbody_.parent_ = &worldTransformModel_;
	worldTransformback_.Initialize();
	//worldTransformback_.parent_ = &worldTransformbody_;
	worldTransformback_.scale_ = { 2.0f,2.0f,1.5f };
	worldTransformCode_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformHead_.parent_ = &worldTransformbody_;
	worldTransformLeftLeg_.Initialize();
	worldTransformLeftLeg_.parent_ = &worldTransformModel_;
	worldTransformRightLeg_.Initialize();
	worldTransformRightLeg_.parent_ = &worldTransformModel_;
	worldTransformAntenaTarget_.Initialize();
	worldTransformAntenaTarget_.parent_ = &worldTransformHead_;
	
	Vector3 antenaPos = vectorTransform(antenaOffset_, worldTransformAntenaTarget_.matWorld_);
	worldTransformAntena_.translation_ = Lerp(1.0f, worldTransformAntena_.translation_, antenaPos);
	worldTransformAntena_.UpdateMatrix();
	worldTransformCode_.translation_ = Lerp(0.5f, worldTransformAntena_.translation_, worldTransformAntenaTarget_.GetWorldPos());
	Matrix4x4 rotateCode = DirectionToDirection({ 0,0,1.0f }, Normalise(worldTransformAntenaTarget_.GetWorldPos() - worldTransformAntena_.translation_));
	worldTransformCode_.matWorld_ = Multiply(Multiply(MakeScaleMatrix(worldTransformCode_.scale_), rotateCode), MakeTranslateMatrix(worldTransformCode_.translation_));
	worldTransformCode_.TransferMatrix();
	
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "ResultPlayer";
	globalVariables->AddItem(groupName, "Translate", worldTransformModel_.translation_);
	globalVariables->AddItem(groupName, "Rotate", worldTransformModel_.rotation_);
	globalVariables->AddItem(groupName, "backRotate", worldTransformback_.rotation_);
	globalVariables->AddItem(groupName, "backTranslate", worldTransformback_.translation_);
	theta_ = 0;
}

void ResultPlayer::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();

	const char* groupName = "ResultPlayer";
	worldTransformModel_.translation_ = globalVariables->GetVector3Value(groupName, "Translate");
	worldTransformModel_.rotation_ = globalVariables->GetVector3Value(groupName, "Rotate");
	backOffset_ = globalVariables->GetVector3Value(groupName, "backTranslate");
	worldTransformback_.rotation_ = globalVariables->GetVector3Value(groupName, "backRotate");
	const char* groupName2 = "PlayerModel";
	antenaOffset_ = globalVariables->GetVector3Value(groupName2, "antena");
	codeOffset_ = globalVariables->GetVector3Value(groupName2, "code");
	headOffset_ = globalVariables->GetVector3Value(groupName2, "head");
	bodyOffset_ = globalVariables->GetVector3Value(groupName2, "body");
	//backOffset_ = globalVariables->GetVector3Value(groupName2, "back");
	leftOffset_ = globalVariables->GetVector3Value(groupName2, "left");
	rightOffset_ = globalVariables->GetVector3Value(groupName2, "right");
	charctorScale_ = globalVariables->GetVector3Value(groupName2, "charactorScale");
	antenaTargetOffset_ = globalVariables->GetVector3Value(groupName2, "antenaTarget");
}


void ResultPlayer::Update() {
	ApplyGlobalVariables();
	


	theta_ += 0.1f;
	worldTransformLeftLeg_.rotation_.x = -3.14f*0.25f + std::sin(theta_)*0.2f;
	worldTransformRightLeg_.rotation_.x = -3.14f * 0.25f - std::sin(theta_)*0.2f;
	worldTransformHead_.rotation_.z = std::sin(theta_) * 0.5f;
	worldTransformHead_.rotation_.y = std::sin(theta_) * 0.1f;

	worldTransformModel_.scale_ = charctorScale_;
	worldTransformModel_.UpdateMatrix();

	worldTransformbody_.translation_ = bodyOffset_;
	//worldTransformbody_.translation_.y += std::sin(floatAnimetion_) * 0.5f;
	worldTransformbody_.UpdateMatrix();
	worldTransformback_.scale_ = { 2.0f,2.0f,1.5f };
	worldTransformback_.scale_.x *= charctorScale_.x;
	worldTransformback_.scale_.y *= charctorScale_.y;
	worldTransformback_.scale_.z *= charctorScale_.z;
	worldTransformback_.translation_ = backOffset_;
	worldTransformback_.UpdateMatrix();
	worldTransformHead_.translation_ = headOffset_;
	worldTransformHead_.UpdateMatrix();
	//worldTransformLeftLeg_.rotation_.x = std::sin(theta_);
	worldTransformLeftLeg_.translation_ = leftOffset_;
	worldTransformLeftLeg_.translation_.y -= 0.1f;
	worldTransformLeftLeg_.UpdateMatrix();
	//worldTransformRightLeg_.rotation_.x = -std::sin(theta_);
	worldTransformRightLeg_.translation_ = rightOffset_;
	worldTransformRightLeg_.translation_.y -= 0.1f;
	worldTransformRightLeg_.UpdateMatrix();
		
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

void ResultPlayer::Draw(const ViewProjection& viewProjection) {
	//model_->Draw(worldTransformOBB_,viewProjection);
	body_->Draw(worldTransformbody_, viewProjection);
	back2_->Draw(worldTransformback_, viewProjection);
	
	head_->Draw(worldTransformHead_, viewProjection);
	leg_->Draw(worldTransformLeftLeg_, viewProjection);
	leg_->Draw(worldTransformRightLeg_, viewProjection);

	antena_->Draw(worldTransformAntena_, viewProjection);
	code_->Draw(worldTransformCode_, viewProjection);
}
