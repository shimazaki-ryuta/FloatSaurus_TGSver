#include "Input.h"
Input* Input::GetInstance()
{
	if (!input_) {
		input_=new Input();
	}
	return input_;
}

void Input::Initialize(WinApp* winApp) {
	//DirectInputのオブジェクトを作成
	HRESULT hr = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));
	//キーボードデバイスを生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	keys = {};
	preKeys = {};
	count = 0;
}

void Input::Update()
{
	preKeys = keys;
	//キーボード情報の取得開始
	keyboard->Acquire();
	keys = {};
	//全てのキーの入力状態を取得する
	keyboard->GetDeviceState(sizeof(keys), &keys);
	
}

bool Input::PushKey(uint8_t keyNumber)const
{
	if (keys[keyNumber]  && !preKeys[keyNumber] ) {
		return true;
	}
	else {
		return false;
	}
}
bool Input::PressKey(uint8_t keyNumber)const
{
	if (keys[keyNumber] ) {
		return true;
	}
	else {
		return false;
	}
}


bool Input::IsReleseKey(uint8_t keyNumber)const
{
	if (!keys[keyNumber]  && preKeys[keyNumber] ) {
		return true;
	}
	else {
		return false;
	}
}

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const
{
	DWORD result;
	result = XInputGetState(stickNo, &out);

	if (result == ERROR_SUCCESS) {
		SetJoyStickDeadZone(stickNo, out);
		return true;
	}
	else {
		return false;
	}
}

void Input::SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out)const
{
	int LstickX = static_cast<int>(out.Gamepad.sThumbLX);
	int LstickY = static_cast<int>(out.Gamepad.sThumbLY);
	int RstickX = static_cast<int>(out.Gamepad.sThumbRX);
	int RstickY = static_cast<int>(out.Gamepad.sThumbRY);
	if (abs(LstickX) < DEADZONE) {
		LstickX = 0;
		out.Gamepad.sThumbLX = LstickX;
	}
	if (abs(LstickY) < DEADZONE) {
		LstickY = 0;
		out.Gamepad.sThumbLY = LstickY;
	}
	if (abs(RstickX) < DEADZONE) {
		RstickX = 0;
		out.Gamepad.sThumbRX = RstickX;
	}
	if (abs(RstickY) < DEADZONE) {
		RstickY = 0;
		out.Gamepad.sThumbRY = RstickY;
	}

}

Input* Input::input_ = nullptr;