#pragma once
#include "worldTransform.h"
#include "viewProjection.h"
#include "Sphere.h"
#include <memory>
#include "textureManager.h"
#include "model.h"
enum ReflectionCount {
	ReflectInfinit,
	reflect4,
};
enum collisionPartner {
	kwall,
	kflore,
};
enum EnemyType {
	kBullet,
	kReflect,
	kBound,
	kTire,
	kSpear,
	kRaser,
	kAimBulletWidth,
	kAimBulletHeight,
	kAimBound,
	kStageUp,
	kStageDown,
	kHoming
};
class GameScene;
class Player;
class IEnemy
{
public:
	IEnemy();
	~IEnemy();
	virtual void Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model) = 0;
	//入力した位置からvelocityを作成
	Vector3 CreateVelocity(Vector3 transform);

	virtual void Update() = 0;
	virtual void Draw(const ViewProjection& viewProjection) = 0;
	virtual void isCollision(OBB pertner) = 0;
	virtual bool GetIsAlive() { return isAlive_; }
	//当たり判定の取得
	virtual OBB& GetOBB() { return obb_; };
	
	virtual void SetStartCount(int count) { startCount_ = count; }
	virtual void SetType(EnemyType a) { type_ = a; }
	virtual bool GetIsHit() { return ishit_; }
	virtual void SetPartener(collisionPartner partner) { collisionpartner_ = partner; }
	virtual EnemyType GetType() { return type_; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
	WorldTransform& GetWorldTransform() { return worldTransform_; };
	void Deth() { isAlive_ = false; };


protected:
	Texturemanager* texManager_;
	ReflectionCount reflectionCount_;
	WorldTransform targetWordTransform_;
	Vector3 velocity_;
	float MoveSpeed_;
	uint32_t texindex_;
	bool isAlive_;
	OBB obb_;
	bool ishit_;
	int cooltime_;
	int reflectCount_;
	Vector3 prePos_;
	bool isDown_;
	collisionPartner collisionpartner_;
	EnemyType type_;
	WorldTransform worldTransform_;
	GameScene* gameScene_;
	Player* player_;
	int startCount_;
	Model* model_;
};

