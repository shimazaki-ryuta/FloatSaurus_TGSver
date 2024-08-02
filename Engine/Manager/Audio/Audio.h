#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include "GlobalVariables.h"
enum AudioNum {
	GameStart,
	Jump,
	EnemyPop,
	Reverce,
	Death,
	DeleteEnemy,
	inGameBGM,
	ResultBGM,
	Block,
	PlusWave,
	Bound,
	Clear,
};
// チャンクヘッダ
struct ChunkHeader {
	char id[4];	  // チャンク毎のID
	int32_t size; // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// "RIFF"
	char type[4];		// "WAVE"
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt"
	WAVEFORMATEX fmt;  // 波形フォーマット
};

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

class Audio
{
public:
public:
	static Audio* GetInstance();

	void Initialize();

	// 音声データの読み込み
	uint32_t SoundLoadWave(const char* filename);

	// 音声データ解放
	void SoundUnload(SoundData* soundData);

	// 音声再生
	void SoundPlayWave(uint32_t audioHandle, float volume);
	//ループ再生
	void SoundPlayloop(uint32_t audioHandle, float volume);

	void StopAudio(uint32_t audioHandle);
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	IXAudio2SourceVoice* sourceVoice[20];
	SoundData soundDatas[20];
	uint32_t audioHandle_;
	uint32_t handle_[20];
	static float SoundVolume[20];
};