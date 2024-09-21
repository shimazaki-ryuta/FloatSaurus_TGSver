#include "AimBulletEnemy.h"
#include "game/Player.h"
#include "game/Scenes/gameScene.h"
AimBulletEnemy::AimBulletEnemy()
{
}

AimBulletEnemy::~AimBulletEnemy()
{
}

void AimBulletEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture,  Model* model){
	/*sphere_ = std::make_unique<Sphere>();
	sphere_->Initialize();*/
	mainmodel.reset(Model::CreateModelFromObj("Resource/Enemy", "bulletMother.obj"));
	worldTransform_.Initialize();
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;
	
	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;

	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kAimBulletHeight;
	isAlive_ = true;
	atackCount_ = 8;
	coolTime_ = 30;
	model_ = model;
	model_->setIsLighting(false);
}

void AimBulletEnemy::Update()
{
  currentCollTime_++;
  worldTransform_.rotation_.y += currentAtackCount_ / 15.0f;
  Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
  obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
  obb_.center = worldTransform_.translation_;
  GetOrientations(rotateMatrix, obb_.orientation);
  if (player_) {
		worldTransform_.translation_.x = player_->GetWorldTransform().translation_.x;
	}
	if(currentAtackCount_<=atackCount_)
	if (currentCollTime_>=coolTime_) {
		Atack();
		currentCollTime_ = 0;
		currentAtackCount_++;
	}
	if (currentAtackCount_ >= atackCount_) {
		isAlive_ = false;
	}
	worldTransform_.UpdateMatrix();
}

void AimBulletEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	mainmodel->Draw(worldTransform_, viewProjection);
}


void AimBulletEnemy::isCollision(OBB pertner)
{
}

void AimBulletEnemy::TextureInitialize()
{
	
		
	
}

void AimBulletEnemy::Atack()
{
	PlayerAimBullet* newBullet = new PlayerAimBullet();
	newBullet->Initialize(Multiply(MoveSpeed_,{ 0.0f,-1.0f,0.0f }), { {1.5f,2.0f,2.0f},worldTransform_.rotation_,worldTransform_.translation_}, texindex_, model_);
	gameScene_->AddEnemyBullet(newBullet);
}
