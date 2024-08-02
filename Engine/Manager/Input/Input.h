#pragma once
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "xinput.lib")
#include <array>
#include <wrl.h>
#include "WinApp.h"

class Input
{
public:
	
	static Input* GetInstance();
	void Initialize(WinApp* winApp);
	void Update();
	const int DEADZONE = 7849;
	/// 押した時
	bool PushKey(uint8_t keyNumber)const;
	/// 押している間
	bool PressKey(uint8_t keyNumber)const;
	/// 離した時
	bool IsReleseKey(uint8_t keyNumber)const;
	//joystateがつながっているかどうか
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;
	//デッドゾーンの設定
	void SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out)const;
private:
	/*Input() = default;
	~Input() = default;*/
	static Input* input_;
	Microsoft::WRL::ComPtr<IDirectInput8>directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard = nullptr;
	std::array<BYTE, 256> keys;
	std::array<BYTE, 256> preKeys;
	int count;

};