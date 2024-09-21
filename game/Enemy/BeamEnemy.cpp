#include "BeamEnemy.h"

BeamEnemy::BeamEnemy()
{
}

BeamEnemy::~BeamEnemy()
{
}

void BeamEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model, Player* player)
{
	player_ = player;

	worldTransform_.Initialize();
	worldTransform_.translation_.x = (int)transform.translate.x * 4.0f;
	worldTransform_.translation_.y = (int)transform.translate.y * 4.0f;
	worldTransform_.translation_.z = (int)transform.translate.z * 4.0f;
	worldTransform_.scale_ = transform.scale;

	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;
	velocity_.x = std::clamp(velocity_.x, -1.0f, 1.0f);
	velocity_.y = std::clamp(velocity_.y, -1.0f, 1.0f);
	velocity_.z = std::clamp(velocity_.z, -1.0f, 1.0f);
	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kRaser;
	isAlive_ = true;
	currentCount = 0;
	model_ = model;
	model_->setIsLighting(false);

	setReticle(worldTransform_.translation_);
}

void BeamEnemy::Update()
{
	currentCount++;
	if (currentCount >= startCount_) {
		count_++;
		Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
		obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y  ,worldTransform_.scale_.z / 2.0f };
		obb_.center = worldTransform_.translation_;
		GetOrientations(rotateMatrix, obb_.orientation);
		if (!ishit_) {
			worldTransform_.scale_ = Add(worldTransform_.scale_, velocity_);
		}
		else {
			worldTransform_.scale_.y = std::abs(worldTransform_.translation_.y - target_.translate.y);
		}
	}
	worldTransform_.UpdateMatrix();
	if (count_ >= 1000) {
		isAlive_ = false;
	}
	ishit_ = false;
}

void BeamEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	model_->Draw(worldTransform_, viewProjection);
}

void BeamEnemy::isCollision(OBB pertner)
{
	ishit_ = true;
	target_.translate = pertner.center;
	target_.scale = pertner.size;

}
