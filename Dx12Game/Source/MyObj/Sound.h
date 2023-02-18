#pragma once
#include <DirectXTK12/Audio.h>
#include <unordered_map>
#include <vector>

namespace MyObj
{
	class Sound
	{
		Sound();
		~Sound();
		static Sound* singleton;

	public:
		// Public Method
		static void OnInit();
		static void OnTerm();

		static void Init();
		static void Load();																					// 読み込み
		static int Play(const int _Track, const bool _IsLoop, const bool _IsFromBegin);						// 再生
		static void Stop(const int _Track, const int _Index, const bool _Immediate = true);					// 停止
		static void ChangeVolume(const bool _IsUp);		// 音量変更
	private:
		// Private Variable
		//  サウンドのロードに必要な変数
		std::unordered_map<unsigned int, std::unique_ptr<DirectX::AudioEngine>> audioEngine;						// オーディオエンジン
		DirectX::AUDIO_ENGINE_FLAGS audioEngineflags;																// オーディオエンジンフラグ
		std::unique_ptr<uint8_t[]> wavData;																			// wavデータ

		//  サウンドの再生に必要な変数
		std::unordered_map<unsigned int, std::unique_ptr<DirectX::SoundEffect>> soundEffect;						// サウンドエフェクト
		std::unordered_map<unsigned int, float> firstVolume;
		std::unordered_map<unsigned int, std::vector<std::shared_ptr<DirectX::SoundEffectInstance>>> soundQue;	// サウンドキュー

		//  音量の倍率
		float volumeMagnification;
		////  シングルトンクラスへの参照
		//class CheckError& m_error;
		// Private Method
		static void PushSoundQue(const int _Track, const bool _IsLoop);	// サウンドキュー作成

	};
}