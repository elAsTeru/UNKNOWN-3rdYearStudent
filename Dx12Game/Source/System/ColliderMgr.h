#pragma once
#include <unordered_map>
#include <vector>
#include "Tag.h"

namespace Component
{
	class SphColl;
}

namespace Sys
{
	class ColliderMgr
	{
		ColliderMgr();
		~ColliderMgr();
		static ColliderMgr* singleton;

	public:
		//Public Method
		static void OnInit();			// 初期化
		static void OnTerm();			// 終了
		static void Update();			// あたり判定の更新(衝突していたら個々が所有する衝突処理を実行する)

		static void AddCollider(Component::SphColl* _SphColl);		// 当たり判定の追加
		static void RemoveCollider(Component::SphColl* _SphColl);	// 当たり判定の削除
		static void RemoveAllCollider();							// あたり判定をすべて削除

	private:
		std::unordered_map<Tag, std::vector<Component::SphColl*>> colliderData;	// 当たり判定データ
		std::vector<Component::SphColl*> pendingData;							// 当たり判定の保留データ

		/// <summary>
		/// 球のあたり判定1,2が衝突していないか?
		/// </summary>
		/// <returns>true:衝突 / false:未衝突</returns>
		static bool JudgTwoSpheres(const Component::SphColl& _Sph1, const Component::SphColl &_Sph2);
		
		/// <summary>
		/// 球のめり込みを修正する
		/// </summary>
		static void FixSinkingSpheres(Component::SphColl& _Sph1, const Component::SphColl& _Sph2);
	};
}