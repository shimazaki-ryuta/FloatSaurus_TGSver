#include "PlayerAimBullet.h"

void PlayerAimBullet::Initialize(Vector3 velocity, Transform transform, uint32_t tex, Model* model)
{
	
	velocity_ = velocity;
	worldTransform_.translation_ = transform.translate;
	//worldTransform_.rotation_ = transform.rotate;
	worldTransform_.scale_ = transform.scale;
	texture_ = tex;

	isAlive_ = true;
	model_ = model;
	//// Y軸回り
	if (velocity_.x > 0.0f) {
		worldTransform_.rotation_.y = 0.0f;
	}
	if (velocity_.x < 0.0f) {
		worldTransform_.rotation_.y = 3.2f;
	}
	if (velocity_.y > 0.0f) {
		worldTransform_.rotation_.z = 1.6f;
	}
	if (velocity_.y < 0.0f) {
		worldTransform_.rotation_.z = 4.75f;
	}
	worldTransform_.Initialize();
	
}

void PlayerAimBullet::Update()
{
		worldTransform_.rotation_.x += 0.2f;
	


	if (std::abs(worldTransform_.translation_.x) > 100.0f) {
		isAlive_ = false;
	}
	if (std::abs(worldTransform_.translation_.y) > 100.0f) {
		isAlive_ = false;
	}
	worldTransform_.translation_ = Add(velocity_, worldTransform_.translation_);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
	obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
	obb_.center = worldTransform_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);
	worldTransform_.UpdateMatrix();
}

void PlayerAimBullet::Draw(const ViewProjection& view)
{
	
	model_->Draw(worldTransform_, view);

}

void PlayerAimBullet::isCollision()
{
	isAlive_ = false;
}
