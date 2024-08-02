#pragma once
#include "Iscene.h"
#include "Input.h"
class TitleScene:public Iscene
{
private:
	int count;
public:
	~TitleScene()override;
	 void Initialize() override;

	 void Update() override;

	 void Draw() override;
	 void Finalize()override;
private:
	Input* input=nullptr;
};

