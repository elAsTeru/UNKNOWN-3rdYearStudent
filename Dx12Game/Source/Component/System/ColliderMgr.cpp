#include <cmath>
#include "ColliderMgr.h"
#include "GameObjectBase.h"
#include "Transform.h"
#include "SphereCollider.h"

namespace MySys
{
	ColliderMgr* ColliderMgr::singleton = nullptr;

	ColliderMgr::ColliderMgr() :
		colliderData(),
		pendingData()
	{
		singleton = nullptr;
	}

	ColliderMgr::~ColliderMgr()
	{

	}

	/// <param name="_Size">プールのサイズを設定 / 設定無しは0</param>
	void ColliderMgr::OnInit()
	{
		if (!singleton)
		{
			singleton = new ColliderMgr;
		}
	}

	void ColliderMgr::OnTerm()
	{
		if (singleton)
		{
			// プールを破棄
			RemoveAllCollider();

			// 当たり判定管理終了
			delete singleton;
			singleton = nullptr;
		}
	}

	// 当たり判定対象　作成のためのメモ
	//
	//	この判定は　敵が攻撃した→プレイヤーに	のような順で判定を行うため
	//	影響を与える側のみの当たり判定関数を使用したい
	// 
	//			　＼| p | e | s | b |
	//	player		|＼ |× |× |× |	プレイヤーは誰にも影響を与えない
	//	enemy		|〇 |〇 |× |× |	敵はプレイヤーに影響を与える
	//	bullet		|× |〇 |＼ |× |	弾は敵に影響を与える
	//	background	|〇 |〇 |〇 |＼ |	背景は全てに影響を与える(今壁はハードコーディング)

	void ColliderMgr::Update()
	{
		for (auto enemyColl : singleton->colliderData[Tag::Enemy])
		{
			if (!enemyColl->isEnable || !enemyColl->parent->GetActive())
			{ continue; }

			// Enemy → Player のあたり判定
			for (auto playerColl : singleton->colliderData[Tag::Player])
			{
				if (!playerColl->isEnable || !playerColl->parent->GetActive())
				{ continue; }

				if (JudgTwoSpheres(*enemyColl, *playerColl))
				{
					enemyColl->parent->OnTriggerEnter(playerColl->parent);
					playerColl->parent->OnTriggerEnter(enemyColl->parent);
				}
			}

			// Enemy → Enemy の衝突判定
			for (auto enemyColl2 : singleton->colliderData[Tag::Enemy])
			{
				// 有効でない || 同じオブジェクト
				if ((!enemyColl2->isEnable || !enemyColl2->parent->GetActive()) || (enemyColl == enemyColl2))
				{ continue; }

				if (JudgTwoSpheres(*enemyColl, *enemyColl2))
				{
					enemyColl->parent->OnTriggerEnter(enemyColl2->parent);
					enemyColl2->parent->OnTriggerEnter(enemyColl->parent);

					// どちらもトリガーじゃ無ければ
					if (!enemyColl->parent->GetComponent<Component::SphColl>()->isTrigger && !enemyColl2->parent->GetComponent<Component::SphColl>()->isTrigger)
					{
						// めり込みを戻す
						FixSinkingSpheres(*enemyColl, *enemyColl2);
					}
				}
			}

			// Enemy → Bullet
			for (auto bulletColl : singleton->colliderData[Tag::Bullet])
			{
				if (!bulletColl->isEnable || !bulletColl->parent->GetActive())
				{ continue; }

				if (JudgTwoSpheres(*enemyColl, *bulletColl))
				{
					enemyColl->parent->OnTriggerEnter(bulletColl->parent);
					bulletColl->parent->OnTriggerEnter(enemyColl->parent);
				}
			}
		}

		for (auto bulletColl : singleton->colliderData[Tag::Bullet])
		{
			if (!bulletColl->isEnable || !bulletColl->parent->GetActive())
			{ continue; }

			// Bullet → Bullet の衝突判定
			for (auto bulletColl2 : singleton->colliderData[Tag::Bullet])
			{
				// 有効でない || 同じオブジェクト
				if ((!bulletColl2->isEnable || !bulletColl2->parent->GetActive()) || (bulletColl == bulletColl2))
				{
					continue;
				}

				if (JudgTwoSpheres(*bulletColl, *bulletColl2))
				{
					bulletColl->parent->OnTriggerEnter(bulletColl2->parent);
					bulletColl2->parent->OnTriggerEnter(bulletColl->parent);
				}
			}


			// Bullet → Player の衝突判定
			for (auto player : singleton->colliderData[Tag::Player])
			{
				// 有効でない
				if ((!player->isEnable || !player->parent->GetActive()))
				{
					continue;
				}

				if (JudgTwoSpheres(*bulletColl, *player))
				{
					bulletColl->parent->OnTriggerEnter(player->parent);
					player->parent->OnTriggerEnter(bulletColl->parent);
				}
			}
		}

		//// 全てのオブジェクトの衝突判定
		//// これを使いたいが、処理を軽くするために上を使用
		//for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		//{
		//	// データ無し → 次に進む
		//	if (singleton->colliderData[tag].empty())
		//	{ continue; }
		// 
		//	for (auto& collA : singleton->colliderData[tag])
		//	{
		//		// collAが無効 → 次に進む
		//		if (!collA->isEnable || !collA->parent->GetActive())
		//		{ continue; }
		// 
		//		for (auto& collB : singleton->colliderData[tag])
		//		{
		//			// collBが無効 or collAと同じオブジェクト → 次に進む
		//			if ((!collA->isEnable || !collA->parent->GetActive()) || (collA == collB))
		//			{ continue; }
		// 
		//			// collAとcollBの衝突判定を行い、衝突してるなら
		//			if (JudgTwoSpheres(*collA, *collB))
		//			{
		//				// めり込みを戻す場合
		//				if (!collA->parent->GetComponent<Component::SphColl>()->isTrigger
		//					&& !collB->parent->GetComponent<Component::SphColl>()->isTrigger)
		//				{
		//					FixSinkingSpheres(*collA, *collB);
		//				}
		//				// トリガーの場合
		//				else
		//				{
		//					// それぞれに衝突したことを伝える
		//					collA->parent->OnTriggerEnter(collB->parent);
		//					collB->parent->OnTriggerEnter(collA->parent);
		//				}
		//			}
		//		}
		//	}
		//}

		// 保留中の当たり判定を判定対象に追加
		if (!singleton->pendingData.empty())
		{
			for (auto pending : singleton->pendingData)
			{
				Tag t = pending->parent->tag;
				singleton->colliderData[t].emplace_back(pending);
			}
			singleton->pendingData.clear();
		}
	}

	void ColliderMgr::AddCollider(Component::SphColl* _SphColl)
	{
		singleton->pendingData.emplace_back(_SphColl);
	}

	void ColliderMgr::RemoveCollider(Component::SphColl* _SphColl)
	{
		// ?対象のオブジェクトは保留データ内にあるか
		auto iter = std::find(singleton->pendingData.begin(), singleton->pendingData.end(), _SphColl);
		if(iter != singleton->pendingData.end())
		{
			// 保留データ内末尾にデータを移動し、排除する
			std::iter_swap(iter, singleton->pendingData.end() - 1);
			singleton->pendingData.pop_back();
			return;
		}

		// ?対象のオブジェクトは有効データ内にあるか
		Tag tag = _SphColl->parent->tag;
		iter = std::find(singleton->colliderData[tag].begin(), singleton->colliderData[tag].end(), _SphColl);
		if (iter != singleton->colliderData[tag].end())
		{
			// 有効データ内末尾にデータを移動し、排除する
			std::iter_swap(iter, singleton->colliderData[tag].end() - 1);
			singleton->colliderData[tag].pop_back();
		}
	}

	void ColliderMgr::RemoveAllCollider()
	{
		// 有効データ領域
		for (auto tag = Tag::Begin; tag != Tag::End; ++tag)
		{
			while (!singleton->colliderData[tag].empty())
			{
				singleton->colliderData[tag].pop_back();
			}
		}
		// 保留データ領域
		while (!singleton->pendingData.empty())
		{
			singleton->pendingData.pop_back();
		}
	}

	bool MySys::ColliderMgr::JudgTwoSpheres(const Component::SphColl& _Sph1, const Component::SphColl&_Sph2)
	{
		// 二つの半径の合計
		float sumRadius = _Sph1.radius + _Sph2.radius;
		// 二つの座標の距離
		Vector3 dist = *_Sph2.center - *_Sph1.center;

		// あたり判定式
		if ((pow(dist.x,2) + pow(dist.y, 2) + pow(dist.z, 2)) <= pow((sumRadius),2))
		{
			return true;
		}
		return false;
	}
	void ColliderMgr::FixSinkingSpheres(Component::SphColl& _Sph1, const Component::SphColl& _Sph2)
	{
		// 必要要素(Aのpos / Bのpos / AとBの半径の合計)
		Vector3& posA = *_Sph1.center;
		Vector3 posB = *_Sph2.center;
		float sumRadius = _Sph1.radius + _Sph2.radius;
		
		// 2つのオブジェクトの距離を求める
		Vector3 vecBA = posA - posB;

		// 条件式の修正が必要
		if (!(vecBA.x == 0 && vecBA.y == 0 && vecBA.z == 0))
		{
			// 正規化するために斜辺を求める
			float hypotenuse = static_cast<float>(std::sqrt(std::pow(vecBA.x, 2) + std::pow(vecBA.y, 2) + std::pow(vecBA.z, 2)));
			// 正規化したベクトル
			Vector3 NorBA = vecBA / hypotenuse;
			// Aオブジェクトの座標のめり込みを修正する
			posA = posB + (NorBA * sumRadius);
		}
	}
}