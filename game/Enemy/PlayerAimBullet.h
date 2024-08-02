#pragma once
#include "worldTransform.h"
#include "viewProjection.h"
#include "Sphere.h"
#include <memory>
#include "textureManager.h"
#include "model.h"
class PlayerAimBullet
{
public: 
	void Initialize(Vector3 velocity,Transform transform,uint32_t tex,Model* model);
	void Update();
	void Draw(const ViewProjection& view);
	OBB& GetOBB() { return obb_; };
	void isCollision();
	bool GetIsAlive() { return isAlive_; }
private:
	WorldTransform worldTransform_;
	Vector3 velocity_;
	uint32_t texture_;
	
	OBB obb_;
	bool isAlive_;
	Model* model_;
};

