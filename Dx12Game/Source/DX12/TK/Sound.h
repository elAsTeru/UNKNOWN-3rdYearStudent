#pragma once
#include <DirectXTK12/Audio.h>
#include <unordered_map>
#include <vector>
#include "SoundType.h"

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

		void Init();

		void LoadSE();
		void LoadBGM();

		/// <summary> SEを再生する。SEタイプ </summary>
		static int PlaySE(const Res::SEType& _Type);
		/// <summary> SEを停止する。再生中のSEのID、すぐに停止するか？ </summary>
		static void StopSE(const Res::SEType& _Type, const int& _Index, const bool& _Immediate = true);

		/// <summary> BGMを再生する。BGMタイプ </summary>
		static void PlayBGM(const Res::BGMType& _Type);
		/// <summary> BGMを停止する。すぐに停止するか？ </summary>
		static void StopBGM(const bool& _Immediate = true);

		//static void ChangeVolume(const bool _IsUp);		// 音量変更
	private:
		// Private Variable

		//  サウンドのロードに必要な変数
		DirectX::AUDIO_ENGINE_FLAGS audioEngineflag;		// オーディオエンジンフラグ
		std::unique_ptr<DirectX::AudioEngine> audioEngine;

		// SEの再生に必要な変数
		std::unordered_map<Res::SEType, std::unique_ptr<DirectX::SoundEffect>> se;								// SE用サウンドエフェクト
		std::unordered_map<Res::SEType, float> seBaseVol;														// SE基準音量
		std::unordered_map<Res::SEType, std::vector<std::shared_ptr<DirectX::SoundEffectInstance>>> seSoundQue;	// SE用サウンドキュー
		// BGMの再生に必要な変数
		std::unordered_map<Res::BGMType, std::unique_ptr<DirectX::SoundEffect>> bgm;								// BGM用サウンドエフェクト
		std::unordered_map<Res::BGMType, float> bgmBaseVol;															// BGM基準音量
		std::unordered_map<Res::BGMType, std::vector<std::shared_ptr<DirectX::SoundEffectInstance>>> bgmSoundQue;	// BGM用サウンドキュー
		Res::BGMType bgmType;
		int8_t bgmId;

		//  音量の倍率
		float seVolMag;		// SEの音量倍率
		float bgmVolMag;	// BGMの音量倍率

		// Private Method
		
		/// <summary> サウンドキューを作成 </summary>
		static void PushSESoundQue(const Res::SEType& _Type);
		static void PushBgmSoundQue(const Res::BGMType& _Type);
	};
}

// SE...SOUND EFFECT		: オブジェクトが呼び出せる		複数同時に再生可能
// BGM...BACK GROUND MUSIC	: シーンが1つ持つことが出来る	複数同時に再生不可能
// AS...AMBIENT SOUND		: シーンが複数持つことが出来る	複数同時に再生可能