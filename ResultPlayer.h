#pragma once
#include "MapManager.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Input.h"
#include "Mymath.h"
#include <memory>

class ResultPlayer
{
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void ApplyGlobalVariables();
private:
	std::unique_ptr<Model> antena_;
	std::unique_ptr<Model> code_;
	std::unique_ptr<Model> head_;
	std::unique_ptr<Model> body_;
	std::unique_ptr<Model> back_;
	std::unique_ptr<Model> back2_;
	std::unique_ptr<Model> leg_;
//	std::unique_ptr<Model> fire_;


	WorldTransform worldTransformModel_;
	WorldTransform worldTransformAntena_;
	WorldTransform worldTransformCode_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformbody_;
	WorldTransform worldTransformback_;
	WorldTransform worldTransformLeftLeg_;
	WorldTransform worldTransformRightLeg_;
	//WorldTransform worldTransformFire_;

	Vector3 antenaOffset_;

	Vector3 codeOffset_;
	Vector3 headOffset_;
	Vector3 bodyOffset_;
	Vector3 backOffset_;
	Vector3 leftOffset_;
	Vector3 rightOffset_;
	//Vector3 fireOffset_;

	float theta_;

	Vector3 charctorScale_ = { 1.0f,1.0f,1.0f };

};

