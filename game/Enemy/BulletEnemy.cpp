#include "BulletEnemy.h"

BulletEnemy::BulletEnemy()
{
}

BulletEnemy::~BulletEnemy()
{
}

void BulletEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model)
{

	worldTransform_.Initialize();
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
	obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
	obb_.center = { 300.0f,300.0f,300.0f };
	GetOrientations(rotateMatrix, obb_.orientation);
	MoveSpeed_ = moveSpeed;
	velocity_ = CreateVelocity(transform.translate);

	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kBullet;
	isAlive_ = true;
	currentCount = 0;
	worldTransform_.UpdateMatrix();
	model_ = model;
	model_->setIsLighting(false);

	//方向によって向きを変えてる
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
	if (velocity_.z < 0.0f) {
		worldTransform_.rotation_.z= 1.6f;
		worldTransform_.rotation_.y = -4.75f;
	}
	worldTransform_.UpdateMatrix();
}


void BulletEnemy::Update()
{
	currentCount++;
	
	
		worldTransform_.rotation_.x += 0.2f;
	
	
	if (currentCount >= startCount_) {
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

		Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
		obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
		obb_.center = worldTransform_.translation_;
		GetOrientations(rotateMatrix, obb_.orientation);
	}
	
		worldTransform_.UpdateMatrix();
}

void BulletEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	model_->Draw(worldTransform_, viewProjection);
}

void BulletEnemy::isCollision(OBB pertner)
{
	isAlive_ = false;
}
