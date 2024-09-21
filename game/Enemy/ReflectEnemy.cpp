#include "ReflectEnemy.h"
#include "game/Player.h"
ReflectEnemy::ReflectEnemy()
{
}

ReflectEnemy::~ReflectEnemy()
{
}

void ReflectEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model, Player* player)
{
	player_ = player;
	
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;
	worldTransform_.Initialize();
	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;
	velocity_.x = std::clamp(velocity_.x, -1.0f, 1.0f);
	velocity_.y = std::clamp(velocity_.y, -1.0f, 1.0f);
	velocity_.z = std::clamp(velocity_.z, -1.0f, 1.0f);
	texindex_ = texture;

	isAlive_ = true;
	ishit_ = false;
	cooltime_ = 0;

	reflectCount_ = 0;
	isDown_ = false;
	type_ = kReflect;
	model_=model;
	model_->setIsLighting(false);
	velocity_ = Subtract(player_->GetWorldTransform().GetWorldPos(), worldTransform_.translation_);
	velocity_ = Normalise(velocity_);
	velocity_ = Multiply(MoveSpeed_, velocity_);
	//worldTransform_.UpdateMatrix();
	currentTime_ = 0;

	setReticle(worldTransform_.translation_);
}

void ReflectEnemy::Update()
{
	currentTime_++;
	if (currentTime_ >= startCount_) {
		//prePos_ = worldTransform_.translation_;
		Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
		obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
		obb_.center = worldTransform_.translation_;
		GetOrientations(rotateMatrix, obb_.orientation);
		worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

		
		if (ishit_ == true) {
			cooltime_++;
		}
		if (cooltime_ >= 10) {
			ishit_ = false;
			cooltime_ = 0;
		}

		if (reflectCount_ >= 12) {
			isAlive_ = false;
		}


		if (worldTransform_.translation_.y >= 52) {
			if (!ishit_) {

				ishit_ = true;
				reflectCount_++;
				velocity_.y *= -1.0f;
			//	worldTransform_.translation_ = prePos_;
			}

		}

	}
		
	worldTransform_.UpdateMatrix();
}

void ReflectEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	model_->Draw(worldTransform_, viewProjection);
}

void ReflectEnemy::isCollision(OBB pertner)
{
	if (!ishit_) {
				
				ishit_ = true;
				reflectCount_++;
				if (collisionpartner_ == kflore) {
					if (std::abs(obb_.center.y - pertner.center.y) <= worldTransform_.scale_.y * 2.0f) {
						velocity_.y *= -1.0f;
						//worldTransform_.translation_ = prePos_;
						if (obb_.center.y >= pertner.center.y) {
							worldTransform_.translation_.y = pertner.center.y + pertner.size.y + worldTransform_.scale_.y;

						}
						else {
							worldTransform_.translation_.y = pertner.center.y - pertner.size.y - worldTransform_.scale_.y;

						}
					}
					else if (std::abs(obb_.center.x - pertner.center.x) <= worldTransform_.scale_.x * 2.0f) {
		
						velocity_.x *= -1.0f;
						worldTransform_.translation_ = prePos_;
					}
				}
				else {
				
					if (std::abs(obb_.center.x - pertner.center.x) <= worldTransform_.scale_.x * 2.0f) {
		
						velocity_.x *= -1.0f;
						worldTransform_.translation_ = prePos_;
					}else if (std::abs(obb_.center.y - pertner.center.y) <= worldTransform_.scale_.y * 2.0f) {
						velocity_.y *= -1.0f;
						worldTransform_.translation_ = prePos_;
		
					}
				}
					
					
			}
}
