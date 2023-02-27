#include "Sound.h"

#include "Logger.h"
#include "FileUtil.h"

#include <Rapidjson/document.h>
#include <Rapidjson/istreamwrapper.h>
#include <fstream>
#include <Rapidjson/error/en.h>
#include <iostream>
#include "DX12Helper.h"
#include "Tool/JsonLoader.h"
#include "Helper/EnumIterator.h"
#include "Helper/StringHelper.h"

namespace MyObj
{
	//  音量の倍率の変化量
	static const float CHANGE_VOLUME_MAGNIFICATION = 0.2f;

	Sound* Sound::singleton = nullptr;

	Sound::Sound():
		audioEngineflag(DirectX::AudioEngine_Default),
		audioEngine(),
		se(),
		seBaseVol(0.0f),
		seSoundQue(),
		bgm(),
		bgmBaseVol(0.0f),
		bgmSoundQue(),
		bgmType(Res::BGMType::End),
		bgmId(-1),
		seVolMag(1.0f),
		bgmVolMag(1.0f)
	{
	}

	Sound::~Sound()
	{
	}

	void Sound::OnInit()
	{
		if (!singleton)
		{
			singleton = new Sound;
			// オーディオエンジンを設定
			singleton->audioEngine = std::make_unique<DirectX::AudioEngine>(singleton->audioEngineflag);
		}
		singleton->Init();
	}

	void Sound::OnTerm()
	{
		if (singleton)
		{
			singleton->audioEngine.release();
			delete singleton;
			singleton = nullptr;
		}
	}

	void Sound::Init()
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		singleton->LoadSE();
		singleton->LoadBGM();
	}

	void Sound::LoadSE()
	{
		// SEデータJSON読み込み
		auto data = System::Tool::LoadJson("Resource/JsonData/SEData.json");

		// メモリ予約
		auto dataNum = data.MemberCount();
		singleton->se.reserve(dataNum);
		singleton->seBaseVol.reserve(dataNum);

		// 2重ロードチェック後、データ読み込み
		typedef System::Helper::EnumIterator < Res::SEType, Res::SEType::Begin, Res::SEType::End > typeItr;
		for (auto itr = typeItr(); itr != typeItr(Res::SEType::End); ++itr)
		{
			const std::string tempId = std::to_string(itr.GetId());
			const char* tempPath = data[tempId.c_str()]["Path"].GetString();

			for (auto itr2 = typeItr(); itr2 != typeItr(Res::SEType::End); ++itr2)
			{
				const std::string tempId2 = std::to_string(itr2.GetId());
				const char* tempPath2 = data[tempId2.c_str()]["Path"].GetString();

				if (tempPath == tempPath2 && itr != itr2)
				{
					// 2重ロード
					ELOG("Error : Sound Double Loading");
				}
			}
			// パスを取得後、DX12で使用可能なワイド文c字列に変換
			const std::wstring path = System::Helper::ToWstr(tempPath);
			// ファイル読み込み
			singleton->se[*itr] = std::make_unique<DirectX::SoundEffect>(singleton->audioEngine.get(), path.data());
			// エラーチェック
			if (singleton->se[*itr] == nullptr)
			{
				ELOG("Error : Sound Load Missing");
			}
			// 初期音量読み込み
			singleton->seBaseVol[*itr] = data[tempId.c_str()]["Volume"].GetFloat();
		}
	}

	void Sound::LoadBGM()
	{
		// BGMデータJSON読み込み
		auto data = System::Tool::LoadJson("Resource/JsonData/BGMData.json");

		// メモリ予約
		auto dataNum = data.MemberCount();
		singleton->bgm.reserve(dataNum);
		singleton->bgmBaseVol.reserve(dataNum);

		// 2重ロードチェック後、データ読み込み
		typedef System::Helper::EnumIterator < Res::BGMType, Res::BGMType::Begin, Res::BGMType::End > typeItr;
		for (auto itr = typeItr(); itr != typeItr(Res::BGMType::End); ++itr)
		{
			const std::string tempId = std::to_string(itr.GetId());
			const char* tempPath = data[tempId.c_str()]["Path"].GetString();

			for (auto itr2 = typeItr(); itr2 != typeItr(Res::BGMType::End); ++itr2)
			{
				const std::string tempId2 = std::to_string(itr2.GetId());
				const char* tempPath2 = data[tempId2.c_str()]["Path"].GetString();

				if (tempPath == tempPath2 && itr != itr2)
				{
					// 2重ロード
					ELOG("Error : Sound Double Loading");
				}
			}
			// パスを取得後、DX12で使用可能なワイド文c字列に変換
			const std::wstring path = System::Helper::ToWstr(tempPath);
			// ファイル読み込み
			singleton->bgm[*itr] = std::make_unique<DirectX::SoundEffect>(singleton->audioEngine.get(), path.data());
			// エラーチェック
			if (singleton->bgm[*itr] == nullptr)
			{
				ELOG("Error : Sound Load Missing");
			}
			// 初期音量読み込み
			singleton->bgmBaseVol[*itr] = data[tempId.c_str()]["Volume"].GetFloat();
		}
	}

	int Sound::PlaySE(const Res::SEType& _Type)
	{
		const bool IsLoop = false;
		uint16_t id = 0;
		// 指定されたタイプのサウンドキューを探索。停止中のキューがあれば再生に変更
		for (auto itr : singleton->seSoundQue[_Type])
		{
			if (itr->GetState() == DirectX::SoundState::STOPPED)
			{
				itr->Play(IsLoop);
				return id;
			}
			++id;
		}
		// 指定されたタイプのサウンドキューがない場合、もしくは全て再生中なら新しくサウンドキューを作成して再生する
		PushSESoundQue(_Type);
		return id;
	}

	void Sound::StopSE(const Res::SEType& _Type, const int& _Index, const bool& _Immediate)
	{
		singleton->seSoundQue[_Type][_Index]->Stop(_Immediate);
	}

	void Sound::PlayBGM(const Res::BGMType& _Type)
	{
		const bool IsLoop = true;
		uint8_t id = 0;
		// 現在再生中のBGMを停止する
		StopBGM(true);
		singleton->bgmType = _Type;
		// 指定されたタイプのサウンドキューを探索。停止中のキューがあれば再生に変更
		for (auto itr : singleton->bgmSoundQue[_Type])
		{
			if (itr->GetState() == DirectX::SoundState::STOPPED)
			{
				itr->Play(IsLoop);
				singleton->bgmId = id;
				return;
			}
			++id;
		}
		// 指定されたタイプのサウンドキューがない場合、もしくは全て再生中なら新しくサウンドキューを作成して再生する
		PushBgmSoundQue(_Type);
		singleton->bgmId = id;
	}

	void Sound::StopBGM(const bool& _Immediate)
	{
		if (singleton->bgmId != -1)
		{
			singleton->bgmSoundQue[singleton->bgmType][singleton->bgmId]->Stop(_Immediate);
			singleton->bgmId = -1;
		}
	}

	//void Sound::ChangeVolume(const bool _IsUp)
	//{
	//	//  音量の倍率を変更
	//	if (_IsUp)
	//	{
	//		singleton->volumeMagnification += CHANGE_VOLUME_MAGNIFICATION;
	//	}
	//	else
	//	{
	//		singleton->volumeMagnification -= CHANGE_VOLUME_MAGNIFICATION;
	//	}

	//	for (int i = 0; i < singleton->soundEffect.size(); ++i)
	//	{
	//		for (auto soundItr : singleton->soundQue[i])
	//		{
	//			soundItr->SetVolume(singleton->firstVolume[i] * singleton->volumeMagnification);
	//		}
	//	}
	//}

	void Sound::PushSESoundQue(const Res::SEType& _Type)
	{
		const bool IsLoop = false;
		//  指定されたタイプのサウンドのインスタンスを生成
		std::shared_ptr<DirectX::SoundEffectInstance> instance = singleton->se[_Type]->CreateInstance();
		//  ボリュームを現在のボリュームに合わせて設定
		instance->SetVolume(singleton->seBaseVol[_Type] * singleton->seVolMag);
		//  サウンドキュー内にインスタンスを格納
		singleton->seSoundQue[_Type].emplace_back(instance);
		//  再生
		singleton->seSoundQue[_Type].back()->Play(IsLoop);
	}

	void Sound::PushBgmSoundQue(const Res::BGMType& _Type)
	{
		const bool IsLoop = true;
		//  指定されたタイプのサウンドのインスタンスを生成
		std::shared_ptr<DirectX::SoundEffectInstance> instance = singleton->bgm[_Type]->CreateInstance();
		//  ボリュームを現在のボリュームに合わせて設定
		instance->SetVolume(singleton->bgmBaseVol[_Type] * singleton->bgmVolMag);
		//  サウンドキュー内にインスタンスを格納
		singleton->bgmSoundQue[_Type].emplace_back(instance);
		//  再生
		singleton->bgmSoundQue[_Type].back()->Play(IsLoop);
	}
}