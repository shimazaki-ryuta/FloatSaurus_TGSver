#pragma once
enum  Scene {
	TITLE_SCENE,
	GAME_SCENE,
	RESULT_SCENE,
};
class Iscene
{
public:
	virtual ~Iscene() {};
	virtual void Initialize()=0;

	virtual void Update()=0;

	virtual void Draw()=0;

	virtual void Finalize() = 0;
	int GetSceneNum() { return sceneNum; }
	bool GetIsEnd() { return isEnd_; };

protected:

	static int sceneNum;

	static bool isEnd_;
};

