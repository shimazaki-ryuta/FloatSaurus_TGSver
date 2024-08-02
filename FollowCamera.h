#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
class FollowCamera
{
public:
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	inline void SetForcusPoint(const Vector3 forcusPoint) { forcusPoint_ = forcusPoint; };
	inline void SetTarget(WorldTransform* target) { target_ = target; };
	inline const ViewProjection& GetViewProjection() { return viewProjection_; };
	//void Reset();
	void ApplyGlobalVariables();
	void Shake();
private:
	// ビュープロジェクション
	ViewProjection viewProjection_;
	//視点の先、Zを大きくするほど傾きが小さくなる
	Vector3 forcusPoint_ = {0,0,100.0f};
	WorldTransform* target_ = nullptr;
	//Vector3 interTargert_;
	
	//カメラの位置
	Vector3 offset_ = {0,2.0f,-100.0f};
	
	//補間の強さ
	//float cameraDelay_;
	//カメラシェイク
	bool isShake_;
	float shakeForce_;
	float kShakeForce_;
};

