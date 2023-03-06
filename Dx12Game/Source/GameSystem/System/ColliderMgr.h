#pragma once
#include <unordered_map>
#include <vector>
#include "Tag.h"

namespace Component
{
	class CSphColl;
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

		static void AddCollider(Component::CSphColl* _SphColl);		// 当たり判定の追加
		static void RemoveCollider(Component::CSphColl* _SphColl);	// 当たり判定の削除
		static void RemoveAllCollider();							// あたり判定をすべて削除

	private:
		std::unordered_map<Tag, std::vector<Component::CSphColl*>> colliderData;	// 当たり判定データ
		std::vector<Component::CSphColl*> pendingData;								// 当たり判定の保留データ

		/// <summary> 球状当たり判定が衝突しているか? </summary>
		static bool DetectionTwoSpheres(const Component::CSphColl& _Sph1, const Component::CSphColl &_Sph2);

		/// <summary> 球場当たり判定のめり込みを修正 </summary>
		static void FixDetectionTwoSpheres(Component::CSphColl& _Sph1, const Component::CSphColl& _Sph2);
	};
}