#include "StageChangeEnemy.h"
#include "game/Player.h"
StageChangeEnemy::StageChangeEnemy()
{
}

StageChangeEnemy::~StageChangeEnemy()
{
}

void StageChangeEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model, Player* player)
{

	worldTransform_.Initialize();
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;

	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;
	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kBullet;
	isAlive_ = true;
	count = 0;
	model_ = model;
	model_->setIsLighting(false);

	setReticle(worldTransform_.translation_);
}

void StageChangeEnemy::Update()
{
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	worldTransform_.UpdateMatrix();
	ImGui::Begin("enemy");
	ImGui::DragFloat3("velocity", &velocity_.x);
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x);

	ImGui::End();
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
	obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
	obb_.center = worldTransform_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);
	if (ishit_ == true) {
		count++;
	}
	if (count > 5) {
		isAlive_ = false;
	}
}

void StageChangeEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	model_->Draw(worldTransform_, viewProjection);
}

void StageChangeEnemy::isCollision(OBB pertner)
{
	ishit_ = true;
	isAlive_ = false;
}
