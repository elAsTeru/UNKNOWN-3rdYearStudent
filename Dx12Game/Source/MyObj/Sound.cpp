#include "Sound.h"

#include "Logger.h"
#include "FileUtil.h"

#include <Rapidjson/document.h>
#include <Rapidjson/istreamwrapper.h>
#include <fstream>
#include <Rapidjson/error/en.h>
#include <iostream>
#include "DX12Helper.h"

namespace MyObj
{
	//  音量の倍率の変化量
	static const float CHANGE_VOLUME_MAGNIFICATION = 0.2f;

	Sound* Sound::singleton = nullptr;

	Sound::Sound():
		volumeMagnification(1),
		audioEngineflags(DirectX::AudioEngine_Default),
		wavData(0),
		soundQue()
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
		}
		Init();
	}

	void Sound::OnTerm()
	{
		if (singleton)
		{
			delete singleton;
			singleton = nullptr;
		}
	}

	void Sound::Init()
	{
		CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		Load();
	}

	void Sound::Load()
	{
		// パスと初期音量をjsonから読み取り
		// 初期音量はそのまま記録していき
		// パスは、全て取得後、被るものがないか？比較して
		// 全て読み込む

		// サウンドデータ読み込み
		std::ifstream ifs("Resource/JsonData/SoundData.json");
		rapidjson::IStreamWrapper isw(ifs);
		// JSONファイル解析
		rapidjson::Document doc;
		doc.ParseStream(isw);
		// 読み込みチェック
		if (doc.HasParseError()) {
			std::cout << "error offset:" << doc.GetErrorOffset() << std::endl;
			std::cout << "error pase:" << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
		}
		// サウンドデータの構造は SE|BGM > Track > Path&Volume
		rapidjson::Value& data = doc;

		int soundCount = data["SE"].MemberCount() + data["BGM"].MemberCount();	// 読み込むサウンド数

		// メモリ予約
		singleton->audioEngine.reserve(soundCount);
		singleton->soundEffect.reserve(soundCount);
		singleton->firstVolume.reserve(soundCount);
		for (int i = 0; i < 2; ++i)
		{
			// 読み込むサウンドタイプ
			const char* type{};
			switch (i)
			{
			case 0:
				type = "SE";
				break;
			case 1:
				type = "BGM";
			}

			for (int j = 0; j < data[type].MemberCount(); ++j)
			{
				// 記録するトラックの番号を設定
				unsigned int trackNumber = j + (i * data["SE"].MemberCount());

				// オーディオエンジンを設定
				singleton->audioEngine[trackNumber] = std::make_unique<DirectX::AudioEngine>(singleton->audioEngineflags);

				// jをトラック番号に変換
				const std::string Temp = std::to_string(j + 1);
				const char* track = Temp.c_str();
				// ファイルパス取得
				const std::string tempPath = data[type][track]["Path"].GetString();
				// DirectXで読み込める形(ワイド型)に変換
				std::wstring path = GetWideStringFromString(tempPath);
				// ファイル読み込み
				singleton->soundEffect[trackNumber] = std::make_unique<DirectX::SoundEffect>(singleton->audioEngine[trackNumber].get(), path.data());
				// エラーチェック
				if (singleton->soundEffect[trackNumber] == nullptr)
				{
					ELOG("Error : Sound Load Missing");
				}
				// 初期音量読み込み
				singleton->firstVolume[trackNumber] = 0.01f;
					//data[type][track]["Volume"].GetFloat();
			}

		}
	}
	int Sound::Play(const int _Track, const bool _IsLoop, const bool _IsFromBegin)
	{
		// 添え字カウント用
		int index = 0;

		// 指定されたタイプのサウンドキューを探索
		for (auto itr : singleton->soundQue[_Track - 1])
		{
			//  停止中のキューがあればそのキューを再生に変更
			if (itr->GetState() == DirectX::SoundState::STOPPED)
			{
				itr->Play(_IsLoop);
				return index;
			}
			index++;
		}
		//  指定されたタイプのサウンドキューがない、
		//  もしくは全て再生中なら新しくサウンドキューを作成して再生する
		PushSoundQue(_Track - 1, _IsLoop);
		return index;
	}
	
	void Sound::Stop(const int _Track, const int _Index, const bool _Immediate)
	{
		singleton->soundQue[_Track-1][_Index]->Stop(_Immediate);
	}

	void Sound::ChangeVolume(const bool _IsUp)
	{
		//  音量の倍率を変更
		if (_IsUp)
		{
			singleton->volumeMagnification += CHANGE_VOLUME_MAGNIFICATION;
		}
		else
		{
			singleton->volumeMagnification -= CHANGE_VOLUME_MAGNIFICATION;
		}

		for (int i = 0; i < singleton->soundEffect.size(); ++i)
		{
			for (auto soundItr : singleton->soundQue[i])
			{
				soundItr->SetVolume(singleton->firstVolume[i] * singleton->volumeMagnification);
			}
		}
	}
	void Sound::PushSoundQue(const int _Track, const bool _IsLoop)
	{
		//  指定されたタイプのサウンドのインスタンスを生成
		std::shared_ptr<DirectX::SoundEffectInstance> instance = singleton->soundEffect[_Track]->CreateInstance();
		//  ボリュームを現在のボリュームに合わせて設定
		instance->SetVolume(singleton->firstVolume[_Track] * singleton->volumeMagnification);
		//  サウンドキュー内にインスタンスを格納
		singleton->soundQue[_Track].emplace_back(instance);
		//  再生
		singleton->soundQue[_Track].back()->Play(_IsLoop);
	}
}