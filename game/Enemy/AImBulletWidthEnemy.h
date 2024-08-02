#pragma once
#include "IEnemy.h"
#include "PlayerAimBullet.h"
class AImBulletWidthEnemy:public IEnemy
{
public:
	AImBulletWidthEnemy();
	~AImBulletWidthEnemy();
	virtual void Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model) override;
	virtual void Update() override;
	virtual void Draw(const ViewProjection& viewProjection) override;
	virtual void isCollision(OBB pertner) override;
	static void  TextureInitialize();
	virtual bool GetIsAlive()override { return isAlive_; }
	virtual OBB& GetOBB()override { return obb_; };



	virtual bool GetIsHit()override { return ishit_; }
	virtual void SetPartener(collisionPartner partner)override { collisionpartner_ = partner; }
private:
   std::unique_ptr<Model> mainmodel;
	int coolTime_;
	int currentCollTime_;
	int atackCount_;
	int currentAtackCount_;
	Vector3 BulletScale_;
	void Atack();
};

