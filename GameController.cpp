#include "GameController.h"

GameController* GameController::GetInstance()
{
	static GameController instance;
	return &instance;
}

void GameController::Initialize() {
	isJoyStickConnect_ = Input::GetInstance()->GetJoystickState(0, joyState_);
}

void GameController::Update() {
	preJoyState_ = joyState_;
	Input::GetInstance()->GetJoystickState(0, joyState_);
}

bool GameController::Enter() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		rastInput_ = 0;
		return true;
	}
	if ((joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
		rastInput_ = 1;
		return true;
	}
	return false;
}
bool GameController::Cancel() {
	return false;
}
bool GameController::Jump() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		rastInput_ = 0;
		return true;
	}
	if ((joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A) && !(preJoyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
		rastInput_ = 1;
		return true;
	}
	return false;
}

bool GameController::ContinueJump() {
	if (Input::GetInstance()->PressKey(DIK_SPACE)) {
		rastInput_ = 0;
		return true;
	}
	if ((joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_A)) {
		rastInput_ = 1;
		return true;
	}
	return false;
}

bool GameController::Reverse() {
	if (Input::GetInstance()->PushKey(DIK_X)) {
		rastInput_ = 0;
		return true;
	}  
	if(joyState_.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD && preJoyState_.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
		rastInput_ = 1;
		return true;
	}
	return false;
}
/*float GameController::Move() {
	if (Input::GetInstance()->PressKey(DIK_LEFT) || joyState_->Gamepad.sThumbLX < 0) {
		//velocity_.y = 0.0f;
		//acceleration_ = { 0 ,0.06f,0 };
		velocity_.x = -1.0f * moveSpeed_;
	}
}*/
bool GameController::Left() {
	if (Input::GetInstance()->PressKey(DIK_LEFT)) {
		rastInput_ = 0;
		return true;
	}
	if (joyState_.Gamepad.sThumbLX < 0) {
		rastInput_ = 1;
		return true;
	}
	return false;
}

bool GameController::Right() {
	if (Input::GetInstance()->PressKey(DIK_RIGHT)) {
		rastInput_ = 0;
		return true;
	} 
	if (joyState_.Gamepad.sThumbLX > 0) {
		rastInput_ = 1;
		return true;
	}
	return false;
}

bool GameController::CloseWindow() {
	if (Input::GetInstance()->PressKey(DIK_ESCAPE)) {
		rastInput_ = 0;
		return true;
	}
	if (joyState_.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) {
		rastInput_ = 1;
		return true;
	}
	return false;
}
