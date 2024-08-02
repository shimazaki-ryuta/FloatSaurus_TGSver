#include "FollowCamera.h"
#include "Globalvariables.h"
#include "RandomEngine.h"
void FollowCamera::Initialize()
{
	viewProjection_.Initialize();
	GlovalVariables* grovalVariables = GlovalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	grovalVariables->CreateGroup(groupName);
	grovalVariables->AddItem(groupName, "offset", offset_);
	grovalVariables->AddItem(groupName, "forcusPoint", forcusPoint_);
	grovalVariables->AddItem(groupName, "shakeForce", kShakeForce_);
}

void FollowCamera::Update()
{
	ApplyGlobalVariables();
	if (target_)
	{
		//Vector3 offset = { 0.0f,2.0f,-20.0f };

		Vector3 playerPosition = target_->GetWorldPos();
		forcusPoint_.y = playerPosition.y;
		Vector3 direction = forcusPoint_ - playerPosition;

		Matrix4x4 rotateMatrix = DirectionToDirection({0,0,1.0f},Normalise(direction));

		//offset = TransformNormal(offset, rotateMatrix);

		//interTargert_ = Lerp(interTargert_, target_->GetWorldPosition(), cameraDelay_);

		viewProjection_.translation_ =  TransformNormal(offset_, rotateMatrix);
		viewProjection_.translation_.x += target_->GetWorldPos().x;
	
		if (isShake_) {
			viewProjection_.translation_.x += RandomEngine::GetInstance()->GetRandom(-shakeForce_/2.0f, shakeForce_ / 2.0f);
			viewProjection_.translation_.y += RandomEngine::GetInstance()->GetRandom(-shakeForce_ / 2.0f, shakeForce_ / 2.0f);
			shakeForce_ *= 0.8f;
			if (shakeForce_ <= 0.1f) {
				isShake_ = false;
			}
		}

		//手動アップデート
		viewProjection_.matView = Inverse(Multiply(Multiply(MakeScaleMatrix({1.0f,1.0f,1.0f}) , rotateMatrix),MakeTranslateMatrix(viewProjection_.translation_)) );
		viewProjection_.UpdateProjectionMatrix();
	}
}

void FollowCamera::Shake() {
	isShake_ = true;
	shakeForce_ = kShakeForce_;
}

void FollowCamera::ApplyGlobalVariables()
{
	GlovalVariables* globalVariables = GlovalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	//cameraDelay_ = globalVariables->GetFloatValue(groupName, "CameraDelay");
	offset_ = globalVariables->GetVector3Value(groupName, "offset");
	forcusPoint_ = globalVariables->GetVector3Value(groupName, "forcusPoint");
	kShakeForce_ = globalVariables->GetFloatValue(groupName, "shakeForce");
}
