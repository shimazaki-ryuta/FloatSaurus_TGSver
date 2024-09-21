#include "AImBulletWidthEnemy.h"
#include "game/Player.h"
#include "game/Scenes/gameScene.h"
AImBulletWidthEnemy::AImBulletWidthEnemy()
{
}

AImBulletWidthEnemy::~AImBulletWidthEnemy()
{
}

void AImBulletWidthEnemy::Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model, Player* player)
{

	mainmodel.reset(Model::CreateModelFromObj("Resource/Enemy", "bulletMother.obj"));

	worldTransform_.Initialize();
	worldTransform_.translation_ = transform.translate;
	worldTransform_.scale_ = transform.scale;
	BulletScale_ = { 1.0f,1.0f,1.0f };
	MoveSpeed_ = moveSpeed;
	velocity_ = velocity;

	texindex_ = texture;
	velocity_ = Multiply(MoveSpeed_, velocity_);
	type_ = kAimBulletWidth;
	isAlive_ = true;
	atackCount_ = 4;
	coolTime_ = 50;
	model_ = model;
	model_->setIsLighting(false);

	setReticle(worldTransform_.translation_);
}

void AImBulletWidthEnemy::Update()
{
	Matrix4x4 rotateMatrix = MakeRotateMatrix(Vector3{ 0.0f,0.0f,0.0f });
	obb_.size = { worldTransform_.scale_.x / 2.0f ,worldTransform_.scale_.y / 2.0f ,worldTransform_.scale_.z / 2.0f };
	obb_.center = worldTransform_.translation_;
	GetOrientations(rotateMatrix, obb_.orientation);
	currentCollTime_++;
	worldTransform_.rotation_.x += currentAtackCount_ / 15.0f;
	if (player_) {
		worldTransform_.translation_.y = player_->GetWorldTransform().translation_.y+1.0f;
	}
	if (currentAtackCount_ <= atackCount_)
		if (currentCollTime_ >= coolTime_) {
			Atack();
			currentCollTime_ = 0;
			currentAtackCount_++;
		}
	if (currentAtackCount_ >= atackCount_) {
		isAlive_ = false;
	}
	worldTransform_.UpdateMatrix();
}

void AImBulletWidthEnemy::Draw(const ViewProjection& viewProjection)
{
	//sphere_->Draw({ 1.0f,1.0f,1.0f,1.0f }, worldTransform_, texindex_, viewProjection);
	mainmodel->Draw(worldTransform_, viewProjection);
}

void AImBulletWidthEnemy::isCollision(OBB pertner)
{
}

void AImBulletWidthEnemy::TextureInitialize()
{
	
		
}

void AImBulletWidthEnemy::Atack()
{
	PlayerAimBullet* newBullet = new PlayerAimBullet();
	Vector3 bulletvel;
	if (worldTransform_.translation_.x > targetWordTransform_.GetWorldPos().x) {
		bulletvel = { -1.0f,0.0f,0.0f };
	}
	else {
		bulletvel = { 1.0f,0.0f,0.0f };
	}
	newBullet->Initialize(Multiply(MoveSpeed_, bulletvel), { {1.5f,2.0f,2.0f},worldTransform_.rotation_,worldTransform_.translation_ }, texindex_, model_);
	gameScene_->AddEnemyBullet(newBullet);
}
