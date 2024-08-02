#include <game/Manager/SceneManager.h>
//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager->Run();
	return 0;
}