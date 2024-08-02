#pragma once
#include "Input.h"
class GameController
{
public:
	static GameController* GetInstance();

	void Initialize();
	void Update();

	bool Enter();
	bool Cancel();
	bool Jump();
	bool ContinueJump();
	bool Reverse();
	//float Move();
	bool Left();
	bool Right();
	
private:
	GameController() = default;
	~GameController() = default;
	GameController(const GameController&) = delete;
	GameController& operator=(const GameController&) = delete;


	Input* input_ = nullptr;
	XINPUT_STATE joyState_;
	XINPUT_STATE preJoyState_;
	//コントローラの接続の有無
	bool isJoyStickConnect_;
	//最終入力(0:キーボード 1:コントローラ)
	int rastInput_;
};

