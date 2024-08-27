#include "TireEnemy.h"

TireEnemy::TireEnemy()
{
}

TireEnemy::~TireEnemy()
{
}

void TireEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model)
{
	/*sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();*/

	worldTransform_.Initialize();
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;

	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;
	velocity_.x = std::clamp(velocity_.x, -1.0f, 1.0f);
	velocity_.y = std::clamp(velocity_.y, -1.0f, 1.0f);
	velocity_.z = std::clamp(velocity_.z, -1.0f, 1.0f);

	velocity_.y = 0.0f;
	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kBullet;
	isAlive_ = true;
	ishit_ = false;
	model_ = model;
	SetType(kTire);
	model_->setIsLighting(false);
}

void TireEnemy::Update()
{
	prePos_ = worldTransform_.translation_;
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
	obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
	obb_.center = worldTransform_.translation_;
	obb_.center.y -= worldTransform_.scale_.y/2.0f;
	GetOrientations(rotateMatrix, obb_.orientation);
	if (!ishit_) {
		velocity_.y = -0.5f;
		worldTransform_.translation_.y += velocity_.y;
	}
	else {
		worldTransform_.rotation_.z -= velocity_.x;
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
		velocity_.y = 0.0f;
	}
	
	worldTransform_.UpdateMatrix();
	ishit_ = false;
}

void TireEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	model_->Draw(worldTransform_, viewProjection);
}

void TireEnemy::isCollision(OBB pertner)
{
	
		if (obb_.center.y + obb_.size.y > pertner.center.y + pertner.size.y) {
			ishit_ = true;
			worldTransform_.translation_.y = pertner.center.y+pertner.size.y+(obb_.size.y*2.0f);
		}
	
	else if (std::abs(obb_.center.x - pertner.center.x) <= worldTransform_.scale_.x * 2.0f) {
		if (std::abs(obb_.center.x - pertner.center.x) <= worldTransform_.scale_.x * 2.0f) {
			velocity_.x *= -1.0f;
			worldTransform_.translation_.x = prePos_.x;
		}
	}
}
