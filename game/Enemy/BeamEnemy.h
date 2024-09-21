#pragma once
#include "IEnemy.h"
class BeamEnemy:public IEnemy
{
public:
	BeamEnemy();
	~BeamEnemy();
	virtual void Initialize(const Transform& transform, const Vector3& velocity, float moveSpeed, uint32_t texture, Model* model, Player* player) override;
	virtual void Update() override;
	virtual void Draw(const ViewProjection& viewProjection) override;
	virtual void isCollision(OBB pertner) override;

	virtual bool GetIsAlive()override { return isAlive_; }
	virtual OBB& GetOBB()override { return obb_; };


	virtual bool GetIsHit()override { return ishit_; }
	virtual void SetPartener(collisionPartner partner)override { collisionpartner_ = partner; }
private:
	Transform target_;
	int count_;
	int currentCount;
};

