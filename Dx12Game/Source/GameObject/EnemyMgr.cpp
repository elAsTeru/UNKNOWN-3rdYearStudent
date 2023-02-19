#include "EnemyMgr.h"

#include <Rapidjson/document.h>
#include <Rapidjson/istreamwrapper.h>
#include <fstream>
#include <Rapidjson/error/en.h>
#include <iostream>

// 個別設定が入る敵
#include "ThroughEnemy.h"
#include "Boss.h"

namespace GameObject
{
	EnemyMgr::EnemyMgr() :
		Base(Tag::Untagged, "EnemyMgr"),
		spawnCount(0),
		phaseCount(0),
		enemyData()
	{
	}

	EnemyMgr::~EnemyMgr()
	{
		
	}

	bool EnemyMgr::Update(const float& _ElapsedTime)
	{
		// データが空だったら出る
		if (enemyData.empty())
		{ return false; }

		while (1)
		{
			// スポーン回数がデータ数以上なら出る
			if (spawnCount >= enemyData[phaseCount].size())
			{ return true; }

			// 次にスポーンする敵の時間を経過時間が越えていなければ
			if (!(_ElapsedTime >= enemyData[phaseCount][spawnCount].spawnTime))
			{ return false; }

			// 敵の種類を判定＆無効中の敵を取得
			GameObject::Base* enemy{};
			GameObject::ThroughEnemy* throughTemp;
			GameObject::Boss* bossTemp;
			switch (enemyData[phaseCount][spawnCount].kind)
			{
			case 0:
				enemy = GameObject::Mgr::FindDeactiveObj("Rander");
				break;
			case 1:
				enemy = GameObject::Mgr::FindDeactiveObj("Reverser");
				break;
			case 2:
				enemy = GameObject::Mgr::FindDeactiveObj("SlowTracker");
				break;
			case 3:
				// 直進のみ
				throughTemp = static_cast<GameObject::ThroughEnemy*>(GameObject::Mgr::FindDeactiveObj("ThroughEnemy"));
				throughTemp->isInvincible = false;
				enemy = throughTemp;
				break;
			case 4:
				enemy = GameObject::Mgr::FindDeactiveObj("AvoidTracker");
				break;
			case 5:
				// 無敵化したものなので上のとは別物
				throughTemp = static_cast<GameObject::ThroughEnemy*>(GameObject::Mgr::FindDeactiveObj("ThroughEnemy"));
				throughTemp->isInvincible = true;
				enemy = throughTemp;
				break;
			case 6:
				bossTemp = static_cast<GameObject::Boss*>(GameObject::Mgr::FindDeactiveObj("Boss"));
				bossTemp->SetLevel(1);
				enemy = bossTemp;
				break;
			case 7:
				bossTemp = static_cast<GameObject::Boss*>(GameObject::Mgr::FindDeactiveObj("Boss"));
				bossTemp->SetLevel(2);
				enemy = bossTemp;
				break;
			case 8:
				bossTemp = static_cast<GameObject::Boss*>(GameObject::Mgr::FindDeactiveObj("Boss"));
				bossTemp->SetLevel(3);
				enemy = bossTemp;
				break;
			}
			// 敵をスポーンさせる
			if (enemy)
			{
				enemy->Init();
				enemy->transform->position = enemyData[phaseCount][spawnCount].pos;
				enemy->transform->rotation = enemyData[phaseCount][spawnCount].rot;
				enemy->SetActive(true);
				// スポーン回数を増やす
				++spawnCount;
			}
		}
		return false;
	}

	bool EnemyMgr::Load(std::string _JsonFilePath)
	{
		std::ifstream ifs(_JsonFilePath);
		rapidjson::IStreamWrapper isw(ifs);
		// JSONファイル解析
		rapidjson::Document doc;
		doc.ParseStream(isw);
		// 読み込みチェック
		if (doc.HasParseError()) {
			std::cout << "error offset:" << doc.GetErrorOffset() << std::endl;
			std::cout << "error pase:" << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
			// 読み込み失敗
			return false;
		}
		rapidjson::Value& data = doc;

		size_t phase = data.MemberCount();		// フェーズ数
		enemyData.reserve(phase);				// フェーズ数分のメモリ確保
		
		// 各フェーズごとにデータ読み込み
		for (uint32_t i = 0; i < phase; ++i)
		{
			// iをフェーズ検索キーに変更
			const std::string TempPhaseKey = std::to_string(i);
			const char* PhaseKey = TempPhaseKey.c_str();

			size_t dataCount = data[PhaseKey].MemberCount();	// データ数
			enemyData[i].reserve(dataCount);					// データ数分のメモリ確保

			for (size_t j = 0; j < dataCount; ++j)
			{
				EnemyData ed{};	//敵データ
				// jをデータ検索キーに変更
				const std::string TempDataKey = std::to_string(j);
				const char* DataKey = TempDataKey.c_str();
				// データ格納
				ed.kind  = data[PhaseKey][DataKey].GetArray()[0].GetInt();						// 種類
				ed.pos.x = data[PhaseKey][DataKey].GetArray()[1].GetFloat();					// 位置X
				ed.pos.y = data[PhaseKey][DataKey].GetArray()[2].GetFloat();					// 位置Y
				ed.pos.z = data[PhaseKey][DataKey].GetArray()[3].GetFloat();					// 位置Z
				ed.rot.x = MyMath::ToRadians(data[PhaseKey][DataKey].GetArray()[4].GetFloat());	// 向きX Dig → Rad
				ed.rot.y = MyMath::ToRadians(data[PhaseKey][DataKey].GetArray()[5].GetFloat());	// 向きY Dig → Rad
				ed.rot.z = MyMath::ToRadians(data[PhaseKey][DataKey].GetArray()[6].GetFloat());	// 向きZ Dig → Rad
				ed.spawnTime = data[PhaseKey][DataKey].GetArray()[7].GetFloat();				// 出現時間
				// データを追加
				enemyData[i].emplace_back(ed);
			}
			enemyData[i].shrink_to_fit();
		}
		// 正常終了
		return true;
	}

	bool EnemyMgr::Next()
	{
		// 出現回数が
		if ((spawnCount >= enemyData[phaseCount].size()) && ((phaseCount + 1) < enemyData.size()))
		{
			++phaseCount;
			spawnCount = 0;

			return true;
		}
		return false;
	}
}